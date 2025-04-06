#include <iostream>

#include "lfu_cache.h"
#include "lru_cache.h"

void test_lru() {
    std::cout << "test lru beg" << std::endl;
    tiny_cache::LRUCache<int, int> ii_cache(1);
    ii_cache.put(1, 1);
    ii_cache.put(2, 2);
    int val = -1;
    const bool get_success = ii_cache.get(1, val);
    if (get_success) {
        std::cout << val << "\t" << "success" << std::endl;
    } else {
        std::cout << val << "\t" << "default" << std::endl;
    }
    std::cout << "test lru end" << std::endl;
}

void test_lfu() {
    std::cout << "test lru beg" << std::endl;
    tiny_cache::LFUCache<int, int> ii_lfu_cache(2);
    ii_lfu_cache.put(1, 1);
    ii_lfu_cache.put(2, 2);
    ii_lfu_cache.put(2, 3);
    int val = -1;
    const bool get_success = ii_lfu_cache.get(1, val);
    if (get_success) {
        std::cout << val << "\t" << "success" << std::endl;
    } else {
        std::cout << val << "\t" << "default" << std::endl;
    }
    std::cout << "test lfu end" << std::endl;
}

int main() {
    test_lru();
    test_lfu();

    return 0;
}
