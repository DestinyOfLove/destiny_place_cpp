#pragma once
#include <cstddef>

namespace destiny {

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

    FreeBlock* free_list_head_;                // free list head
    void* memory_start_ = nullptr;  // 池内存起始地址

    std::size_t block_size_ = 0;  // 块大小
    std::size_t block_cnt_ = 0;   // 块数量
};

}  // namespace destiny