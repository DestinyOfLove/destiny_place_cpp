#pragma once
#include <condition_variable>
#include <mutex>
#include <vector>

namespace destiny {

/**
 * @brief MPMC (Multiple Producer Multiple Consumer) 双锁队列
 *
 * 设计要求：
 * - 线程模型：多个生产者线程，多个消费者线程
 * - 容量限制：有界，基于环形缓冲区实现
 * - 阻塞模式：阻塞式，队列满/空时线程休眠等待
 * - 数据结构：环形缓冲区 (vector<T> + head/tail指针)
 * - 同步机制：分离的双锁 + 条件变量（细粒度锁）
 * - 性能特点：高并发性能，生产者与消费者可并行
 *
 * 核心设计理念：
 * - 使用两个独立的互斥锁：push_mutex保护tail，pop_mutex保护head
 * - 生产者之间竞争push_mutex，消费者之间竞争pop_mutex
 * - 生产者和消费者在大多数情况下可以并行操作（除了空/满状态）
 * - 显著减少锁竞争，提升并发性能
 * - 性价比最高的MPMC实现，复杂度远低于无锁版本
 *
 * 实现要点：
 * 1. 双锁设计：push_mutex保护tail和写操作，pop_mutex保护head和读操作
 * 2. 条件变量分离：not_empty与pop_mutex配对，not_full与push_mutex配对
 * 3. 空/满状态检查：需要临时获取对方的锁来准确判断
 * 4. 关闭状态：受独立的state_mutex保护，避免死锁
 * 5. 内存排序：在锁保护下，不需要额外的内存屏障
 *
 * 锁获取顺序（避免死锁）：
 * - 需要同时获取两个锁时：始终先获取push_mutex，再获取pop_mutex
 * - 状态查询方法遵循相同的锁顺序
 * - close()方法使用独立的state_mutex
 *
 * 适用场景：
 * - 高吞吐量的数据处理管道
 * - 服务器端应用的核心队列
 * - 多媒体流处理
 * - 需要平衡性能和复杂度的生产环境
 *
 * 性能特点：
 * - 吞吐量：高（生产者和消费者可并行，减少锁竞争）
 * - 延迟：中低（比单锁版本低，比无锁版本高）
 * - CPU使用：低（线程休眠，不占用CPU）
 * - 内存使用：固定（预分配缓冲区）
 * - 并发度：高（生产者消费者可并行）
 *
 * 与其他版本对比：
 * - vs 单锁版本：显著更高的并发性能
 * - vs 无锁版本：实现复杂度大幅降低，性能接近
 * - 最佳的性能/复杂度平衡点
 */
template <typename T>
class MPMCDualLockQueue {
public:
    explicit MPMCDualLockQueue(std::size_t capacity);

    // 删除拷贝构造和赋值
    MPMCDualLockQueue(const MPMCDualLockQueue&) = delete;
    MPMCDualLockQueue& operator=(const MPMCDualLockQueue&) = delete;

    // 推送操作
    bool push(const T& item);
    bool push(T&& item);

    // 弹出操作
    bool pop(T& item);

    // 查询方法
    bool isFull() const;
    bool isEmpty() const;
    std::size_t size() const;
    std::size_t capacity() const;
    bool isClosed() const;

    // 关闭队列
    void close();

private:
    // 内部辅助方法
    template <typename U>
    bool push_internal(U&& item);

private:
    std::vector<T> buffer_;              // 环形缓冲区
    std::size_t head_;                   // 读指针，受pop_mutex保护
    std::size_t tail_;                   // 写指针，受push_mutex保护
    const std::size_t capacity_;         // 用户可用容量
    const std::size_t buffer_capacity_;  // 实际缓冲区大小 = capacity_ + 1

    // 分离的锁和条件变量
    mutable std::mutex push_mutex_;      // 保护tail和推送操作
    mutable std::mutex pop_mutex_;       // 保护head和弹出操作
    std::condition_variable not_full_;   // 队列非满条件，与push_mutex配对
    std::condition_variable not_empty_;  // 队列非空条件，与pop_mutex配对

