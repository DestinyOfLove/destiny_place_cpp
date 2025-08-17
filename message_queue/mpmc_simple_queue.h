#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace destiny {

/**
 * @brief MPMC (Multiple Producer Multiple Consumer) 简单队列
 *
 * 设计要求：
 * - 线程模型：多个生产者线程，多个消费者线程
 * - 容量限制：有界，基于最大容量限制
 * - 阻塞模式：阻塞式，队列满/空时线程休眠等待
 * - 数据结构：std::queue（底层使用std::deque）
 * - 同步机制：单一互斥锁 + 条件变量
 * - 性能特点：实现简单，适合中等负载场景
 *
 * 核心设计理念：
 * - 使用单一互斥锁保护所有操作，实现简单
 * - std::queue 提供 FIFO 语义，自动管理内存
 * - 生产者：当队列满时阻塞等待，直到有空间可用
 * - 消费者：当队列空时阻塞等待，直到有数据可用
 * - 优雅关闭：支持close()功能，唤醒所有等待线程
 *
 * 实现要点：
 * 1. 单锁设计：一个互斥锁保护queue和所有状态
 * 2. 两个条件变量：not_empty用于消费者，not_full用于生产者
 * 3. 容量控制：通过比较queue.size()与capacity实现
 * 4. 关闭状态：原子布尔标志，支持优雅关闭
 * 5. FIFO保证：std::queue自动提供先进先出语义
 *
 * 适用场景：
 * - 中等吞吐量的生产者-消费者模式
 * - 任务队列，工作线程池
 * - 消息传递系统
 * - 不需要极致性能的多线程通信
 *
 * 性能特点：
 * - 吞吐量：中等（单锁成为瓶颈）
 * - 延迟：中等（涉及锁竞争和线程唤醒）
 * - CPU使用：低（线程休眠，不占用CPU）
 * - 内存使用：动态（std::queue自动管理）
 *
 * 与环形缓冲区版本对比：
 * - 优势：实现简单，代码易懂，自动内存管理
 * - 劣势：动态内存分配，可能的cache miss，单锁瓶颈
 */
template <typename T>
class MPMCSimpleQueue {
public:
    explicit MPMCSimpleQueue(std::size_t capacity);

    // 删除拷贝构造和赋值
    MPMCSimpleQueue(const MPMCSimpleQueue&) = delete;
    MPMCSimpleQueue& operator=(const MPMCSimpleQueue&) = delete;

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
    std::queue<T> queue_;         // 底层队列
    const std::size_t capacity_;  // 最大容量

    mutable std::mutex mutex_;           // 保护所有共享状态
    std::condition_variable not_full_;   // 队列非满条件
    std::condition_variable not_empty_;  // 队列非空条件
    bool closed_;                        // 关闭标志，受 mutex_ 保护
};

// 实现部分

template <typename T>
MPMCSimpleQueue<T>::MPMCSimpleQueue(std::size_t capacity) : capacity_(capacity), closed_(false) {
    if (capacity == 0) {
        throw std::invalid_argument("MPMCSimpleQueue capacity must be greater than 0");
    }
}

template <typename T>
bool MPMCSimpleQueue<T>::push(const T& item) {
    return push_internal(item);
}

template <typename T>
bool MPMCSimpleQueue<T>::push(T&& item) {
    return push_internal(std::move(item));
}

template <typename T>
template <typename U>
bool MPMCSimpleQueue<T>::push_internal(U&& item) {
    std::unique_lock<std::mutex> lock(mutex_);

    // 等待队列有空间，或者队列关闭
    not_full_.wait(lock, [this] { return queue_.size() < capacity_ || closed_; });

    // 检查是否因为关闭而退出等待
    if (closed_) {
        return false;
    }

    // 推送元素
    queue_.push(std::forward<U>(item));

    // 通知一个等待的消费者
    not_empty_.notify_one();
    return true;
}

template <typename T>
bool MPMCSimpleQueue<T>::pop(T& item) {
    std::unique_lock<std::mutex> lock(mutex_);

    // 等待队列有数据，或者队列关闭
    not_empty_.wait(lock, [this] { return !queue_.empty() || closed_; });

    // 如果队列为空，说明是因为关闭而退出
    if (queue_.empty()) {
        return false;
    }

    // 取出元素
    item = std::move(queue_.front());
    queue_.pop();

    // 通知一个等待的生产者
    not_full_.notify_one();
    return true;
}

template <typename T>
bool MPMCSimpleQueue<T>::isFull() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size() >= capacity_;
}

template <typename T>
bool MPMCSimpleQueue<T>::isEmpty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

template <typename T>
std::size_t MPMCSimpleQueue<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

template <typename T>
std::size_t MPMCSimpleQueue<T>::capacity() const {
    return capacity_;
}

template <typename T>
bool MPMCSimpleQueue<T>::isClosed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return closed_;
}

template <typename T>
void MPMCSimpleQueue<T>::close() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
    }
    // 唤醒所有等待的线程
    not_full_.notify_all();
    not_empty_.notify_all();
}

}  // namespace destiny