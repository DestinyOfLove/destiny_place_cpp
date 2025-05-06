#pragma once
#include <vector>
#include <atomic>
#include <cstddef>

namespace destiny {

template <typename T>
class MQSPSC {
public:
    explicit MQSPSC(std::size_t capacity);

    bool enqueue(const T& item);
    bool dequeue(T& item);

    std::size_t capacity() const;

private:
    std::vector<T> buffer_;
    std::size_t capacity_;  // 实际缓冲区大小为用户容量 + 1
    std::atomic<std::size_t> head_;
    std::atomic<std::size_t> tail_;
};

template <typename T>
MQSPSC<T>::MQSPSC(std::size_t capacity)
    : buffer_(capacity + 1),
      capacity_(capacity + 1),
      head_(0),
      tail_(0) {}

template <typename T>
bool MQSPSC<T>::enqueue(const T& item) {
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    std::size_t head = head_.load(std::memory_order_acquire);

    std::size_t next_tail = (tail + 1) % capacity_;
    if (next_tail == head) return false;  // 队列已满

    buffer_[tail] = item;
    tail_.store(next_tail, std::memory_order_release);
    return true;
}

template <typename T>
bool MQSPSC<T>::dequeue(T& item) {
    std::size_t head = head_.load(std::memory_order_relaxed);
    std::size_t tail = tail_.load(std::memory_order_acquire);

    if (head == tail) return false;  // 队列为空

    item = buffer_[head];
    head_.store((head + 1) % capacity_, std::memory_order_release);
    return true;
}

template <typename T>
std::size_t MQSPSC<T>::capacity() const {
    return capacity_ - 1;
}

}  // namespace destiny