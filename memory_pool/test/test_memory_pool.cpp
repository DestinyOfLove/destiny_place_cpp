#include <iostream>
#include <cassert>
#include <vector>
#include <thread>
#include <numeric>
#include <type_traits> // Required for std::is_same

// --- Include all pool headers ---
#include "../fixed_memory_pool/include/fixed_memory_pool.h"
#include "../thread_safe_fixed_memory_pool_mutex/include/fixed_memory_pool.h"
#include "../thread_safe_fixed_memory_pool_lock_free/include/fixed_memory_pool.h"
#include "../thread_safe_fixed_memory_pool_thread_local/include/thread_local_memory_pool.h"
#include "../general_memory_pool/include/general_memory_pool.h"

// --- Type Aliases to resolve name clashes ---
using BasicPool = destiny::FixedSizeMemoryPool;
using MutexPool = destiny_mutex::FixedSizeMemoryPool;
using LockFreePool = destiny_lock_free::FixedSizeMemoryPool;
using ThreadLocalPool = destiny::ThreadLocalMemoryPool;

struct TestObject {
    uint64_t a, b, c, d; // 32 bytes
};

const int NUM_THREADS = 4;
const int ALLOCS_PER_THREAD = 10000;

template<typename Pool>
void thread_func(Pool* pool) {
    std::vector<void*> allocs;
    allocs.reserve(ALLOCS_PER_THREAD);
    for (int i = 0; i < ALLOCS_PER_THREAD; ++i) {
        void* p = pool->allocate();
        assert(p != nullptr);
        allocs.push_back(p);
    }
    for (void* p : allocs) {
        pool->deallocate(p);
    }
}

template<typename Pool>
void run_threaded_test(const std::string& name) {
    std::cout << "\n--- Testing " << name << " ---" << std::endl;

    size_t pool_block_count = NUM_THREADS * ALLOCS_PER_THREAD + 1;

    // For the ThreadLocalMemoryPool, we need to account for the blocks cached by each thread.
    if (std::is_same<Pool, ThreadLocalPool>::value) {
        const size_t BLOCKS_TO_FETCH = 32; // This must match the private constant in the implementation
        pool_block_count += NUM_THREADS * (BLOCKS_TO_FETCH - 1);
    }

    Pool pool(sizeof(TestObject), pool_block_count);
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(thread_func<Pool>, &pool);
    }
    for (auto& t : threads) {
        t.join();
    }
    void* p = pool.allocate();
    assert(p != nullptr);
    pool.deallocate(p);
    std::cout << name << " Test Passed." << std::endl;
}

void test_general_pool() {
    std::cout << "\n--- Testing GeneralMemoryPool ---" << std::endl;
    destiny::GeneralMemoryPool pool;
    
    void* p8 = pool.allocate(8);
    assert(p8 != nullptr);
    void* p512 = pool.allocate(512);
    assert(p512 != nullptr);
    pool.deallocate(p8);
    pool.deallocate(p512);

    void* p1024 = pool.allocate(1024);
    assert(p1024 != nullptr);
    pool.deallocate(p1024);

    std::cout << "GeneralMemoryPool Test Passed." << std::endl;
}

int main() {
    std::cout << "--- Running Correctness Tests ---" << std::endl;

    {
        BasicPool pool(sizeof(TestObject), 5);
        void* p = pool.allocate();
        assert(p != nullptr);
        pool.deallocate(p);
        std::cout << "Basic Pool Sanity Check Passed." << std::endl;
    }

    run_threaded_test<MutexPool>("Mutex Pool");
    run_threaded_test<LockFreePool>("Lock-Free Pool");
    run_threaded_test<ThreadLocalPool>("Thread-Local Pool");
    test_general_pool();

    std::cout << "\nAll correctness tests passed." << std::endl;

    return 0;
}
