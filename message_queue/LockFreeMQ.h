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

    void push(const T& data);
    T* pop();

private:
    std::atomic<MQNode<T>*> head_;
    std::atomic<MQNode<T>*> tail_;
};

template <typename T>
LockFreeMQ<T>::LockFreeMQ() {
    MQNode<T>* dummy = new MQNode<T>();
    head_.store(dummy);
    tail_.store(dummy);
}

template <typename T>
LockFreeMQ<T>::~LockFreeMQ() {
    while (T* tmp = pop()) {
        delete tmp;
        tmp = nullptr;
    }
    delete head_.load();
}

template <typename T>
void LockFreeMQ<T>::push(const T& data) {
    T* new_data = new T(data);
    MQNode<T>* new_node = new MQNode<T>(new_data);

    while (true) {
        MQNode<T>* old_tail = tail_.load();
        MQNode<T>* old_tail_next = old_tail->next.load();
        if (tail_.load() == old_tail) {
            if (old_tail_next == nullptr) {
                if (old_tail->next.compare_exchange_weak(old_tail_next, new_node)) {
                    tail_.compare_exchange_strong(old_tail, new_node);
                    return;
                }
                // 失败了，说明 next已经被修改，此时不需要做操作（特地更新 tail），直接到下一个循环即可
                // 或者执行这个操作: tail_.compare_exchange_weak(old_tail, old_tail->next.load());
            } else {
                tail_.compare_exchange_weak(old_tail, old_tail_next);
            }
        }
    }
}

template <typename T>
T* LockFreeMQ<T>::pop() {
    while (true) {
        MQNode<T>* old_head = head_.load();
        MQNode<T>* old_head_next = old_head->next.load();
        MQNode<T>* tail = tail_.load();

        // 尝试插入，更新 head为新的 dummy head
        // 判断队列是否为空，为空返回 nullptr
        // 不为空，则更新 head
        if (head_.load() == old_head) {
            if (old_head == tail) {
                // 存在两种可能，1. 为空；2. 只有一个元素，但是 next没来得及更新
                if (old_head_next == nullptr) {
                    return nullptr;
                }
                // 辅助推进，加速
                tail_.compare_exchange_weak(tail, old_head_next);
            } else {
                // 不为空。尝试更新 head
                if (head_.compare_exchange_weak(old_head, old_head_next)) {
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

}  // namespace destiny