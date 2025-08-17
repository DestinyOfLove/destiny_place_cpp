#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <vector>

namespace destiny {

/**
 * @brief SPSC (Single Producer Single Consumer) 阻塞队列
 *
 * 设计要求：
 * - 线程模型：一个生产者线程，一个消费者线程
 * - 容量限制：有界，基于环形缓冲区实现
 * - 阻塞模式：阻塞式，队列满/空时线程休眠等待
 * - 数据结构：环形缓冲区 (vector<T> + head/tail指针)
 * - 同步机制：原子操作 + 条件变量
 * - 性能特点：CPU效率优先，避免忙等待
 *
 * 核心设计理念：
 * - 基于SPSC非阻塞的核心算法，添加阻塞等待机制
 * - 生产者：当队列满时阻塞等待，直到有空间可用
 * - 消费者：当队列空时阻塞等待，直到有数据可用
 * - 优雅关闭：支持close()功能，唤醒所有等待线程
 *
 * 实现要点：
 * 1. 原子指针：使用atomic<size_t>管理head/tail指针
 * 2. 内存排序：采用与SPSC非阻塞相同的内存排序策略
 * 3. 条件变量：not_empty用于消费者等待，not_full用于生产者等待
 * 4. 互斥锁：仅用于条件变量，不保护数据操作
 * 5. 关闭状态：原子布尔标志，支持优雅关闭
 *
 * 适用场景：
 * - 生产者-消费者模式，CPU效率优先
 * - 任务队列处理，允许线程休眠
 * - 日志异步写入，后台处理
 * - 中等吞吐量要求，延迟不是关键指标的场景
 *
 * 性能特点：
 * - 吞吐量：高（基于高效的环形缓冲区）
 * - 延迟：中等（涉及线程唤醒开销）
 * - CPU使用：低（线程休眠，不占用CPU）
 * - 内存使用：固定（预分配缓冲区）
 */
template <typename T>
class SPSCBlockingQueue {
public:
    explicit SPSCBlockingQueue(std::size_t capacity);
    bool push(const T& item);
    bool push(T&& item);
    bool pop(T& item);
    bool isFull() const;
    bool isEmpty() const;
    std::size_t size() const;
    std::size_t capacity() const;
    void close();

private:
    // 内部辅助方法
    template <typename U>
    bool push_internal(U&& item);

    std::vector<T> msg_q_;
    std::atomic<std::size_t> head_{};
    std::atomic<std::size_t> tail_{};
    std::size_t user_capacity_;
    std::size_t buffer_capacity_;

    std::condition_variable not_full_;
    std::condition_variable not_empty_;
    std::mutex mtx_;
    std::atomic<bool> is_closed_;
};

template <typename T>
SPSCBlockingQueue<T>::SPSCBlockingQueue(std::size_t capacity)
    : user_capacity_(capacity), buffer_capacity_(capacity + 1), is_closed_(false) {
    msg_q_.resize(buffer_capacity_);
    head_.store(0, std::memory_order_relaxed);
    tail_.store(0, std::memory_order_relaxed);
}

template <typename T>
bool SPSCBlockingQueue<T>::push(const T& item) {
    return push_internal(item);
}

template <typename T>
bool SPSCBlockingQueue<T>::push(T&& item) {
    return push_internal(std::move(item));
}

template <typename T>
template <typename U>
bool SPSCBlockingQueue<T>::push_internal(U&& item) {
    std::unique_lock<std::mutex> lock(mtx_);
    // 等待空间，但要考虑关闭状态
    not_full_.wait(lock, [this] {
        std::size_t tail = tail_.load(std::memory_order_relaxed);
        std::size_t head = head_.load(std::memory_order_acquire);
        return (tail + 1) % buffer_capacity_ != head || is_closed_.load(std::memory_order_acquire);
    });

    // 再次检查关闭状态（可能在等待期间被关闭）
    if (is_closed_.load(std::memory_order_acquire)) {
        return false;
    }

    // 执行插入
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    msg_q_[tail] = std::forward<U>(item);
    tail_.store((tail + 1) % buffer_capacity_, std::memory_order_release);
    not_empty_.notify_one();
    return true;
}

template <typename T>
bool SPSCBlockingQueue<T>::pop(T& item) {
    std::unique_lock<std::mutex> lock(mtx_);

    // 等待数据，或者关闭
    not_empty_.wait(lock, [this] {
        std::size_t head = head_.load(std::memory_order_relaxed);
        std::size_t tail = tail_.load(std::memory_order_acquire);
        return head != tail || is_closed_.load(std::memory_order_acquire);  // 有数据或者已关闭
    });

    // 检查是否是因为关闭而退出等待
    std::size_t head = head_.load(std::memory_order_relaxed);
    std::size_t tail = tail_.load(std::memory_order_acquire);
    if (head == tail) {
        // 队列为空且已关闭
        return false;
    }

    // 读取数据
    item = std::move(msg_q_[head]);
    head_.store((head + 1) % buffer_capacity_, std::memory_order_release);
    not_full_.notify_one();
    return true;
}

template <typename T>
bool SPSCBlockingQueue<T>::isFull() const {
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    std::size_t head = head_.load(std::memory_order_relaxed);
    return (tail + 1) % buffer_capacity_ == head;
}

template <typename T>
bool SPSCBlockingQueue<T>::isEmpty() const {
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    std::size_t head = head_.load(std::memory_order_relaxed);
    return head == tail;
}

template <typename T>
std::size_t SPSCBlockingQueue<T>::size() const {
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    std::size_t head = head_.load(std::memory_order_relaxed);
    return (tail + buffer_capacity_ - head) % buffer_capacity_;
}

template <typename T>
std::size_t SPSCBlockingQueue<T>::capacity() const {
    return user_capacity_;
}

template <typename T>
void SPSCBlockingQueue<T>::close() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        is_closed_.store(true, std::memory_order_release);
    }  // 释放锁后再进行通知，避免通知被阻塞
    not_full_.notify_all();   // 唤醒所有等待的生产者
    not_empty_.notify_all();  // 唤醒所有等待的消费者
}

}  // namespace destiny
