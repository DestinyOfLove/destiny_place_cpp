#include <iostream>
#include <vector>
#include <algorithm>

class Person {
public:
    int height;
    int k;

    Person(int h, int k) : height(h), k(k) {}

    // 这里提供一个比较函数，用于排序
    static bool compare(const Person &a, const Person &b) {
        if (a.height == b.height) return a.k < b.k;
        return a.height > b.height;
    }
};

std::vector<Person> reconstructQueue(std::vector<Person> &people) {
    std::sort(people.begin(), people.end(), Person::compare);

    std::vector<Person> queue;
    for (const auto &person: people) {
        queue.insert(queue.begin() + person.k, person);
    }

    return queue;
}

int main() {
    std::vector<Person> people = {
            Person(7, 0), Person(4, 4), Person(7, 1),
            Person(5, 0), Person(6, 1), Person(5, 2)
    };
    std::vector<Person> queue = reconstructQueue(people);

    for (const auto &person: queue) {
        std::cout << "{" << person.height << ", " << person.k << "} ";
    }
    return 0;
}
