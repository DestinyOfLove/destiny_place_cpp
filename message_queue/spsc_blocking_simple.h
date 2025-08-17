#pragma once
#include <condition_variable>
#include <mutex>
#include <vector>

namespace destiny {

/**
 * @brief SPSC (Single Producer Single Consumer) 简单阻塞队列
 *
 * 设计要求：
 * - 线程模型：一个生产者线程，一个消费者线程
 * - 容量限制：有界，基于环形缓冲区实现
 * - 阻塞模式：阻塞式，队列满/空时线程休眠等待
 * - 数据结构：环形缓冲区 (vector<T> + head/tail指针)
 * - 同步机制：传统互斥锁 + 条件变量（纯锁实现）
 * - 性能特点：实现简单，适合学习和理解
 *
 * 核心设计理念：
 * - 使用传统的互斥锁保护所有操作，最简单直观的实现
 * - 完全依赖mutex + condition_variable，不使用原子操作
 * - 生产者：当队列满时阻塞等待，直到有空间可用
 * - 消费者：当队列空时阻塞等待，直到有数据可用
 * - 优雅关闭：支持close()功能，唤醒所有等待线程
 *
 * 实现要点：
 * 1. 纯锁设计：所有操作都在锁保护下进行，包括指针读写
 * 2. 环形缓冲区：使用传统的head/tail指针管理，非原子
 * 3. 条件变量：not_empty用于消费者等待，not_full用于生产者等待
 * 4. 关闭状态：简单布尔标志，受mutex保护
 * 5. 教学友好：代码结构清晰，易于理解阻塞队列原理
 *
 * 适用场景：
 * - 学习和理解阻塞队列的基本原理
 * - 对性能要求不高的简单生产者-消费者场景
 * - 原型开发和快速验证
 * - 作为优化版本的对比参考
 *
 * 性能特点：
 * - 吞吐量：中低（所有操作都需要获取锁）
 * - 延迟：较高（每次操作都有锁开销）
 * - CPU使用：低（线程休眠，不占用CPU）
 * - 内存使用：固定（预分配缓冲区）
 *
 * 与优化版本对比：
 * - 优势：代码简单易懂，实现直观
 * - 劣势：性能较低，所有操作都有锁开销
 */
template <typename T>
class SPSCBlockingSimpleQueue {
public:
    explicit SPSCBlockingSimpleQueue(std::size_t capacity);

    // 删除拷贝构造和赋值
    SPSCBlockingSimpleQueue(const SPSCBlockingSimpleQueue&) = delete;
    SPSCBlockingSimpleQueue& operator=(const SPSCBlockingSimpleQueue&) = delete;

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
    std::vector<T> buffer_;           // 环形缓冲区
    std::size_t head_;                // 读指针，非原子
    std::size_t tail_;                // 写指针，非原子
    const std::size_t capacity_;      // 用户可用容量
    const std::size_t buffer_capacity_; // 实际缓冲区大小 = capacity_ + 1

    mutable std::mutex mutex_;           // 保护所有共享状态
    std::condition_variable not_full_;   // 队列非满条件
    std::condition_variable not_empty_;  // 队列非空条件
    bool closed_;                        // 关闭标志，受 mutex_ 保护
};

// 实现部分

template <typename T>
SPSCBlockingSimpleQueue<T>::SPSCBlockingSimpleQueue(std::size_t capacity)
    : capacity_(capacity),
      buffer_capacity_(capacity + 1),
      buffer_(buffer_capacity_),
      head_(0),
      tail_(0),
      closed_(false) {
    if (capacity == 0) {
        throw std::invalid_argument("SPSCBlockingSimpleQueue capacity must be greater than 0");
    }
}

template <typename T>
bool SPSCBlockingSimpleQueue<T>::push(const T& item) {
    return push_internal(item);
}

template <typename T>
bool SPSCBlockingSimpleQueue<T>::push(T&& item) {
    return push_internal(std::move(item));
}

template <typename T>
template <typename U>
bool SPSCBlockingSimpleQueue<T>::push_internal(U&& item) {
    std::unique_lock<std::mutex> lock(mutex_);

    // 等待队列有空间，或者队列关闭
    not_full_.wait(lock, [this] {
        return (tail_ + 1) % buffer_capacity_ != head_ || closed_;
    });

    // 检查是否因为关闭而退出等待
    if (closed_) {
        return false;
    }

    // 推送元素
    buffer_[tail_] = std::forward<U>(item);
    tail_ = (tail_ + 1) % buffer_capacity_;

    // 通知等待的消费者
    not_empty_.notify_one();
    return true;
}

template <typename T>
bool SPSCBlockingSimpleQueue<T>::pop(T& item) {
    std::unique_lock<std::mutex> lock(mutex_);

    // 等待队列有数据，或者队列关闭
    not_empty_.wait(lock, [this] {
        return head_ != tail_ || closed_;
    });

    // 如果队列为空，说明是因为关闭而退出
    if (head_ == tail_) {
        return false;
    }

    // 取出元素
    item = std::move(buffer_[head_]);
    head_ = (head_ + 1) % buffer_capacity_;

    // 通知等待的生产者
    not_full_.notify_one();
    return true;
}

template <typename T>
bool SPSCBlockingSimpleQueue<T>::isFull() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return (tail_ + 1) % buffer_capacity_ == head_;
}

template <typename T>
bool SPSCBlockingSimpleQueue<T>::isEmpty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return head_ == tail_;
}

template <typename T>
std::size_t SPSCBlockingSimpleQueue<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return (tail_ + buffer_capacity_ - head_) % buffer_capacity_;
}

template <typename T>
std::size_t SPSCBlockingSimpleQueue<T>::capacity() const {
    return capacity_;
}

template <typename T>
bool SPSCBlockingSimpleQueue<T>::isClosed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return closed_;
}

template <typename T>
void SPSCBlockingSimpleQueue<T>::close() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
    }
    // 唤醒所有等待的线程
    not_full_.notify_all();
    not_empty_.notify_all();
}

}  // namespace destiny