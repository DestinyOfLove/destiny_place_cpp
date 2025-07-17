#pragma once
#include <cstddef>
// Use the lock-free pool with its new namespace
#include "../../thread_safe_fixed_memory_pool_lock_free/include/fixed_memory_pool.h"

namespace destiny {

class ThreadLocalMemoryPool {
public:
    ThreadLocalMemoryPool(std::size_t block_size, std::size_t block_cnt);
    ~ThreadLocalMemoryPool();

    void* allocate();
    void deallocate(void* ptr);

private:
    struct FreeBlock {
        FreeBlock* next;
    };

    // The central pool to fetch chunks of memory from.
    destiny_lock_free::FixedSizeMemoryPool central_pool_;

    // Thread-local free list
    static thread_local FreeBlock* local_free_list_;

    // Number of blocks to fetch from the central pool at a time.
    static const std::size_t BLOCKS_TO_FETCH = 32;

    // Fetches a new batch of blocks from the central pool.
    void fetch_from_central_pool();
};

} // namespace destiny