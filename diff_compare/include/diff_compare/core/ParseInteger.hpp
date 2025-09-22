#pragma once

#include <boost/utility/string_view.hpp>
#include <cstddef>

namespace diff_compare {

inline bool parseIntegerStrict(boost::string_view view, long long& result) {
    if (view.empty()) {
        return false;
    }

    std::size_t index = 0;
    bool negative = false;
    if (view[0] == '-' || view[0] == '+') {
        negative = view[0] == '-';
        ++index;
        if (index == view.size()) {
            return false;
        }
    }

    long long value = 0;
    for (; index < view.size(); ++index) {
        const char ch = view[index];
        if (ch < '0' || ch > '9') {
            return false;
        }
        value = value * 10 + (ch - '0');
    }

    result = negative ? -value : value;
    return true;
}

inline bool parseIntegerStrict(const std::string& text, long long& result) {
    return parseIntegerStrict(boost::string_view(text.data(), text.size()), result);
}

}  // namespace diff_compare