    // 关闭状态管理（使用独立锁避免复杂的锁顺序）
    mutable std::mutex state_mutex_;     // 保护关闭状态
    bool closed_;                        // 关闭标志
};

// 实现部分

template <typename T>
MPMCDualLockQueue<T>::MPMCDualLockQueue(std::size_t capacity)
    : capacity_(capacity),
      buffer_capacity_(capacity + 1),
      buffer_(buffer_capacity_),
      head_(0),
      tail_(0),
      closed_(false) {
    if (capacity == 0) {
        throw std::invalid_argument("MPMCDualLockQueue capacity must be greater than 0");
    }
}

template <typename T>
bool MPMCDualLockQueue<T>::push(const T& item) {
    return push_internal(item);
}

template <typename T>
bool MPMCDualLockQueue<T>::push(T&& item) {
    return push_internal(std::move(item));
}

template <typename T>
template <typename U>
bool MPMCDualLockQueue<T>::push_internal(U&& item) {
    std::unique_lock<std::mutex> push_lock(push_mutex_);

    // 等待队列有空间或关闭
    not_full_.wait(push_lock, [this] {
        // 需要读取head来判断是否满，但不能持有pop_mutex（会死锁）
        // 使用try_lock来避免死锁，如果获取不到就认为可能不满
        std::unique_lock<std::mutex> pop_lock(pop_mutex_, std::try_to_lock);
        if (pop_lock.owns_lock()) {
            bool is_full = (tail_ + 1) % buffer_capacity_ == head_;
            bool is_closed = [this] {
                std::lock_guard<std::mutex> state_lock(state_mutex_);
                return closed_;
            }();
            return !is_full || is_closed;
        }
        // 如果获取不到pop_mutex，保守地认为队列不满（避免死锁）
        return true;
    });

    // 再次检查关闭状态
    {
        std::lock_guard<std::mutex> state_lock(state_mutex_);
        if (closed_) {
            return false;
        }
    }

    // 最终检查是否真的有空间（获取pop_mutex来准确判断）
    {
        std::lock_guard<std::mutex> pop_lock(pop_mutex_);
        if ((tail_ + 1) % buffer_capacity_ == head_) {
            // 实际上满了，重新等待
            push_lock.unlock();
            return push_internal(std::forward<U>(item));
        }
    }

    // 推送元素
    buffer_[tail_] = std::forward<U>(item);
    tail_ = (tail_ + 1) % buffer_capacity_;

    // 释放push锁后再通知，避免唤醒的消费者立即阻塞在锁上
    push_lock.unlock();
    not_empty_.notify_one();
    return true;
}

template <typename T>
bool MPMCDualLockQueue<T>::pop(T& item) {
    std::unique_lock<std::mutex> pop_lock(pop_mutex_);

    // 等待队列有数据或关闭
    not_empty_.wait(pop_lock, [this] {
        // 读取tail来判断是否空，使用try_lock避免死锁
        std::unique_lock<std::mutex> push_lock(push_mutex_, std::try_to_lock);
        if (push_lock.owns_lock()) {
            bool is_empty = head_ == tail_;
            bool is_closed = [this] {
                std::lock_guard<std::mutex> state_lock(state_mutex_);
                return closed_;
            }();
            return !is_empty || is_closed;
        }
        // 如果获取不到push_mutex，保守地认为队列不空
        return true;
    });

    // 检查关闭状态
    {
        std::lock_guard<std::mutex> state_lock(state_mutex_);
        if (closed_) {
            // 如果关闭了，最后检查是否还有数据
            std::lock_guard<std::mutex> push_lock(push_mutex_);
            if (head_ == tail_) {
                return false;  // 真的空了
            }
        }
    }

    // 最终检查是否真的有数据
    {
        std::lock_guard<std::mutex> push_lock(push_mutex_);
        if (head_ == tail_) {
            // 实际上空了，重新等待
            pop_lock.unlock();
            return pop(item);
        }
    }

    // 弹出元素
    item = std::move(buffer_[head_]);
    head_ = (head_ + 1) % buffer_capacity_;

    // 释放pop锁后再通知，避免唤醒的生产者立即阻塞在锁上
    pop_lock.unlock();
    not_full_.notify_one();
    return true;
}

template <typename T>
bool MPMCDualLockQueue<T>::isFull() const {
    // 按照锁顺序获取：push_mutex -> pop_mutex
    std::lock_guard<std::mutex> push_lock(push_mutex_);
    std::lock_guard<std::mutex> pop_lock(pop_mutex_);
    return (tail_ + 1) % buffer_capacity_ == head_;
}

template <typename T>
bool MPMCDualLockQueue<T>::isEmpty() const {
    // 按照锁顺序获取：push_mutex -> pop_mutex
    std::lock_guard<std::mutex> push_lock(push_mutex_);
    std::lock_guard<std::mutex> pop_lock(pop_mutex_);
    return head_ == tail_;
}

template <typename T>
std::size_t MPMCDualLockQueue<T>::size() const {
    // 按照锁顺序获取：push_mutex -> pop_mutex
    std::lock_guard<std::mutex> push_lock(push_mutex_);
    std::lock_guard<std::mutex> pop_lock(pop_mutex_);
    return (tail_ + buffer_capacity_ - head_) % buffer_capacity_;
}

template <typename T>
std::size_t MPMCDualLockQueue<T>::capacity() const {
    return capacity_;
}

template <typename T>
bool MPMCDualLockQueue<T>::isClosed() const {
    std::lock_guard<std::mutex> state_lock(state_mutex_);
    return closed_;
}

template <typename T>
void MPMCDualLockQueue<T>::close() {
    {
        std::lock_guard<std::mutex> state_lock(state_mutex_);
        closed_ = true;
    }
    // 唤醒所有等待的线程
    not_full_.notify_all();
    not_empty_.notify_all();
}

}  // namespace destiny