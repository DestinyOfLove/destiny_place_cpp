#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

namespace destiny {

/**
 * @brief MPMC (Multiple Producer Multiple Consumer) 无界队列
 *
 * 设计要求：
 * - 线程模型：多个生产者线程，多个消费者线程
 * - 容量限制：无界，仅受系统内存限制
 * - 阻塞模式：阻塞式，仅在队列空时消费者阻塞
 * - 数据结构：std::queue（底层使用std::deque）
 * - 同步机制：单一互斥锁 + 条件变量
 * - 性能特点：生产者永不阻塞，内存使用弹性
 *
 * 核心设计理念：
 * - 无容量限制，生产者永远不会因为队列满而阻塞
 * - 适合生产速度不可控或突发性流量的场景
 * - std::queue自动管理内存，无需预分配
 * - 仅消费者在队列空时阻塞等待
 * - 内存使用随队列长度动态变化
 *
 * 实现要点：
 * 1. 无界设计：没有容量检查，push永远成功（除非关闭）
 * 2. 单锁简化：只需一个互斥锁保护queue和状态
 * 3. 单条件变量：只需not_empty，无需not_full
 * 4. 内存管理：依赖std::queue的动态内存管理
 * 5. 风险控制：需要监控内存使用，防止无限增长
 *
 * 使用注意事项：
 * - 内存风险：如果生产速度持续超过消费速度，内存会无限增长
 * - 性能考虑：队列过长时，cache miss增加，性能下降
 * - 监控需求：需要监控队列长度和内存使用
 * - 流控机制：应用层应实现适当的流量控制
 *
 * 适用场景：
 * - 事件收集系统（日志、监控数据等）
 * - 消息中间件的缓冲区
 * - 突发流量处理（流量波峰缓冲）
 * - 异步任务队列（允许积压）
 * - 生产速度不可控的场景
 *
 * 性能特点：
 * - 吞吐量：中等（单锁限制，但无容量检查开销）
 * - 延迟：中等（涉及锁竞争，但生产者无阻塞）
 * - CPU使用：低（仅消费者可能休眠）
 * - 内存使用：弹性（动态增长，有内存泄漏风险）
 * - 生产者延迟：极低（永不阻塞）
 *
 * 与有界版本对比：
 * - 优势：生产者永不阻塞，处理突发流量能力强
 * - 劣势：内存使用不可控，可能导致系统OOM
 */
template <typename T>
class MPMCUnboundedQueue {
public:
    // 无需容量参数的构造函数
    MPMCUnboundedQueue() = default;

    // 删除拷贝构造和赋值
    MPMCUnboundedQueue(const MPMCUnboundedQueue&) = delete;
    MPMCUnboundedQueue& operator=(const MPMCUnboundedQueue&) = delete;

    // 推送操作（永不阻塞，除非关闭）
    bool push(const T& item);
    bool push(T&& item);

    // 弹出操作（空时阻塞）
    bool pop(T& item);

    // 查询方法
    bool isEmpty() const;
    std::size_t size() const;
    bool isClosed() const;

    // 关闭队列
    void close();

    // 内存管理辅助
    void clear();           // 清空队列（用于内存回收）
    void shrink_to_fit();   // 请求内存收缩（取决于实现）

private:
    // 内部辅助方法
    template <typename U>
    bool push_internal(U&& item);

private:
    std::queue<T> queue_;               // 底层无界队列

    mutable std::mutex mutex_;          // 保护所有共享状态
    std::condition_variable not_empty_; // 队列非空条件（无需not_full）
    bool closed_ = false;               // 关闭标志，受 mutex_ 保护
};

// 实现部分

template <typename T>
bool MPMCUnboundedQueue<T>::push(const T& item) {
    return push_internal(item);
}

template <typename T>
bool MPMCUnboundedQueue<T>::push(T&& item) {
    return push_internal(std::move(item));
}

template <typename T>
template <typename U>
bool MPMCUnboundedQueue<T>::push_internal(U&& item) {
    std::lock_guard<std::mutex> lock(mutex_);

    // 检查是否已关闭
    if (closed_) {
        return false;
    }

    // 无界队列，直接推送（永不因为容量而失败）
    queue_.push(std::forward<U>(item));

    // 通知一个等待的消费者
    not_empty_.notify_one();
    return true;
}

template <typename T>
bool MPMCUnboundedQueue<T>::pop(T& item) {
    std::unique_lock<std::mutex> lock(mutex_);

    // 等待队列有数据，或者队列关闭
    not_empty_.wait(lock, [this] {
        return !queue_.empty() || closed_;
    });

    // 如果队列为空，说明是因为关闭而退出
    if (queue_.empty()) {
        return false;
    }

    // 取出元素
    item = std::move(queue_.front());
    queue_.pop();

    // 无界队列，无需通知生产者（他们永不等待）
    return true;
}

template <typename T>
bool MPMCUnboundedQueue<T>::isEmpty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

template <typename T>
std::size_t MPMCUnboundedQueue<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

template <typename T>
bool MPMCUnboundedQueue<T>::isClosed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return closed_;
}

template <typename T>
void MPMCUnboundedQueue<T>::close() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
    }
    // 只需唤醒等待的消费者（生产者永不等待）
    not_empty_.notify_all();
}

template <typename T>
void MPMCUnboundedQueue<T>::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    // 清空队列，释放内存
    std::queue<T> empty_queue;
    queue_.swap(empty_queue);
    // empty_queue 在此处析构，释放内存
}

template <typename T>
void MPMCUnboundedQueue<T>::shrink_to_fit() {
    // std::queue基于std::deque，没有shrink_to_fit
    // 这个方法主要用于接口一致性，实际效果有限
    // 如果需要真正的内存回收，使用clear()方法
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 通过临时队列来强制内存收缩
    if (!queue_.empty()) {
        std::queue<T> temp_queue;
        temp_queue.swap(queue_);
        
        // 重新构建队列，可能减少内存占用
        while (!temp_queue.empty()) {
            queue_.push(std::move(temp_queue.front()));
            temp_queue.pop();
        }
    }
}

}  // namespace destiny