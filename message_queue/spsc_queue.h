#pragma once
#include <atomic>
#include <cstddef>
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

private:
    // TODO: 设计数据成员
    // 关键考虑点：
    // 1. 缓冲区：vector<T> vs alignas对齐
    // 2. 原子指针：atomic<size_t> head_, tail_
    // 3. 容量管理：user_capacity_ vs buffer_capacity_
    // 4. 缓存行对齐：避免false sharing
    // 5. 内存排序：relaxed, acquire, release的使用
};

// TODO: 实现所有方法
// 实现提示：
// 1. 构造函数：初始化缓冲区和原子变量
// 2. push: 检查满 -> 写入 -> 更新tail (release)
// 3. pop: 检查空 -> 读取 -> 更新head (release)
// 4. 查询方法：使用适当的内存排序读取原子变量

}  // namespace destiny