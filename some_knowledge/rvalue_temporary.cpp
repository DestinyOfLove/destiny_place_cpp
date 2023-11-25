#include <iostream>

class MyClass {
public:
    MyClass() { std::cout << "MyClass constructor" << std::endl; }
    ~MyClass() { std::cout << "MyClass destructor" << std::endl; }

    void test() const { std::cout << "test called " << std::endl; }
};

int main() {
    MyClass() = MyClass(); // 创建一个临时的 MyClass，将其转换为 xvalue，并赋值给另一个临时对象。
    const MyClass& life_time_extend = MyClass();
    life_time_extend.test();
    return 0;
}
