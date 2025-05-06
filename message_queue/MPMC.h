#pragma once
#include <mutex>
#include <queue>

namespace destiny {

// 消息队列，多个生产者-多个消费者
// 锁的方式实现
template <typename T>
class MQMPMC {
public:
    explicit MQMPMC(std::size_t capacity) : capacity_(capacity) {}
    ~MQMPMC() = default;
    bool enqueue(const T& item);
    bool dequeue(T& item);

    const std::size_t& size() const { return q_.size(); }

    const std::size_t& capacity() const { return capacity_; }

private:
    std::queue<T> q_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::size_t capacity_;
};

template <typename T>
bool MQMPMC<T>::enqueue(const T& item) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (q_.size() >= capacity_) {
        return false;
    }
    q_.push(item);
    cv_.notify_one();
    return true;
}

template <typename T>
bool MQMPMC<T>::dequeue(T& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !q_.empty(); });
    if (q_.empty()) {
        return false;
    }
    item = q_.front();
    q_.pop();
    return true;
}

}  // namespace destiny