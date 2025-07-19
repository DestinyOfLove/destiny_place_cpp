
#pragma once
#include <cassert>
#include <map>
#include <stdexcept>

#include "cache_policy.h"

namespace tiny_cache {

template <typename Key, typename Value, typename Hash = std::hash<Key>>
class LFUCache : public CachePolicy<Key, Value, Hash> {
public:
    explicit LFUCache(int32_t capacity) : min_freq_(0), capacity_(capacity) {
        if (capacity == 0) {
            throw std::invalid_argument("capacity can't be 0");
        }
    }

    ~LFUCache() override;
    bool get(const Key& key, Value& val) override;
    void put(const Key& key, const Value& value) override;
    void clear() override;
    std::size_t size() const override;
    std::size_t capacity() const override;

private:
    std::list<Key>& getListByFreq(int32_t freq);
    void updateMinFreq(int32_t possible_new_freq);

private:
    struct ValueEntry {
        explicit ValueEntry(const Value& val) : value(val), freq(1) {}

        // explicit ValueEntry(Value&& val) : value(std::forward<Value>(val)), freq(1) {}

        Value value;
        int32_t freq;
    };

    using CacheMap = std::unordered_map<Key, ValueEntry, Hash>;

    using ListIter = typename std::list<Key>::iterator;
    using FreqListsMap = std::unordered_map<int32_t, std::list<Key>>;
    using ListIterMap = std::unordered_map<Key, ListIter, Hash>;

    CacheMap cache_data_;

    FreqListsMap freq_lists_;    // same freq in a list
    ListIterMap list_iter_map_;  // iter map

    int32_t min_freq_;

    int32_t capacity_;
};

template <typename Key, typename Value, typename Hash>
LFUCache<Key, Value, Hash>::~LFUCache() = default;

template <typename Key, typename Value, typename Hash>
bool LFUCache<Key, Value, Hash>::get(const Key& key, Value& val) {
    auto it = cache_data_.find(key);
    if (it == cache_data_.end()) {
        return false;
    }

    // 确保 key 存在后才执行以下代码
    auto list_it = list_iter_map_.find(key);
    assert(list_it != list_iter_map_.end());

    int32_t& freq = it->second.freq;  // 使用迭代器访问
    std::list<Key>& old_list = getListByFreq(freq);
    std::list<Key>& new_list = getListByFreq(freq + 1);
    old_list.splice(new_list.begin(), old_list, list_it->second);
    freq += 1;
    val = it->second.value;  // 使用迭代器访问

    updateMinFreq(freq);

    return true;
}

template <typename Key, typename Value, typename Hash>
void LFUCache<Key, Value, Hash>::put(const Key& key, const Value& value) {
    if (size() == capacity_) {
        // remove min freq element
        auto min_freq_list_it = freq_lists_.find(min_freq_);
        if (min_freq_list_it != freq_lists_.end() && !min_freq_list_it->second.empty()) {
            // 能找到，抹除一个。策略暂定为最后一个。有问题，如果只有两个元素，put ele1，然后不停put
            // ele2，会导致ele2不停pop和push
            auto& back_key = min_freq_list_it->second.back();
            list_iter_map_.erase(back_key);
            cache_data_.erase(back_key);
            min_freq_list_it->second.pop_back();
        }
    }
    auto it = cache_data_.find(key);
    if (it == cache_data_.end()) {
        // 没找到，放进去
        // ValueEntry value_entry(value);
        // cache_data_[key] = value_entry;
        cache_data_.emplace(key, ValueEntry(value));  // 使用 emplace

        std::list<Key>& freq_one_list = getListByFreq(1);
        freq_one_list.push_front(key);
        list_iter_map_[key] = freq_one_list.begin();
        updateMinFreq(1);
        return;
    }
    // 找到，更新value
    // 更新频率，放到新的列表
    it->second.value = value;
    auto list_it = list_iter_map_.find(key);
    int32_t& freq = it->second.freq;
    std::list<Key>& old_list = getListByFreq(freq);
    std::list<Key>& new_list = getListByFreq(freq + 1);
    old_list.splice(new_list.begin(), old_list, list_it->second);
    freq += 1;

    updateMinFreq(freq);
}

template <typename Key, typename Value, typename Hash>
void LFUCache<Key, Value, Hash>::clear() {
    cache_data_.clear();
    freq_lists_.clear();
    list_iter_map_.clear();
    min_freq_ = 0;
}

template <typename Key, typename Value, typename Hash>
std::size_t LFUCache<Key, Value, Hash>::size() const {
    return cache_data_.size();
}

template <typename Key, typename Value, typename Hash>
std::size_t LFUCache<Key, Value, Hash>::capacity() const {
    return capacity_;
}

template <typename Key, typename Value, typename Hash>
std::list<Key>& LFUCache<Key, Value, Hash>::getListByFreq(int32_t freq) {
    auto it = freq_lists_.find(freq);
    if (it == freq_lists_.end()) {
        freq_lists_[freq] = std::list<Key>();
    }
    return freq_lists_.at(freq);
}

template <typename Key, typename Value, typename Hash>
void LFUCache<Key, Value, Hash>::updateMinFreq(int32_t possible_new_freq) {
    if (min_freq_ == 0) {
        min_freq_ = possible_new_freq;
        return;
    }
    auto it = freq_lists_.find(min_freq_);
    if (it == freq_lists_.end()) {
        return;
    }
    if (it->second.empty()) {
        min_freq_ = possible_new_freq;
    }
}

}  // namespace tiny_cache
