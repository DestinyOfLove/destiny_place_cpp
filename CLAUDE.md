# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A modular C++ playground repository featuring LeetCode solutions, algorithm implementations, design patterns, memory management systems, and concurrent data structures. Uses CMake for build management with each directory as a self-contained subproject.

## Build System

**Primary Build Commands:**
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)          # Build all targets
make test               # Run tests (CTest)
```

**Running Executables:**
```bash
./build/leetcode/twoSum2                    # Run specific LeetCode solution
./build/cache/test_cache                    # Run cache tests
./build/memory_pool/test_memory_pool        # Run memory pool tests
./build/memory_pool/benchmark_memory_pool   # Run memory pool benchmarks
```

**Development Mode:**
```bash
# Auto-build new files without CMakeLists.txt edits
make                    # After adding new .cpp files
```

## Architecture & Module Structure

### Core Modules

**Memory Pool System** (`memory_pool/`)
- **FixedSizeMemoryPool**: Basic fixed-size block allocation
- **ThreadLocalMemoryPool**: Per-thread caching for lock-free performance
- **LockFreePool**: CAS-based concurrent allocation
- **MutexPool**: Traditional mutex-locked allocation
- **GeneralPool**: Variable-size allocation using size-classed ThreadLocalPools
- **Benchmarking**: Comprehensive performance testing with Python visualization

**Message Queues** (`message_queue/`)
- **LockFreeMQ**: Lock-free MPMC queue using atomic operations
- **MPMC.h**: Multi-producer multi-consumer queue variants
- **SPSC.h**: Single-producer single-consumer queue
- **RingMQMPMC.h**: Ring buffer-based MPMC queue

**Cache Policies** (`cache/`)
- **LRUCache**: Least Recently Used eviction policy
- **LFUCache**: Least Frequently Used eviction policy
- **CachePolicy**: Abstract base class for cache implementations

**Algorithm Implementations** (`algorithm/`)
- **nth_element.cpp**: Quickselect algorithm
- **vector_array/**: Array-based algorithms (longest consecutive sequence, min cover substring)

**Design Principles** (`design_princples/`)
- **SOLID** principle examples (Liskov Substitution, Dependency Inversion)
- Clean code demonstrations with before/after comparisons

**LeetCode Solutions** (`leetcode/`)
- Individual .cpp files for each problem solution
- Auto-compiled to separate executables
- Covers arrays, strings, dynamic programming, graphs, etc.

### Build Configuration

**CMake Standards:**
- **All modules**: C++11 standard (updated from C++17 in memory_pool)
- **Auto-detection**: New .cpp files automatically become executables in algorithm/leetcode/playground

**Library Dependencies:**
- **memory_pool**: nlohmann_json for benchmark data export
- **Concurrent**: std::atomic, std::thread for lock-free implementations

## Code Patterns & Conventions

### Memory Pool Architecture
```cpp
// Example: ThreadLocalPool pattern
using BasicPool = destiny::FixedSizeMemoryPool;
using ThreadLocalPool = destiny::ThreadLocalMemoryPool;
// Performance hierarchy: ThreadLocal > LockFree > Mutex
```

### Cache Interface Pattern
```cpp
template <typename Key, typename Value>
class CachePolicy {
public:
    virtual bool get(const Key& key, Value& val) = 0;
    virtual void put(const Key& key, const Value& value) = 0;
    virtual void clear() = 0;
    virtual std::size_t size() const = 0;
};
```

### Message Queue Patterns
- **Lock-free**: Uses std::atomic with memory ordering constraints
- **Dummy node**: Sentinel node pattern for empty queue handling
- **ABA prevention**: Careful pointer management in lock-free structures

## Testing & Benchmarking

### Memory Pool Testing
- **Correctness**: Multi-threaded allocation/deallocation tests
- **Performance**: Throughput and p99 latency measurements
- **Visualization**: Python scripts generate benchmark charts

### Cache Testing
- **Unit tests**: LRU/LFU correctness validation
- **Performance**: Hit/miss ratio analysis

### LeetCode Testing
- **Individual execution**: Each .cpp is a standalone executable
- **Problem-specific**: Input/output validation per problem

## Development Workflow

### Adding New Code

**LeetCode Problem:**
1. Add `problem_name.cpp` to `leetcode/`
2. `make` from build directory - auto-compiles
3. Run `./build/leetcode/problem_name`

**New Algorithm:**
1. Add `algorithm_name.cpp` to `algorithm/`
2. Same auto-build applies

**New Library:**
1. Create subdirectory with CMakeLists.txt
2. Follow memory_pool pattern for library structure
3. Add subdirectory to root CMakeLists.txt

### Performance Analysis

**Memory Pool Benchmarks:**
```bash
cd memory_pool/test
python3 visualize.py  # Generate benchmark charts
```

**Key Metrics:**
- Throughput vs thread count
- p99 latency under contention
- Comparison against system malloc

## IDE Integration

**CLion/VS Code:**
- Open root directory for CMake integration
- Individual targets appear in build/run configurations
- Debug per-target executables directly

**Build Directory Structure:**
```
build/
├── bin/                    # Executables
├── algorithm/
├── cache/
├── leetcode/
├── memory_pool/
├── message_queue/
└── ...
```