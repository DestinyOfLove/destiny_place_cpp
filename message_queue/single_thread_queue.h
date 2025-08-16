#pragma once
#include <queue>

namespace destiny {
template <typename T>

// 单线程 queue，非阻塞模式
class SingleThreadQueue {
public:
    bool push(const T& t) {
        msg_q_.push(t);
        return true;
    }

    bool pop(T& t) {
        if (msg_q_.empty()) {
            return false;
        }
        t = msg_q_.front();
        msg_q_.pop();
        return true;
    }

private:
    std::queue<T> msg_q_;
};
}  // namespace destiny
