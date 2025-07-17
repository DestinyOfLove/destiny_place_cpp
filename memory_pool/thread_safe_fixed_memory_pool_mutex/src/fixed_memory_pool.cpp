#include "fixed_memory_pool.h"

#include <cstdlib>
#include <new>

namespace destiny_mutex {

FixedSizeMemoryPool::FixedSizeMemoryPool(std::size_t block_size, std::size_t block_cnt) {
    block_size_ = block_size;
    block_cnt_ = block_cnt;

    memory_start_ = malloc(block_cnt * block_size);

    // 头插法创建自由链表
    free_list_head_ = nullptr;
    for (std::size_t i = 0; i < block_cnt; ++i) {
        void* block_ptr = static_cast<char*>(memory_start_) + i * block_size;
        static_cast<FreeBlock*>(block_ptr)->next = free_list_head_;
        free_list_head_ = static_cast<FreeBlock*>(block_ptr);
    }
}

FixedSizeMemoryPool::~FixedSizeMemoryPool() {
    free(memory_start_);
}

void* FixedSizeMemoryPool::allocate() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (free_list_head_ == nullptr) {
        return nullptr;
    }

    FreeBlock* free_block = free_list_head_;
    free_list_head_ = free_list_head_->next;
    return free_block;
}

void FixedSizeMemoryPool::deallocate(void* ptr) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (ptr == nullptr) {
        return;
    }
    auto free_block = static_cast<FreeBlock*>(ptr);
    free_block->next = free_list_head_;
    free_list_head_ = free_block;
}

}  // namespace destiny_mutex
