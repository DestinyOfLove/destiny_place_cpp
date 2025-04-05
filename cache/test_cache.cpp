#include <iostream>

#include "lru_cache.h"

int main() {
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

    return 0;
}
