#pragma once
#include <atomic>

namespace destiny {

template <typename T>
struct MQNode {
    T* data;
    std::atomic<MQNode*> next;

    MQNode() : data(nullptr), next(nullptr) {}

    MQNode(T* data) : data(data), next(nullptr) {}
};

template <typename T>
class LockFreeMQ {
public:
    LockFreeMQ();
    ~LockFreeMQ();

    bool push(const T& data);
    T* pop();
    void close();
    bool is_close() const;

private:
    std::atomic<MQNode<T>*> head_;
    std::atomic<MQNode<T>*> tail_;
    std::atomic<bool> is_closed_;
};

template <typename T>
LockFreeMQ<T>::LockFreeMQ() {
    MQNode<T>* dummy = new MQNode<T>();
    head_.store(dummy, std::memory_order_relaxed);
    tail_.store(dummy, std::memory_order_relaxed);
    is_closed_.store(false, std::memory_order_relaxed);
}

template <typename T>
LockFreeMQ<T>::~LockFreeMQ() {
    while (T* tmp = pop()) {
        delete tmp;
        tmp = nullptr;
    }
    delete head_.load(std::memory_order_relaxed);
}

template <typename T>
bool LockFreeMQ<T>::push(const T& data) {
    if (is_closed_.load(std::memory_order_acquire)) {
        return false;
    }
    T* new_data = new T(data);
    MQNode<T>* new_node = new MQNode<T>(new_data);

    while (true) {
        MQNode<T>* old_tail = tail_.load(std::memory_order_acquire);
        MQNode<T>* old_tail_next = old_tail->next.load(std::memory_order_acquire);
        if (tail_.load(std::memory_order_relaxed) == old_tail) {
            if (old_tail_next == nullptr) {
                if (old_tail->next.compare_exchange_weak(
                        old_tail_next, new_node, std::memory_order_release, std::memory_order_relaxed)) {
                    tail_.compare_exchange_strong(
                        old_tail, new_node, std::memory_order_release, std::memory_order_relaxed);
                    return true;
                }
                // 失败了，说明 next已经被修改，此时不需要做操作（特地更新 tail），直接到下一个循环即可
                // 或者执行这个操作: tail_.compare_exchange_weak(old_tail,
                // old_tail->next.load(std::memory_order_acquire), std::memory_order_release,std::memory_order_relaxed);
            } else {
                tail_.compare_exchange_weak(
                    old_tail, old_tail_next, std::memory_order_release, std::memory_order_relaxed);
            }
        }
    }
}

template <typename T>
T* LockFreeMQ<T>::pop() {
    while (true) {
        MQNode<T>* old_head = head_.load(std::memory_order_acquire);
        MQNode<T>* old_head_next = old_head->next.load(std::memory_order_acquire);
        MQNode<T>* tail = tail_.load(std::memory_order_acquire);

        // 尝试插入，更新 head为新的 dummy head
        // 判断队列是否为空，为空返回 nullptr
        // 不为空，则更新 head
        if (head_.load(std::memory_order_relaxed) == old_head) {
            if (old_head == tail) {
                // 存在两种可能，1. 为空；2. 只有一个元素，但是 next没来得及更新
                if (old_head_next == nullptr) {
                    if (is_closed_.load(std::memory_order_acquire)) {
                        return nullptr;
                    }
                    continue;
                }
                // 辅助推进，加速
                tail_.compare_exchange_weak(tail, old_head_next, std::memory_order_release, std::memory_order_relaxed);
            } else {
                // 不为空。尝试更新 head
                if (head_.compare_exchange_weak(
                        old_head, old_head_next, std::memory_order_release, std::memory_order_relaxed)) {
                    // 成功了，需要返回结果，并将 next处理为 dummy
                    T* res = old_head_next->data;
                    old_head_next->data = nullptr;
                    delete old_head;
                    return res;
                }
            }
        }
    }
}

template <typename T>
void LockFreeMQ<T>::close() {
    is_closed_.store(true, std::memory_order_release);
}

template <typename T>
bool LockFreeMQ<T>::is_close() const {
    return is_closed_.load(std::memory_order_acquire);
}

}  // namespace destiny