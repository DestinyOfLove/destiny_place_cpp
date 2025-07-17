#include "fixed_memory_pool.h"

#include <cstdlib>
#include <new>

namespace destiny_lock_free {

FixedSizeMemoryPool::FixedSizeMemoryPool(std::size_t block_size, std::size_t block_cnt) {
    block_size_ = block_size;
    block_cnt_ = block_cnt;

    memory_start_ = malloc(block_cnt * block_size);

    // 初始化原子指针
    free_list_head_.store(nullptr);

    // 头插法创建自由链表
    FreeBlock* head = nullptr;
    for (std::size_t i = 0; i < block_cnt; ++i) {
        void* block_ptr = static_cast<char*>(memory_start_) + i * block_size;
        static_cast<FreeBlock*>(block_ptr)->next = head;
        head = static_cast<FreeBlock*>(block_ptr);
    }
    free_list_head_.store(head);
}

FixedSizeMemoryPool::~FixedSizeMemoryPool() {
    free(memory_start_);
}

void* FixedSizeMemoryPool::allocate() {
    FreeBlock* head = free_list_head_.load(std::memory_order_relaxed);
    while (head != nullptr && !free_list_head_.compare_exchange_weak(head, head->next, std::memory_order_release, std::memory_order_relaxed));
    return head;
}

void FixedSizeMemoryPool::deallocate(void* ptr) {
    if (ptr == nullptr) {
        return;
    }
    auto free_block = static_cast<FreeBlock*>(ptr);
    FreeBlock* head = free_list_head_.load(std::memory_order_relaxed);
    do {
        free_block->next = head;
    } while (!free_list_head_.compare_exchange_weak(head, free_block, std::memory_order_release, std::memory_order_relaxed));
}

}  // namespace destiny_lock_free
