#include <iostream>

class MyResource {
public:
    MyResource() {
        std::cout << "Resource acquired." << std::endl;
    }

    ~MyResource() {
        std::cout << "Resource released." << std::endl;
    }
};

int main() {
    try {
        MyResource resource;  // Resource acquired
        throw std::runtime_error("An exception occurred.");
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    return 0;  // Resource released
}
