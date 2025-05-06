#pragma once
#include <vector>

namespace destiny {
template <typename T>
class RingMPMCQueue {
public:
    explicit RingMPMCQueue(size_t capacity) : buffer(capacity), head(0), tail(0), size(0), closed(false) {}

    bool push(const T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        cv_not_full.wait(lock, [&] { return size < buffer.size() || closed; });
        if (closed) return false;

        buffer[tail] = item;
        tail = (tail + 1) % buffer.size();
        ++size;

        cv_not_empty.notify_one();
        return true;
    }

    bool pop(T& result) {
        std::unique_lock<std::mutex> lock(mtx);
        cv_not_empty.wait(lock, [&] { return size > 0 || closed; });
        if (size == 0) return false;

        result = buffer[head];
        head = (head + 1) % buffer.size();
        --size;

        cv_not_full.notify_one();
        return true;
    }

    void close() {
        std::lock_guard<std::mutex> lock(mtx);
        closed = true;
        cv_not_empty.notify_all();
        cv_not_full.notify_all();
    }

private:
    std::vector<T> buffer;
    size_t head;
    size_t tail;
    size_t size;
    bool closed;
    std::mutex mtx;
    std::condition_variable cv_not_empty;
    std::condition_variable cv_not_full;
};
}  // namespace destiny