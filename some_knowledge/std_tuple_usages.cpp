#include <iostream>
#include <tuple>
#include <string>

// Example 1: Creating and accessing a tuple
void example1() {
    // Create a tuple with three elements of different types
    std::tuple<int, double, std::string> myTuple(10, 3.14, "Hello");

    // Access the elements using std::get and print them
    std::cout << std::get<0>(myTuple) << std::endl;  // Output: 10
    std::cout << std::get<1>(myTuple) << std::endl;  // Output: 3.14
    std::cout << std::get<2>(myTuple) << std::endl;  // Output: Hello
}

// Example 2: Returning multiple values from a function using std::tuple
std::tuple<int, double, std::string> getValues() {
    int intValue = 10;
    double doubleValue = 3.14;
    std::string stringValue = "Hello";

    return std::make_tuple(intValue, doubleValue, stringValue);
}

void example2() {
    // Call the function and store the returned tuple
    std::tuple<int, double, std::string> result = getValues();

    // Access and print the elements of the tuple
    std::cout << std::get<0>(result) << std::endl;  // Output: 10
    std::cout << std::get<1>(result) << std::endl;  // Output: 3.14
    std::cout << std::get<2>(result) << std::endl;  // Output: Hello
}

// Example 3: Using std::tie to unpack a tuple into separate variables
void example3() {
    std::tuple<int, double, std::string> myTuple(10, 3.14, "Hello");

    // Unpack the tuple into separate variables using std::tie
    int intValue;
    double doubleValue;
    std::string stringValue;
    std::tie(intValue, doubleValue, stringValue) = myTuple;

    // Print the unpacked values
    std::cout << intValue << std::endl;         // Output: 10
    std::cout << doubleValue << std::endl;      // Output: 3.14
    std::cout << stringValue << std::endl;      // Output: Hello
}

int main() {
    // Call each example function
    std::cout << "Example 1:" << std::endl;
    example1();
    std::cout << std::endl;

    std::cout << "Example 2:" << std::endl;
    example2();
    std::cout << std::endl;

    std::cout << "Example 3:" << std::endl;
    example3();
    std::cout << std::endl;

    return 0;
}
