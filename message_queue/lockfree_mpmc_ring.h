#pragma once
#include <atomic>
#include <vector>

namespace destiny {

/**
 * @brief MPMC (Multiple Producer Multiple Consumer) 无锁环形队列
 *
 * 设计要求：
 * - 线程模型：多个生产者线程，多个消费者线程
 * - 容量限制：有界，基于预分配的环形缓冲区
 * - 阻塞模式：非阻塞，所有操作立即返回
 * - 数据结构：环形缓冲区 + 原子计数器/指针
 * - 同步机制：CAS操作，无互斥锁
 * - 性能特点：真正的极高性能，无动态内存分配
 *
 * 核心设计理念：
 * - 预分配环形缓冲区，完全避免动态内存分配
 * - 使用原子序列号来解决ABA问题和竞争条件
 * - 每个槽位有独立的序列号，实现细粒度同步
 * - 生产者和消费者可完全并行操作
 * - 真正的lock-free和wait-free特性
 *
 * 算法核心：
 * 1. 序列号机制：每个槽位关联一个序列号，表示该槽位的状态
 * 2. 生产者协议：只能写入序列号匹配的空槽位
 * 3. 消费者协议：只能读取序列号匹配的满槽位
 * 4. 环形推进：头尾指针自动环绕，无需模运算
 * 5. ABA避免：序列号的单调递增特性天然避免ABA问题
 *
 * 序列号状态：
 * - sequence[i] == i: 槽位为空，等待生产者写入
 * - sequence[i] == i+1: 槽位已满，等待消费者读取
 * - 生产者将 sequence[i] 从 i 更新为 i+1
 * - 消费者将 sequence[i] 从 i+1 更新为 i+capacity
 *
 * 性能特点：
 * - **吞吐量**：极高（无锁+无动态分配+缓存友好）
 * - **延迟**：极低（无线程阻塞+预分配内存）
 * - **CPU使用**：中等（CAS失败时可能忙等）
 * - **内存使用**：固定（预分配，不会增长）
 * - **缓存效果**：优秀（连续内存布局）
 *
 * 实现挑战：
 * 1. 序列号溢出：需要处理长期运行的溢出情况
 * 2. False sharing：需要适当的缓存行对齐
 * 3. 内存排序：精确的memory_order选择对性能关键
 * 4. 公平性：避免某些线程饥饿
 *
 * 适用场景：
 * - 极高性能要求的系统（HFT、网络包处理等）
 * - 实时系统（低延迟要求）
 * - 高频数据采集和处理
 * - 系统级基础设施组件
 * - 网络协议栈（如DPDK应用）
 *
 * 与无界版本对比：
 * - 优势：极高性能，无内存分配瓶颈，可预测的性能
 * - 劣势：容量固定，满时会拒绝写入
 * - 定位：性能王者，生产环境的最优选择
 */
template <typename T>
class LockFreeMPMCRing {
public:
    explicit LockFreeMPMCRing(std::size_t capacity);
    ~LockFreeMPMCRing() = default;

    // 删除拷贝构造和赋值
    LockFreeMPMCRing(const LockFreeMPMCRing&) = delete;
    LockFreeMPMCRing& operator=(const LockFreeMPMCRing&) = delete;

    // 非阻塞推送（满时返回false）
    bool push(const T& item);
    bool push(T&& item);

    // 非阻塞弹出（空时返回false）
    bool pop(T& item);

    // 查询方法（注意：在高并发下结果可能瞬间失效）
    bool empty() const;
    bool full() const;
    std::size_t size() const;       // 近似值，仅用于监控
    std::size_t capacity() const;

private:
    // 内部辅助方法
    template <typename U>
    bool push_internal(U&& item);

    // 缓存行大小（避免false sharing）
    static constexpr std::size_t CACHE_LINE_SIZE = 64;

private:
    // 环形缓冲区结构
    struct alignas(CACHE_LINE_SIZE) Slot {
        std::atomic<std::size_t> sequence;
        T data;
        
        Slot() : sequence(0) {}
    };

    // 对齐到缓存行，避免false sharing
    alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> head_seq_;     // 生产者序列号
    alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> tail_seq_;     // 消费者序列号
    
