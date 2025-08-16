#pragma once
#include <cstddef>
#include <vector>

namespace destiny {

template <typename T>
class RingBufferQueue {
public:
    explicit RingBufferQueue(std::size_t capacity);

    // 基本操作
    bool push(const T& item);
    bool pop(T& item);

    // 查询操作
    bool empty() const;
    bool full() const;
    std::size_t size() const;
    std::size_t capacity() const;

    // 调试辅助
    void clear();

private:
    // 判空：head == tail
    // 判满：( tail + 1 ) % buffer_capacity_ == head
    // 需要多一个空间
    // [head, tail)
    std::vector<T> msg_q_;
    std::size_t head_;
    std::size_t tail_;
    std::size_t user_capacity_;    // 用户可用容量
    std::size_t buffer_capacity_;  // 实际缓冲区大小 = user_capacity_ + 1
};

// 构造函数实现
template <typename T>
RingBufferQueue<T>::RingBufferQueue(std::size_t capacity)
    : user_capacity_(capacity), buffer_capacity_(capacity + 1), head_(0), tail_(0) {
    msg_q_.resize(buffer_capacity_);
}

// 核心操作实现
template <typename T>
bool RingBufferQueue<T>::push(const T& item) {
    // 判断是否满了
    // 满了返回 false
    // 不满放进去，返回 true

    if (head_ == (tail_ + 1) % buffer_capacity_) {
        return false;
    }
    msg_q_[tail_] = item;
    tail_ = (tail_ + 1) % buffer_capacity_;
    return true;
}

template <typename T>
bool RingBufferQueue<T>::pop(T& item) {
    if (head_ == tail_) {
        return false;
    }
    item = msg_q_[head_];
    head_ = (head_ + 1) % buffer_capacity_;
    return true;
}

// 查询操作实现
template <typename T>
bool RingBufferQueue<T>::empty() const {
    return head_ == tail_;
}

template <typename T>
bool RingBufferQueue<T>::full() const {
    return (tail_ + 1) % buffer_capacity_ == head_;
}

template <typename T>
std::size_t RingBufferQueue<T>::size() const {
    return (tail_ + buffer_capacity_ - head_) % buffer_capacity_;
}

template <typename T>
std::size_t RingBufferQueue<T>::capacity() const {
    return user_capacity_;
}

template <typename T>
void RingBufferQueue<T>::clear() {
    head_ = tail_ = 0;
    // 注意：不需要清空vector，只需重置指针即可
    // msg_q_的大小保持不变，提高性能
}

}  // namespace destiny
