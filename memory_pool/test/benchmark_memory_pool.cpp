#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>
#include <string>
#include <functional>
#include <random>
#include <algorithm>
#include <fstream>
#include "nlohmann/json.hpp"

// --- Include all pool headers ---
#include "../fixed_memory_pool/include/fixed_memory_pool.h"
#include "../thread_safe_fixed_memory_pool_mutex/include/fixed_memory_pool.h"
#include "../thread_safe_fixed_memory_pool_lock_free/include/fixed_memory_pool.h"
#include "../thread_safe_fixed_memory_pool_thread_local/include/thread_local_memory_pool.h"
#include "../general_memory_pool/include/general_memory_pool.h"

// --- Type Aliases for Clarity ---
using BasicPool = destiny::FixedSizeMemoryPool;
using MutexPool = destiny_mutex::FixedSizeMemoryPool;
using LockFreePool = destiny_lock_free::FixedSizeMemoryPool;
using ThreadLocalPool = destiny::ThreadLocalMemoryPool;
using GeneralPool = destiny::GeneralMemoryPool;

// --- Benchmark Configuration ---
const int ALLOCS_PER_THREAD = 10000;
const std::vector<int> THREAD_COUNTS = {1, 4, 16};
const std::vector<size_t> SIZES_TO_TEST = {32, 256};
const std::vector<size_t> MIXED_SIZES = {16, 32, 64, 128, 256};

// --- Helper Functions ---
void do_work(int iterations) {
    for (int i = 0; i < iterations; ++i) {
        volatile int x = i;
    }
}

// --- Benchmark Runner --- 
struct BenchmarkResult {
    double throughput_ops_per_sec;
    double p99_latency_ns;
};

