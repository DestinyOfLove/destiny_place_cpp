#pragma once
#include <atomic>
#include <memory>

namespace destiny {

/**
 * @brief MPMC (Multiple Producer Multiple Consumer) 无锁无界队列
 *
 * 设计要求：
 * - 线程模型：多个生产者线程，多个消费者线程
 * - 容量限制：无界，仅受系统内存限制
 * - 阻塞模式：非阻塞，所有操作立即返回
 * - 数据结构：原子指针链表 + dummy节点
 * - 同步机制：CAS操作，无互斥锁
 * - 性能特点：理论高性能，但受内存分配器限制
 *
 * 核心设计理念：
 * - 基于Michael & Scott算法的无锁队列实现
 * - 使用dummy节点简化空队列处理
 * - 所有操作通过CAS实现线程安全
 * - 无阻塞等待，适合低延迟场景
 * - **重要限制**：频繁的动态内存分配成为性能瓶颈
 *
 * 算法核心：
 * 1. 链表结构：head -> [dummy] -> [node1] -> [node2] -> ... <- tail
 * 2. dummy节点：简化空队列判断，head始终指向dummy
 * 3. CAS操作：所有指针更新都使用compare_and_swap
 * 4. ABA问题：通过节点所有权管理避免ABA问题
 * 5. 内存回收：使用智能指针管理节点生命周期
 *
 * 性能特点：
 * - **理论吞吐量**：极高（无锁竞争）
 * - **实际性能瓶颈**：动态内存分配器（new/delete）通常带锁
 * - **内存开销**：每个元素需要额外的节点结构
 * - **延迟**：低（无线程阻塞）
 * - **CPU使用**：中等（CAS可能导致忙等）
 *
 * 实现挑战：
 * 1. ABA问题：指针在CAS期间被修改后又改回原值
 * 2. 内存回收：何时安全释放节点内存
 * 3. 协作推进：确保操作最终完成
 * 4. 内存排序：正确的memory_order选择
 *
 * 适用场景：
 * - 学习无锁算法的参考实现
 * - 需要无锁语义的特殊场景
 * - 可以接受内存分配开销的应用
 * - 低延迟要求但吞吐量不是关键的场景
 *
 * 不适用场景：
 * - 高吞吐量场景（受内存分配器限制）
 * - 内存受限环境（节点开销大）
 * - 对性能预测性要求高的场景
 *
 * 与有界无锁版本对比：
 * - 优势：无容量限制，实现相对简单
 * - 劣势：动态内存分配成为主要性能瓶颈
 * - 建议：生产环境优先考虑有界版本
 */
template <typename T>
class LockFreeMPMCQueue {
public:
    LockFreeMPMCQueue();
    ~LockFreeMPMCQueue();

    // 删除拷贝构造和赋值
    LockFreeMPMCQueue(const LockFreeMPMCQueue&) = delete;
    LockFreeMPMCQueue& operator=(const LockFreeMPMCQueue&) = delete;

    // 非阻塞推送（始终成功，除非内存不足）
    bool push(const T& item);
    bool push(T&& item);

    // 非阻塞弹出（空时返回false）
    bool pop(T& item);

    // 查询方法（注意：在并发环境下结果可能瞬间失效）
    bool empty() const;
    std::size_t size() const;  // 注意：O(n)复杂度，生产环境慎用

private:
    // 内部节点结构
    struct Node {
        std::atomic<T*> data;        // 数据指针，nullptr表示dummy节点
        std::atomic<Node*> next;     // 下一个节点指针

        Node() : data(nullptr), next(nullptr) {}
        explicit Node(T* item) : data(item), next(nullptr) {}
    };

