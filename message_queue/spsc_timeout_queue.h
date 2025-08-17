#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <vector>

namespace destiny {

/**
 * @brief SPSC (Single Producer Single Consumer) 超时队列
 *
 * 设计要求：
 * - 线程模型：一个生产者线程，一个消费者线程
 * - 容量限制：有界，基于环形缓冲区实现
 * - 超时模式：支持可选超时，可以设置等待时长
 * - 数据结构：环形缓冲区 (vector<T> + head/tail指针)
 * - 同步机制：原子操作 + 条件变量
 * - 性能特点：平衡性能和灵活性
 *
 * 核心设计理念：
 * - 在阻塞队列基础上增加超时机制
 * - 提供多种推送/弹出接口：阻塞、超时、非阻塞
 * - 生产者：支持无限等待、超时等待、立即返回
 * - 消费者：支持无限等待、超时等待、立即返回
 * - 优雅关闭：支持close()功能，唤醒所有等待线程
 *
 * 接口设计：
 * 1. push(item) - 阻塞推送，直到成功或关闭
 * 2. push_for(item, duration) - 超时推送，等待指定时长
 * 3. try_push(item) - 非阻塞推送，立即返回
 * 4. pop(item) - 阻塞弹出，直到成功或关闭
 * 5. pop_for(item, duration) - 超时弹出，等待指定时长
 * 6. try_pop(item) - 非阻塞弹出，立即返回
 *
 * 适用场景：
 * - 需要灵活控制等待时长的生产者-消费者模式
 * - 网络通信中的超时处理
 * - 批处理系统，等待一定时间收集数据
 * - 需要快速失败机制的任务队列
 *
 * 性能特点：
 * - 吞吐量：高（基于高效的环形缓冲区）
 * - 延迟：可控（通过超时参数调节）
 * - CPU使用：灵活（可选择忙等或休眠）
 * - 内存使用：固定（预分配缓冲区）
 */
template <typename T>
class SPSCTimeoutQueue {
public:
    explicit SPSCTimeoutQueue(std::size_t capacity);

    // 删除拷贝构造和赋值
    SPSCTimeoutQueue(const SPSCTimeoutQueue&) = delete;
    SPSCTimeoutQueue& operator=(const SPSCTimeoutQueue&) = delete;

    // 阻塞式推送（无限等待）
    bool push(const T& item);
    bool push(T&& item);

    // 超时推送（等待指定时长）
    template <typename Rep, typename Period>
    bool push_for(const T& item, const std::chrono::duration<Rep, Period>& timeout_duration);

    template <typename Rep, typename Period>
    bool push_for(T&& item, const std::chrono::duration<Rep, Period>& timeout_duration);

    // 非阻塞推送（立即返回）
    bool try_push(const T& item);
    bool try_push(T&& item);

    // 阻塞式弹出（无限等待）
    bool pop(T& item);

    // 超时弹出（等待指定时长）
    template <typename Rep, typename Period>
    bool pop_for(T& item, const std::chrono::duration<Rep, Period>& timeout_duration);

    // 非阻塞弹出（立即返回）
    bool try_pop(T& item);

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
    bool push_internal(U&& item, bool wait, const std::chrono::milliseconds* timeout = nullptr);

    bool pop_internal(T& item, bool wait, const std::chrono::milliseconds* timeout = nullptr);

private:
    std::vector<T> buffer_;
    std::atomic<std::size_t> head_;
    std::atomic<std::size_t> tail_;
    const std::size_t capacity_;
    const std::size_t buffer_capacity_;  // capacity_ + 1

    mutable std::mutex mutex_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
    std::atomic<bool> closed_;
};

// 实现部分

template <typename T>
SPSCTimeoutQueue<T>::SPSCTimeoutQueue(std::size_t capacity)
    : capacity_(capacity),
      buffer_capacity_(capacity + 1),
      buffer_(buffer_capacity_),
      head_(0),
      tail_(0),
      closed_(false) {}

// 阻塞式推送
template <typename T>
bool SPSCTimeoutQueue<T>::push(const T& item) {
    return push_internal(item, true, nullptr);
}

template <typename T>
bool SPSCTimeoutQueue<T>::push(T&& item) {
    return push_internal(std::move(item), true, nullptr);
}

// 超时推送
template <typename T>
template <typename Rep, typename Period>
bool SPSCTimeoutQueue<T>::push_for(const T& item, const std::chrono::duration<Rep, Period>& timeout_duration) {
    auto timeout_ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeout_duration);
    return push_internal(item, true, &timeout_ms);
}

template <typename T>
template <typename Rep, typename Period>
bool SPSCTimeoutQueue<T>::push_for(T&& item, const std::chrono::duration<Rep, Period>& timeout_duration) {
    auto timeout_ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeout_duration);
    return push_internal(std::move(item), true, &timeout_ms);
}

// 非阻塞推送
template <typename T>
bool SPSCTimeoutQueue<T>::try_push(const T& item) {
    return push_internal(item, false, nullptr);
}

template <typename T>
bool SPSCTimeoutQueue<T>::try_push(T&& item) {
    return push_internal(std::move(item), false, nullptr);
}

