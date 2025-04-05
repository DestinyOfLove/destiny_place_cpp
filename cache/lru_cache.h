
#pragma once
#include <map>

#include "cache_policy.h"

namespace tiny_cache {

template <typename Key, typename Value, typename Hash = std::hash<Key>>
class LRUCache : public CachePolicy<Key, Value, Hash> {
public:
    explicit LRUCache(std::size_t capacity) : capacity_(capacity) {
        if (capacity_ == 0) {
            throw std::invalid_argument("capacity can't be 0");
        }
    }

    ~LRUCache() override = default;

    [[nodiscard]] bool get(const Key& key, Value& val) override;

    void put(const Key& key, const Value& value) override;

    void clear() override;

    std::size_t size() const override;

    [[nodiscard]] std::size_t capacity() const override;

private:
    using ListType = std::list<std::pair<Key, Value>>;
    using MapType = std::unordered_map<Key, typename ListType::iterator, Hash>;

    ListType list_;
    MapType map_;

    std::size_t capacity_;
};

template <typename Key, typename Value, typename Hash>
bool LRUCache<Key, Value, Hash>::get(const Key& key, Value& val) {
    auto it = map_.find(key);
    if (it == map_.end()) {
        return false;
    }
    val = it->second->second;
    list_.splice(list_.begin(), list_, it->second);
    return true;
}

template <typename Key, typename Value, typename Hash>
void LRUCache<Key, Value, Hash>::put(const Key& key, const Value& value) {
    auto it = map_.find(key);
    if (it == map_.end()) {
        if (map_.size() == capacity_) {
            map_.erase(list_.back().first);
            list_.pop_back();
        }
        std::pair<Key, Value> new_node = std::make_pair(key, value);
        list_.push_front(new_node);
        map_[key] = list_.begin();
        return;
    }
    auto list_iter = it->second;
    list_iter->second = value;
    list_.splice(list_.begin(), list_, list_iter);
}

template <typename Key, typename Value, typename Hash>
void LRUCache<Key, Value, Hash>::clear() {
    list_.clear();
    map_.clear();
}

template <typename Key, typename Value, typename Hash>
std::size_t LRUCache<Key, Value, Hash>::size() const {
    return map_.size();
}

template <typename Key, typename Value, typename Hash>
std::size_t LRUCache<Key, Value, Hash>::capacity() const {
    return capacity_;
}
}  // namespace tiny_cache