    // 内部辅助方法
    template <typename U>
    bool push_internal(U&& item);

private:
    std::atomic<Node*> head_;        // 头指针，指向dummy节点
    std::atomic<Node*> tail_;        // 尾指针，指向最后一个节点
};

// 实现部分

template <typename T>
LockFreeMPMCQueue<T>::LockFreeMPMCQueue() {
    // 创建dummy节点
    Node* dummy = new Node();
    head_.store(dummy, std::memory_order_relaxed);
    tail_.store(dummy, std::memory_order_relaxed);
}

template <typename T>
LockFreeMPMCQueue<T>::~LockFreeMPMCQueue() {
    // 清空队列并释放所有节点
    T item;
    while (pop(item)) {
        // 继续弹出直到队列为空
    }
    
    // 释放dummy节点
    Node* head = head_.load(std::memory_order_relaxed);
    delete head;
}

template <typename T>
bool LockFreeMPMCQueue<T>::push(const T& item) {
    return push_internal(item);
}

template <typename T>
bool LockFreeMPMCQueue<T>::push(T&& item) {
    return push_internal(std::move(item));
}

template <typename T>
template <typename U>
bool LockFreeMPMCQueue<T>::push_internal(U&& item) {
    // 分配新节点和数据
    T* data = nullptr;
    Node* new_node = nullptr;
    
    try {
        data = new T(std::forward<U>(item));
        new_node = new Node(data);
    } catch (...) {
        delete data;
        delete new_node;
        return false;  // 内存分配失败
    }

    while (true) {
        Node* last = tail_.load(std::memory_order_acquire);
        Node* next = last->next.load(std::memory_order_acquire);

        // 检查tail是否被其他线程修改
        if (last == tail_.load(std::memory_order_acquire)) {
            if (next == nullptr) {
                // tail确实指向最后一个节点，尝试链接新节点
                if (last->next.compare_exchange_weak(
                        next, new_node,
                        std::memory_order_release,
                        std::memory_order_relaxed)) {
                    // 成功链接，现在尝试移动tail
                    tail_.compare_exchange_weak(
                        last, new_node,
                        std::memory_order_release,
                        std::memory_order_relaxed);
                    break;
                }
            } else {
                // tail落后了，帮助推进tail
                tail_.compare_exchange_weak(
                    last, next,
                    std::memory_order_release,
                    std::memory_order_relaxed);
            }
        }
    }
    
    return true;
}

template <typename T>
bool LockFreeMPMCQueue<T>::pop(T& item) {
    while (true) {
        Node* first = head_.load(std::memory_order_acquire);
        Node* last = tail_.load(std::memory_order_acquire);
        Node* next = first->next.load(std::memory_order_acquire);

        // 检查head是否被其他线程修改
        if (first == head_.load(std::memory_order_acquire)) {
            if (first == last) {
                if (next == nullptr) {
                    // 队列为空
                    return false;
                }
                // tail落后了，帮助推进tail
                tail_.compare_exchange_weak(
                    last, next,
                    std::memory_order_release,
                    std::memory_order_relaxed);
            } else {
                // 队列不为空，读取数据
                if (next == nullptr) {
                    // 不一致状态，重试
                    continue;
                }

                T* data = next->data.load(std::memory_order_acquire);
                if (data == nullptr) {
                    // next是新的dummy节点，重试
                    continue;
                }

                // 尝试移动head
                if (head_.compare_exchange_weak(
                        first, next,
                        std::memory_order_release,
                        std::memory_order_relaxed)) {
                    // 成功，复制数据并清理
                    item = *data;
                    delete data;
                    delete first;  // 释放旧的dummy节点
                    return true;
                }
            }
        }
    }
}

template <typename T>
bool LockFreeMPMCQueue<T>::empty() const {
    Node* first = head_.load(std::memory_order_acquire);
    Node* last = tail_.load(std::memory_order_acquire);
    
    return (first == last) && (first->next.load(std::memory_order_acquire) == nullptr);
}

template <typename T>
std::size_t LockFreeMPMCQueue<T>::size() const {
    std::size_t count = 0;
    Node* current = head_.load(std::memory_order_acquire);
    
    // 跳过dummy节点
    current = current->next.load(std::memory_order_acquire);
    
    while (current != nullptr) {
        ++count;
        current = current->next.load(std::memory_order_acquire);
    }
    
    return count;
}

}  // namespace destiny