#pragma once
#include <atomic>
#include <vector>

namespace destiny {

/**
 * @brief SPSC (Single Producer Single Consumer) 非阻塞队列
 *
 * 设计特点：
 * - 线程模型：一个生产者线程，一个消费者线程
 * - 容量：有界，基于环形缓冲区
 * - 阻塞模式：非阻塞，操作立即返回成功/失败
 * - 同步机制：原子操作，无锁实现
 * - 性能：极高，适用于高频交易、实时系统
 *
 * 内存排序约定：
 * - relaxed: 读取自己维护的指针
 * - acquire: 读取对方线程的指针
 * - release: 写入给对方线程的指针
 */
template <typename T>
class SPSCQueue {
public:
    /**
     * @brief 构造函数
     * @param capacity 用户可用容量
     */
    explicit SPSCQueue(std::size_t capacity);

    /**
     * @brief 生产者接口：非阻塞入队
     * @param item 要入队的元素
     * @return true 成功入队，false 队列已满
     * @note 只能被一个生产者线程调用
     */
    bool push(const T& item);

    /**
     * @brief 消费者接口：非阻塞出队
     * @param item 接收出队元素的引用
     * @return true 成功出队，false 队列为空
     * @note 只能被一个消费者线程调用
     */
    bool pop(T& item);

    /**
     * @brief 查询接口：队列是否为空
     * @return true 队列为空
     * @note 线程安全，但结果可能瞬间失效
     */
    bool empty() const;

    /**
     * @brief 查询接口：队列是否已满
     * @return true 队列已满
     * @note 线程安全，但结果可能瞬间失效
     */
    bool full() const;

    /**
     * @brief 查询接口：当前队列大小
     * @return 当前队列中的元素数量
     * @note 线程安全，但结果可能瞬间失效
     */
    std::size_t size() const;

    /**
     * @brief 查询接口：队列容量
     * @return 队列的最大容量
     */
    std::size_t capacity() const;

    /**
     * @brief 查询接口：队列是否已关闭
     * @return true 队列已关闭
     * @note 线程安全
     */
    bool is_closed() const;

    /**
     * @brief 关闭队列
     * @note 关闭后拒绝新的push操作，但允许读取剩余数据
     * @note 线程安全，可以被任何线程调用
     */
    void close();

private:
    std::vector<T> msg_q_;
    std::atomic<std::size_t> head_{};
    std::atomic<std::size_t> tail_{};
    std::size_t user_capacity_;
    std::size_t buffer_capacity_;
    std::atomic<bool> is_closed_{false};
};

template <typename T>
SPSCQueue<T>::SPSCQueue(std::size_t capacity) : user_capacity_(capacity), buffer_capacity_(capacity + 1) {
    msg_q_.resize(buffer_capacity_);
    // head_ 和 tail_ 已通过{}初始化为0，无需额外操作
}

template <typename T>
bool SPSCQueue<T>::push(const T& item) {
    // 检查队列是否已关闭
    if (is_closed_.load(std::memory_order_acquire)) {
        return false;
    }

    std::size_t tail = tail_.load(std::memory_order_relaxed);
    std::size_t head = head_.load(std::memory_order_acquire);
    std::size_t next_tail = (tail + 1) % buffer_capacity_;
    if (head == next_tail) {
        return false;
    }

    msg_q_[tail] = item;
    tail_.store(next_tail, std::memory_order_release);
    return true;
}

template <typename T>
bool SPSCQueue<T>::pop(T& item) {
    std::size_t head = head_.load(std::memory_order_relaxed);
    std::size_t tail = tail_.load(std::memory_order_acquire);

    if (head == tail) {
        return false;
    }

    std::size_t next_head = (head + 1) % buffer_capacity_;
    item = msg_q_[head];
    head_.store(next_head, std::memory_order_release);
    return true;
}

template <typename T>
bool SPSCQueue<T>::empty() const {
    std::size_t head = head_.load(std::memory_order_relaxed);
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    return head == tail;
}

template <typename T>
bool SPSCQueue<T>::full() const {
    std::size_t head = head_.load(std::memory_order_relaxed);
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    return (tail + 1) % buffer_capacity_ == head;
}

template <typename T>
std::size_t SPSCQueue<T>::size() const {
    std::size_t head = head_.load(std::memory_order_relaxed);
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    return (tail + buffer_capacity_ - head) % buffer_capacity_;
}

template <typename T>
std::size_t SPSCQueue<T>::capacity() const {
    return user_capacity_;
}

template <typename T>
bool SPSCQueue<T>::is_closed() const {
    return is_closed_.load(std::memory_order_acquire);
}

template <typename T>
void SPSCQueue<T>::close() {
    is_closed_.store(true, std::memory_order_release);
}

}  // namespace destiny
