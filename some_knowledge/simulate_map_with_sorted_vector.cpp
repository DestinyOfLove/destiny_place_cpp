#include <string>

typedef std::pair<std::string, int> Data;  // 在这个例子里，"map" 容纳的类型

class DataCompare {
public:
    // 用于排序的比较函数
    bool operator()(const Data &lhs, const Data &rhs) const {
        return keyLess(lhs.first, rhs.first);
    }

    // 用于查找的比较函数 (形式1)
    bool operator()(const Data &lhs, const Data::first_type &k) const {
        return keyLess(lhs.first, k);
    }

    // 用于查找的比较函数 (形式2)
    bool operator()(const Data::first_type &k, const Data &rhs) const {
        return keyLess(k, rhs.first);
    }

private:
    // 比较函数
    bool keyLess(const Data::first_type &k1, const Data::first_type &k2) const {
        return k1 < k2;
    }
};
