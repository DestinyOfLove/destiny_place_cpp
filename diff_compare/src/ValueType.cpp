#include "diff_compare/ValueType.hpp"

#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>

namespace diff_compare {

namespace {

struct Registry {
    std::map<std::string, ValueTraits> byPrefix;
    std::map<ValueType, std::string> labels;
};

Registry& registry() {
    static Registry instance = [] {
        Registry reg;
        reg.byPrefix.emplace("Int_", ValueTraits{ValueType::Integer, "Integer"});
        reg.labels.emplace(ValueType::Integer, "Integer");
        reg.byPrefix.emplace("Str_", ValueTraits{ValueType::String, "String"});
        reg.labels.emplace(ValueType::String, "String");
        return reg;
    }();
    return instance;
}

std::mutex& registryMutex() {
    static std::mutex mutex;
    return mutex;
}

}  // namespace

void registerValueType(std::string prefix, ValueTraits traits) {
    std::lock_guard<std::mutex> lock(registryMutex());
    Registry& reg = registry();

    if (reg.byPrefix.find(prefix) != reg.byPrefix.end()) {
        throw std::invalid_argument("ValueType prefix already registered: " + prefix);
    }

    const auto label_it = reg.labels.find(traits.type);
    if (label_it == reg.labels.end()) {
        reg.labels.emplace(traits.type, traits.label);
    } else if (label_it->second != traits.label) {
        throw std::invalid_argument("ValueType already registered with a different label");
    }

    reg.byPrefix.emplace(std::move(prefix), std::move(traits));
}

ValueType valueTypeFromHeader(const std::string& header) {
    std::lock_guard<std::mutex> lock(registryMutex());
    const Registry& reg = registry();

    for (const auto& entry : reg.byPrefix) {
        const std::string& prefix = entry.first;
        if (header.compare(0, prefix.size(), prefix) == 0) {
            return entry.second.type;
        }
    }

    throw std::invalid_argument("Unsupported column header prefix: " + header);
}

std::string toString(ValueType type) {
    std::lock_guard<std::mutex> lock(registryMutex());
    const Registry& reg = registry();

    const auto it = reg.labels.find(type);
    if (it != reg.labels.end()) {
        return it->second;
    }
    throw std::logic_error("Unknown ValueType");
}

}  // namespace diff_compare