    const std::size_t capacity_;                                     // 队列容量
    const std::size_t mask_;                                         // 容量掩码（capacity_-1）
    std::vector<Slot> buffer_;                                       // 环形缓冲区
};

// 实现部分

template <typename T>
LockFreeMPMCRing<T>::LockFreeMPMCRing(std::size_t capacity)
    : capacity_(capacity),
      mask_(capacity - 1),
      buffer_(capacity),
      head_seq_(0),
      tail_seq_(0) {
    
    // 容量必须是2的幂，以便使用位掩码优化
    if (capacity == 0 || (capacity & (capacity - 1)) != 0) {
        throw std::invalid_argument("LockFreeMPMCRing capacity must be a power of 2 and > 0");
    }

    // 初始化序列号
    for (std::size_t i = 0; i < capacity_; ++i) {
        buffer_[i].sequence.store(i, std::memory_order_relaxed);
    }
}

template <typename T>
bool LockFreeMPMCRing<T>::push(const T& item) {
    return push_internal(item);
}

template <typename T>
bool LockFreeMPMCRing<T>::push(T&& item) {
    return push_internal(std::move(item));
}

template <typename T>
template <typename U>
bool LockFreeMPMCRing<T>::push_internal(U&& item) {
    std::size_t head = head_seq_.load(std::memory_order_relaxed);
    
    while (true) {
        Slot& slot = buffer_[head & mask_];
        std::size_t seq = slot.sequence.load(std::memory_order_acquire);
        
        if (seq == head) {
            // 槽位可用，尝试占用
            if (head_seq_.compare_exchange_weak(
                    head, head + 1, 
                    std::memory_order_relaxed, 
                    std::memory_order_relaxed)) {
                
                // 成功占用槽位，写入数据
                slot.data = std::forward<U>(item);
                
                // 标记槽位为已满（序列号+1）
                slot.sequence.store(head + 1, std::memory_order_release);
                return true;
            }
        } else if (seq < head) {
            // 槽位已被占用但数据还未写入，重试
            head = head_seq_.load(std::memory_order_relaxed);
        } else {
            // 队列已满 (seq > head)
            return false;
        }
    }
}

template <typename T>
bool LockFreeMPMCRing<T>::pop(T& item) {
    std::size_t tail = tail_seq_.load(std::memory_order_relaxed);
    
    while (true) {
        Slot& slot = buffer_[tail & mask_];
        std::size_t seq = slot.sequence.load(std::memory_order_acquire);
        
        if (seq == tail + 1) {
            // 槽位有数据，尝试占用
            if (tail_seq_.compare_exchange_weak(
                    tail, tail + 1,
                    std::memory_order_relaxed,
                    std::memory_order_relaxed)) {
                
                // 成功占用槽位，读取数据
                item = std::move(slot.data);
                
                // 标记槽位为空闲（序列号增加capacity_）
                slot.sequence.store(tail + capacity_, std::memory_order_release);
                return true;
            }
        } else if (seq < tail + 1) {
            // 槽位数据已被消费但序列号还未更新，重试
            tail = tail_seq_.load(std::memory_order_relaxed);
        } else {
            // 队列为空 (seq > tail + 1)
            return false;
        }
    }
}

template <typename T>
bool LockFreeMPMCRing<T>::empty() const {
    std::size_t head = head_seq_.load(std::memory_order_acquire);
    std::size_t tail = tail_seq_.load(std::memory_order_acquire);
    return head == tail;
}

template <typename T>
bool LockFreeMPMCRing<T>::full() const {
    std::size_t head = head_seq_.load(std::memory_order_acquire);
    std::size_t tail = tail_seq_.load(std::memory_order_acquire);
    return (head - tail) >= capacity_;
}

template <typename T>
std::size_t LockFreeMPMCRing<T>::size() const {
    std::size_t head = head_seq_.load(std::memory_order_acquire);
    std::size_t tail = tail_seq_.load(std::memory_order_acquire);
    return head - tail;
}

template <typename T>
std::size_t LockFreeMPMCRing<T>::capacity() const {
    return capacity_;
}

}  // namespace destiny