#include <iostream>
void* operator new(size_t size) {
    std::cout << "Custom new for size " << size << std::endl;
    void* storage = malloc(size);
    if (storage == nullptr) {
        throw std::bad_alloc();
    }
    return storage;
}

void operator delete(void* p) {
    std::cout << "Custom delete" << std::endl;
    free(p);
}

class MyClass {
};

int main() {
    std::cout << sizeof (MyClass) << std::endl;
    auto* obj = new MyClass(); // Uses custom operator new
    delete obj; // Uses custom operator delete
}
