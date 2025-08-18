#pragma once
#include <condition_variable>
#include <mutex>
#include <vector>

namespace destiny {

/**
 * @brief MPMC (Multiple Producer Multiple Consumer) 环形队列
 *
 * 设计要求：
 * - 线程模型：多个生产者线程，多个消费者线程
 * - 容量限制：有界，基于环形缓冲区实现
 * - 阻塞模式：阻塞式，队列满/空时线程休眠等待
 * - 数据结构：环形缓冲区 (vector<T> + head/tail指针)
 * - 同步机制：单一互斥锁 + 条件变量
 * - 性能特点：缓存友好，无动态内存分配
 *
 * 核心设计理念：
 * - 使用环形缓冲区代替std::queue，提升缓存locality
 * - 单一互斥锁保护head和tail指针，实现简单
 * - 预分配内存，避免运行时动态分配
 * - 生产者：当队列满时阻塞等待，直到有空间可用
 * - 消费者：当队列空时阻塞等待，直到有数据可用
 * - 优雅关闭：支持close()功能，唤醒所有等待线程
 *
 * 实现要点：
 * 1. 环形缓冲区：head和tail指针管理，(tail+1)%capacity == head表示满
 * 2. 单锁设计：一个互斥锁保护所有状态变量
 * 3. 条件变量：not_empty用于消费者，not_full用于生产者
 * 4. 内存预分配：构造时分配，运行时无额外分配
 * 5. 关闭状态：布尔标志，受mutex保护
 *
 * 适用场景：
 * - 中等吞吐量的数据处理管道
 * - 内存使用敏感的环境
 * - 需要固定延迟保证的系统
 * - 批处理系统的缓冲区
 *
 * 性能特点：
 * - 吞吐量：中等（单锁限制，但比std::queue版本好）
 * - 延迟：中等（涉及锁竞争和线程唤醒）
 * - CPU使用：低（线程休眠，不占用CPU）
 * - 内存使用：固定（预分配缓冲区）
 * - 缓存友好：连续内存布局，良好的空间局部性
 *
 * 与std::queue版本对比：
 * - 优势：更好的缓存locality，无动态内存分配，固定内存占用
 * - 劣势：容量固定，无法动态扩容
 */
template <typename T>
class MPMCRingQueue {
public:
    explicit MPMCRingQueue(std::size_t capacity);

    // 删除拷贝构造和赋值
    MPMCRingQueue(const MPMCRingQueue&) = delete;
    MPMCRingQueue& operator=(const MPMCRingQueue&) = delete;

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
    std::size_t head_;                   // 读指针
    std::size_t tail_;                   // 写指针
    const std::size_t capacity_;         // 用户可用容量
    const std::size_t buffer_capacity_;  // 实际缓冲区大小 = capacity_ + 1

    mutable std::mutex mutex_;           // 保护所有共享状态
    std::condition_variable not_full_;   // 队列非满条件
    std::condition_variable not_empty_;  // 队列非空条件
    bool closed_;                        // 关闭标志，受 mutex_ 保护
};

// 实现部分

template <typename T>
MPMCRingQueue<T>::MPMCRingQueue(std::size_t capacity)
    : capacity_(capacity),
      buffer_capacity_(capacity + 1),
      buffer_(buffer_capacity_),
      head_(0),
      tail_(0),
      closed_(false) {
    if (capacity == 0) {
        throw std::invalid_argument("MPMCRingQueue capacity must be greater than 0");
    }
}

template <typename T>
bool MPMCRingQueue<T>::push(const T& item) {
    return push_internal(item);
}

template <typename T>
bool MPMCRingQueue<T>::push(T&& item) {
    return push_internal(std::move(item));
}

template <typename T>
template <typename U>
bool MPMCRingQueue<T>::push_internal(U&& item) {
    std::unique_lock<std::mutex> lock(mutex_);

    // 等待队列有空间，或者队列关闭
    not_full_.wait(lock, [this] {
        return (tail_ + 1) % buffer_capacity_ != head_ || closed_;
    });

    // 检查是否因为关闭而退出等待
    if (closed_) {
        return false;
    }

    // 推送元素到tail位置
    buffer_[tail_] = std::forward<U>(item);
    tail_ = (tail_ + 1) % buffer_capacity_;

    // 通知等待的消费者
    not_empty_.notify_one();
    return true;
}

template <typename T>
bool MPMCRingQueue<T>::pop(T& item) {
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
bool MPMCRingQueue<T>::isFull() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return (tail_ + 1) % buffer_capacity_ == head_;
}

template <typename T>
bool MPMCRingQueue<T>::isEmpty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return head_ == tail_;
}

template <typename T>
std::size_t MPMCRingQueue<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return (tail_ + buffer_capacity_ - head_) % buffer_capacity_;
}

template <typename T>
std::size_t MPMCRingQueue<T>::capacity() const {
    return capacity_;
}

template <typename T>
bool MPMCRingQueue<T>::isClosed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return closed_;
}

template <typename T>
void MPMCRingQueue<T>::close() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
    }
    // 唤醒所有等待的线程
    not_full_.notify_all();
    not_empty_.notify_all();
}

}  // namespace destiny