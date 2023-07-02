template<bool>
struct StaticAssert;

template<>
struct StaticAssert<true> {
};

//template<bool N>
template<int N>
struct DivideByZero {
    static const int value = 1 / N;
//    static const bool value = N;
};

#define STATIC_ASSERT(expr, msg) \
    typedef StaticAssert<sizeof(DivideByZero<expr>::value) != 0> StaticAssert_##__LINE__


#include <iostream>

template<typename T>
void processValue(T value) {
    STATIC_ASSERT(std::is_integral<T>::value, "T must be an integral type");

    // 处理值的逻辑
    std::cout << "Processing value: " << value << std::endl;
}

int main() {
    int intValue = 10;
    float floatValue = 3.14;

    processValue(intValue);   // 正常调用
    processValue(floatValue); // 静态断言失败，编译时会产生除以零错误

    return 0;
}
