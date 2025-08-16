#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <vector>

namespace destiny {

/**
 * @brief SPSC (Single Producer Single Consumer) 阻塞队列
 *
 * 设计要求：
 * - 线程模型：一个生产者线程，一个消费者线程
 * - 容量限制：有界，基于环形缓冲区实现
 * - 阻塞模式：阻塞式，队列满/空时线程休眠等待
 * - 数据结构：环形缓冲区 (vector<T> + head/tail指针)
 * - 同步机制：原子操作 + 条件变量
 * - 性能特点：CPU效率优先，避免忙等待
 *
 * 核心设计理念：
 * - 基于SPSC非阻塞的核心算法，添加阻塞等待机制
 * - 生产者：当队列满时阻塞等待，直到有空间可用
 * - 消费者：当队列空时阻塞等待，直到有数据可用
 * - 优雅关闭：支持close()功能，唤醒所有等待线程
 *
 * 实现要点：
 * 1. 原子指针：使用atomic<size_t>管理head/tail指针
 * 2. 内存排序：采用与SPSC非阻塞相同的内存排序策略
 * 3. 条件变量：not_empty用于消费者等待，not_full用于生产者等待
 * 4. 互斥锁：仅用于条件变量，不保护数据操作
 * 5. 关闭状态：原子布尔标志，支持优雅关闭
 *
 * 适用场景：
 * - 生产者-消费者模式，CPU效率优先
 * - 任务队列处理，允许线程休眠
 * - 日志异步写入，后台处理
 * - 中等吞吐量要求，延迟不是关键指标的场景
 *
 * 性能特点：
 * - 吞吐量：高（基于高效的环形缓冲区）
 * - 延迟：中等（涉及线程唤醒开销）
 * - CPU使用：低（线程休眠，不占用CPU）
 * - 内存使用：固定（预分配缓冲区）
 */
template <typename T>
class SPSCBlockingQueue {
    // TODO: 实现所有必要的成员和方法
    // 参考SPSC非阻塞队列的设计，添加阻塞等待机制
};

}  // namespace destiny