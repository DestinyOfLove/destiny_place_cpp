#include <iostream>

// 1. 类内静态成员，类外定义

class A {
public:
    static int a;
};

int A::a = 1;

// 2. 类内静态成员，类内定义
class B {
public:
    // static int a = 1; // 编译错误
};

// 3. 类内静态成员const整形类别，类内初始化
// const static integral data member
class C {
public:
    static const int a = 1;
    static const char b = 'a';
    static const long c = 1;
    static const unsigned int d = 1;
    static const unsigned char e = 'a';
    static const unsigned long f = 1;
    static const unsigned long long g = 1;
    static const unsigned short h = 1;
    // static const unsigned double i = 1.0;        // 编译错误
};

struct Tmp {
    int a = 10;
};

class MyClass {
public:
    static constexpr int myConstExprVar = 20;  // 也可以在类内部初始化
    static constexpr double myConstExprVar2 = 20;
    static constexpr Tmp myConstExprVar3{};
};

int main() {
    std::cout << A::a << std::endl;
    std::cout << C::a << std::endl;
    std::cout << C::b << std::endl;
    std::cout << C::c << std::endl;
    std::cout << C::d << std::endl;
    std::cout << C::e << std::endl;
    std::cout << C::f << std::endl;
    std::cout << C::g << std::endl;
    std::cout << C::h << std::endl;
}