// 内部推送实现
template <typename T>
template <typename U>
bool SPSCTimeoutQueue<T>::push_internal(U&& item, bool wait, const std::chrono::milliseconds* timeout) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (!wait) {
        // 非阻塞模式：立即检查是否有空间
        std::size_t tail = tail_.load(std::memory_order_relaxed);
        std::size_t head = head_.load(std::memory_order_acquire);
        if ((tail + 1) % buffer_capacity_ == head || closed_.load(std::memory_order_acquire)) {
            return false;
        }
    } else if (timeout) {
        // 超时模式：等待指定时长
        bool success = not_full_.wait_for(lock, *timeout, [this] {
            std::size_t tail = tail_.load(std::memory_order_relaxed);
            std::size_t head = head_.load(std::memory_order_acquire);
            return (tail + 1) % buffer_capacity_ != head || closed_.load(std::memory_order_acquire);
        });

        if (!success || closed_.load(std::memory_order_acquire)) {
            return false;
        }
    } else {
        // 阻塞模式：无限等待
        not_full_.wait(lock, [this] {
            std::size_t tail = tail_.load(std::memory_order_relaxed);
            std::size_t head = head_.load(std::memory_order_acquire);
            return (tail + 1) % buffer_capacity_ != head || closed_.load(std::memory_order_acquire);
        });

        if (closed_.load(std::memory_order_acquire)) {
            return false;
        }
    }

    // 执行推送
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    buffer_[tail] = std::forward<U>(item);
    tail_.store((tail + 1) % buffer_capacity_, std::memory_order_release);

    // 通知等待的消费者
    not_empty_.notify_one();
    return true;
}

// 阻塞式弹出
template <typename T>
bool SPSCTimeoutQueue<T>::pop(T& item) {
    return pop_internal(item, true, nullptr);
}

// 超时弹出
template <typename T>
template <typename Rep, typename Period>
bool SPSCTimeoutQueue<T>::pop_for(T& item, const std::chrono::duration<Rep, Period>& timeout_duration) {
    auto timeout_ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeout_duration);
    return pop_internal(item, true, &timeout_ms);
}

// 非阻塞弹出
template <typename T>
bool SPSCTimeoutQueue<T>::try_pop(T& item) {
    return pop_internal(item, false, nullptr);
}

// 内部弹出实现
template <typename T>
bool SPSCTimeoutQueue<T>::pop_internal(T& item, bool wait, const std::chrono::milliseconds* timeout) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (!wait) {
        // 非阻塞模式：立即检查是否有数据
        std::size_t head = head_.load(std::memory_order_relaxed);
        std::size_t tail = tail_.load(std::memory_order_acquire);
        if (head == tail) {
            return false;
        }
    } else if (timeout) {
        // 超时模式：等待指定时长
        bool success = not_empty_.wait_for(lock, *timeout, [this] {
            std::size_t head = head_.load(std::memory_order_relaxed);
            std::size_t tail = tail_.load(std::memory_order_acquire);
            return head != tail || closed_.load(std::memory_order_acquire);
        });

        // 检查是否超时或关闭
        std::size_t head = head_.load(std::memory_order_relaxed);
        std::size_t tail = tail_.load(std::memory_order_acquire);
        if (head == tail) {
            return false;  // 队列为空（超时或关闭）
        }
    } else {
        // 阻塞模式：无限等待
        not_empty_.wait(lock, [this] {
            std::size_t head = head_.load(std::memory_order_relaxed);
            std::size_t tail = tail_.load(std::memory_order_acquire);
            return head != tail || closed_.load(std::memory_order_acquire);
        });

        // 检查是否因为关闭而退出
        std::size_t head = head_.load(std::memory_order_relaxed);
        std::size_t tail = tail_.load(std::memory_order_acquire);
        if (head == tail) {
            return false;  // 队列为空且已关闭
        }
    }

    // 执行弹出
    std::size_t head = head_.load(std::memory_order_relaxed);
    item = std::move(buffer_[head]);
    head_.store((head + 1) % buffer_capacity_, std::memory_order_release);

    // 通知等待的生产者
    not_full_.notify_one();
    return true;
}

// 查询方法
template <typename T>
bool SPSCTimeoutQueue<T>::isFull() const {
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    std::size_t head = head_.load(std::memory_order_acquire);
    return (tail + 1) % buffer_capacity_ == head;
}

template <typename T>
bool SPSCTimeoutQueue<T>::isEmpty() const {
    std::size_t head = head_.load(std::memory_order_relaxed);
    std::size_t tail = tail_.load(std::memory_order_acquire);
    return head == tail;
}

template <typename T>
std::size_t SPSCTimeoutQueue<T>::size() const {
    std::size_t head = head_.load(std::memory_order_relaxed);
    std::size_t tail = tail_.load(std::memory_order_acquire);
    return (tail + buffer_capacity_ - head) % buffer_capacity_;
}

template <typename T>
std::size_t SPSCTimeoutQueue<T>::capacity() const {
    return capacity_;
}

template <typename T>
bool SPSCTimeoutQueue<T>::isClosed() const {
    return closed_.load(std::memory_order_acquire);
}

// 关闭队列
template <typename T>
void SPSCTimeoutQueue<T>::close() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_.store(true, std::memory_order_release);
    }
    // 唤醒所有等待的线程
    not_full_.notify_all();
    not_empty_.notify_all();
}

}  // namespace destiny