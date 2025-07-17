#pragma once
#include <cstddef>
#include <vector>
#include <memory>
#include "../../thread_safe_fixed_memory_pool_thread_local/include/thread_local_memory_pool.h"

namespace destiny {

class GeneralMemoryPool {
public:
    GeneralMemoryPool();
    ~GeneralMemoryPool();

    void* allocate(std::size_t size);
    void deallocate(void* ptr);

private:
    // Header stored before each allocated block to know its origin pool
    struct BlockHeader {
        ThreadLocalMemoryPool* pool;
    };

    // Align size up to the nearest power of 2, or multiple of a base.
    // This simple version uses fixed size classes.
    static std::size_t align_size(std::size_t size);

    // Get the index for the size class
    static std::size_t get_size_class_index(std::size_t size);

    // A vector of fixed-size memory pools for different size classes.
    std::vector<std::unique_ptr<ThreadLocalMemoryPool>> size_class_pools_;

    // Max size managed by the pools. Larger allocations go to malloc.
    static const std::size_t MAX_MANAGED_SIZE = 512;
    static const std::size_t MIN_MANAGED_SIZE = 8;
    static const std::size_t NUM_SIZE_CLASSES = 7; // 8, 16, 32, 64, 128, 256, 512
};

} // namespace destiny
