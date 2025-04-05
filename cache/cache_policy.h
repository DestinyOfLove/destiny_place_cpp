#pragma once
#include <list>
#include <typeindex>

namespace tiny_cache {
template <typename Key, typename Value, typename Hash = std::hash<Key>>
class CachePolicy {
public:
    virtual ~CachePolicy() = default;

    [[nodiscard]] virtual bool get(const Key& key, Value& val) = 0;

    virtual void put(const Key& key, const Value& value) = 0;

    virtual void clear() = 0;

    virtual std::size_t size() const = 0;

    [[nodiscard]] virtual std::size_t capacity() const = 0;
};
}  // namespace tiny_cache
