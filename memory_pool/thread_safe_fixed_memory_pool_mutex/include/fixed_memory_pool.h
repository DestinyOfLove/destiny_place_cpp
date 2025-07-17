#pragma once
#include <cstddef>
#include <mutex>

namespace destiny_mutex {

class FixedSizeMemoryPool {
public:
    FixedSizeMemoryPool(std::size_t block_size, std::size_t block_cnt);
    ~FixedSizeMemoryPool();

    void* allocate();
    void deallocate(void* ptr);

private:
    struct FreeBlock {
        FreeBlock* next;
    };

    FreeBlock* free_list_head_;
    void* memory_start_ = nullptr;

    std::size_t block_size_ = 0;
    std::size_t block_cnt_ = 0;

    std::mutex mutex_;
};

}  // namespace destiny_mutex