#include "thread_local_memory_pool.h"

namespace destiny {

// Define the thread-local static variable
thread_local ThreadLocalMemoryPool::FreeBlock* ThreadLocalMemoryPool::local_free_list_ = nullptr;

ThreadLocalMemoryPool::ThreadLocalMemoryPool(std::size_t block_size, std::size_t block_cnt)
    : central_pool_(block_size, block_cnt) {}

ThreadLocalMemoryPool::~ThreadLocalMemoryPool() {
    // Note: This destructor doesn't handle returning memory from thread-local
    // caches back to the central pool. In a real-world scenario, a more
    // complex mechanism would be needed to avoid memory leaks if threads
    // terminate with non-empty caches.
}

void* ThreadLocalMemoryPool::allocate() {
    if (local_free_list_ == nullptr) {
        fetch_from_central_pool();
    }

    if (local_free_list_ == nullptr) { // Still null after fetch? Central pool is empty.
        return nullptr;
    }

    FreeBlock* block = local_free_list_;
    local_free_list_ = local_free_list_->next;
    return block;
}

void ThreadLocalMemoryPool::deallocate(void* ptr) {
    if (ptr == nullptr) {
        return;
    }
    auto free_block = static_cast<FreeBlock*>(ptr);
    free_block->next = local_free_list_;
    local_free_list_ = free_block;
}

void ThreadLocalMemoryPool::fetch_from_central_pool() {
    // Fetch a batch of blocks from the central pool
    for (std::size_t i = 0; i < BLOCKS_TO_FETCH; ++i) {
        void* block_ptr = central_pool_.allocate();
        if (block_ptr == nullptr) {
            break; // Central pool is exhausted
        }
        // Add the new block to the local free list
        auto free_block = static_cast<FreeBlock*>(block_ptr);
        free_block->next = local_free_list_;
        local_free_list_ = free_block;
    }
}

} // namespace destiny
