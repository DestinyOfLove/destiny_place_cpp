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
    // TODO(human): 设计数据成员
    // 考虑以下几个关键问题：
    // 1. 如何存储数据？（提示：vector<T>）
    // 2. 如何跟踪头尾位置？（提示：head_, tail_）
    // 3. 如何区分满和空？（提示：capacity_ vs actual_capacity_）
    // 4. 如何高效计算下一个位置？（提示：模运算 or 位运算）

    // 数据成员声明区域
    // ...
};

// 构造函数实现
template <typename T>
RingBufferQueue<T>::RingBufferQueue(std::size_t capacity) {
    // TODO(human): 实现构造函数
    // 思考：
    // - 如何初始化缓冲区大小？
    // - head和tail的初始值是什么？
    // - 是否需要额外的空间来区分满/空状态？
}

// 核心操作实现
template <typename T>
bool RingBufferQueue<T>::push(const T& item) {
    // TODO(human): 实现push操作
    // 步骤提示：
    // 1. 检查队列是否已满
    // 2. 在tail位置存储数据
    // 3. 更新tail指针（注意环形特性）
    // 4. 返回成功状态
    return false;  // 占位符
}

template <typename T>
bool RingBufferQueue<T>::pop(T& item) {
    // TODO(human): 实现pop操作
    // 步骤提示：
    // 1. 检查队列是否为空
    // 2. 从head位置读取数据
    // 3. 更新head指针（注意环形特性）
    // 4. 返回成功状态
    return false;  // 占位符
}

// 查询操作实现
template <typename T>
bool RingBufferQueue<T>::empty() const {
    // TODO(human): 实现empty检查
    // 思考：什么条件下队列为空？
    return true;  // 占位符
}

template <typename T>
bool RingBufferQueue<T>::full() const {
    // TODO(human): 实现full检查
    // 思考：什么条件下队列为满？
    return false;  // 占位符
}

template <typename T>
std::size_t RingBufferQueue<T>::size() const {
    // TODO(human): 计算当前队列中的元素数量
    // 提示：需要考虑环形特性，可能需要模运算
    return 0;  // 占位符
}

template <typename T>
std::size_t RingBufferQueue<T>::capacity() const {
    // TODO(human): 返回队列的最大容量
    // 注意：这里返回的是用户可用容量，不是内部缓冲区大小
    return 0;  // 占位符
}

template <typename T>
void RingBufferQueue<T>::clear() {
    // TODO(human): 清空队列
    // 思考：需要重置哪些状态？
}

}  // namespace destiny