#include "general_memory_pool.h"
#include <cstdlib>
#include <new>

namespace destiny {

GeneralMemoryPool::GeneralMemoryPool() {
    size_class_pools_.resize(NUM_SIZE_CLASSES);
    for (std::size_t i = 0; i < NUM_SIZE_CLASSES; ++i) {
        std::size_t block_size = MIN_MANAGED_SIZE << i;
        // The number of blocks in each pool is arbitrary, can be tuned.
        size_class_pools_[i].reset(new ThreadLocalMemoryPool(block_size + sizeof(BlockHeader), 1024));
    }
}

GeneralMemoryPool::~GeneralMemoryPool() = default;

std::size_t GeneralMemoryPool::get_size_class_index(std::size_t size) {
    if (size <= 8) return 0;
    if (size <= 16) return 1;
    if (size <= 32) return 2;
    if (size <= 64) return 3;
    if (size <= 128) return 4;
    if (size <= 256) return 5;
    if (size <= 512) return 6;
    return -1; // Invalid index
}

void* GeneralMemoryPool::allocate(std::size_t size) {
    if (size > MAX_MANAGED_SIZE) {
        // For large objects, fall back to the system allocator.
        // We need to store a null header to indicate this.
        void* mem = malloc(size + sizeof(BlockHeader));
        if (!mem) return nullptr;
        BlockHeader* header = static_cast<BlockHeader*>(mem);
        header->pool = nullptr;
        return static_cast<void*>(header + 1);
    }

    std::size_t index = get_size_class_index(size);
    ThreadLocalMemoryPool* pool = size_class_pools_[index].get();

    void* mem = pool->allocate();
    if (!mem) return nullptr;

    // Place the header before the block
    BlockHeader* header = static_cast<BlockHeader*>(mem);
    header->pool = pool;

    return static_cast<void*>(header + 1);
}

void GeneralMemoryPool::deallocate(void* ptr) {
    if (ptr == nullptr) {
        return;
    }

    // Get the header
    BlockHeader* header = static_cast<BlockHeader*>(ptr) - 1;

    if (header->pool != nullptr) {
        // Belongs to one of our pools
        header->pool->deallocate(header);
    } else {
        // Was allocated by malloc
        free(header);
    }
}

} // namespace destiny
