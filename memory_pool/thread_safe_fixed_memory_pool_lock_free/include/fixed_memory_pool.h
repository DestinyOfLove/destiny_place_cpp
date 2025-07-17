#pragma once
#include <cstddef>
#include <atomic>

namespace destiny_lock_free {

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

    std::atomic<FreeBlock*> free_list_head_;
    void* memory_start_ = nullptr;

    std::size_t block_size_ = 0;
    std::size_t block_cnt_ = 0;
};

}  // namespace destiny_lock_free