template<typename Allocator>
BenchmarkResult run_benchmark(Allocator& allocator, int num_threads, const std::vector<size_t>& sizes, int work_iterations) {
    std::vector<std::thread> threads;
    std::vector<std::vector<double>> latencies(num_threads);

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; ++i) {
        latencies[i].reserve(ALLOCS_PER_THREAD);
        threads.emplace_back([&, i]() {
            std::mt19937 gen(i);
            std::uniform_int_distribution<> size_dist(0, sizes.size() - 1);
            void* allocations[ALLOCS_PER_THREAD];

            for (int j = 0; j < ALLOCS_PER_THREAD / 10; ++j) {
                size_t size = sizes[size_dist(gen)];
                void* p = allocator.allocate(size);
                allocator.deallocate(p, size);
            }

            for (int j = 0; j < ALLOCS_PER_THREAD; ++j) {
                size_t size = sizes[size_dist(gen)];
                auto t1 = std::chrono::high_resolution_clock::now();
                allocations[j] = allocator.allocate(size);
                auto t2 = std::chrono::high_resolution_clock::now();
                latencies[i].push_back(std::chrono::duration<double, std::nano>(t2 - t1).count());
                do_work(work_iterations);
            }

            for (int j = 0; j < ALLOCS_PER_THREAD; ++j) {
                allocator.deallocate(allocations[j], sizes[0]);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;

    std::vector<double> all_latencies;
    for(const auto& v : latencies) {
        all_latencies.insert(all_latencies.end(), v.begin(), v.end());
    }
    std::sort(all_latencies.begin(), all_latencies.end());

    BenchmarkResult result;
    result.throughput_ops_per_sec = (double)(ALLOCS_PER_THREAD * num_threads) / duration.count();
    result.p99_latency_ns = all_latencies.empty() ? 0 : all_latencies[all_latencies.size() * 0.99];

    return result;
}

// --- Allocator Wrappers ---
struct MallocWrapper {
    void* allocate(size_t size) { return malloc(size); }
    void deallocate(void* p, size_t) { free(p); }
};

template<typename PoolType>
struct FixedPoolWrapper {
    PoolType pool;
    FixedPoolWrapper(size_t size, size_t count) : pool(size, count) {}
    void* allocate(size_t) { return pool.allocate(); }
    void deallocate(void* p, size_t) { pool.deallocate(p); }
};

struct GeneralPoolWrapper {
    GeneralPool pool;
    void* allocate(size_t size) { return pool.allocate(size); }
    void deallocate(void* p, size_t) { pool.deallocate(p); }
};

int main() {
    nlohmann::json results_json;
    results_json["metadata"] = {
        {"cpu", "Apple M1 Pro (10 cores)"},
        {"os", "macOS"},
        {"compiler", "Apple Clang"},
        {"allocs_per_thread", ALLOCS_PER_THREAD}
    };
    results_json["results"] = nlohmann::json::array();

    std::cout << "Running benchmarks..." << std::endl;

    for (int threads : THREAD_COUNTS) {
        MallocWrapper malloc_wrapper;
        auto res = run_benchmark(malloc_wrapper, threads, MIXED_SIZES, 100);
        results_json["results"].push_back({
            {"name", "malloc/free"}, {"threads", threads}, {"mix", "Mixed"}, {"contention", "Medium"},
            {"throughput", res.throughput_ops_per_sec}, {"p99_latency", res.p99_latency_ns}
        });
    }

    for (int threads : THREAD_COUNTS) {
        if (threads == 1) {
            FixedPoolWrapper<BasicPool> pool(32, ALLOCS_PER_THREAD * threads);
            auto res = run_benchmark(pool, 1, {32}, 0);
            results_json["results"].push_back({
                {"name", "1. BasicPool"}, {"threads", 1}, {"mix", "32B"}, {"contention", "None"},
                {"throughput", res.throughput_ops_per_sec}, {"p99_latency", res.p99_latency_ns}
            });
        }
        FixedPoolWrapper<MutexPool> mutex_pool(32, ALLOCS_PER_THREAD * threads);
        auto res_mutex = run_benchmark(mutex_pool, threads, {32}, 0);
        results_json["results"].push_back({
            {"name", "2. MutexPool"}, {"threads", threads}, {"mix", "32B"}, {"contention", "High"},
            {"throughput", res_mutex.throughput_ops_per_sec}, {"p99_latency", res_mutex.p99_latency_ns}
        });

        FixedPoolWrapper<LockFreePool> lockfree_pool(32, ALLOCS_PER_THREAD * threads);
        auto res_lockfree = run_benchmark(lockfree_pool, threads, {32}, 0);
        results_json["results"].push_back({
            {"name", "3. LockFreePool"}, {"threads", threads}, {"mix", "32B"}, {"contention", "High"},
            {"throughput", res_lockfree.throughput_ops_per_sec}, {"p99_latency", res_lockfree.p99_latency_ns}
        });

        FixedPoolWrapper<ThreadLocalPool> tl_pool(32, ALLOCS_PER_THREAD * threads);
        auto res_tl = run_benchmark(tl_pool, threads, {32}, 0);
        results_json["results"].push_back({
            {"name", "4. ThreadLocalPool"}, {"threads", threads}, {"mix", "32B"}, {"contention", "High"},
            {"throughput", res_tl.throughput_ops_per_sec}, {"p99_latency", res_tl.p99_latency_ns}
        });
    }

    for (int threads : THREAD_COUNTS) {
        GeneralPoolWrapper general_pool;
        auto res = run_benchmark(general_pool, threads, MIXED_SIZES, 100);
        results_json["results"].push_back({
            {"name", "5. GeneralPool"}, {"threads", threads}, {"mix", "Mixed"}, {"contention", "Medium"},
            {"throughput", res.throughput_ops_per_sec}, {"p99_latency", res.p99_latency_ns}
        });
    }

    const std::string filename = "memory_pool/test/test_result.json";
    std::ofstream output_file(filename);
    output_file << std::setw(4) << results_json << std::endl;
    std::cout << "Benchmark results saved to " << filename << std::endl;

    return 0;
}
