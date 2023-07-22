#include <iostream>

class Transaction {
public:
    Transaction();

    ~Transaction();

    virtual void logTransaction(const std::string &info) const;
};

Transaction::Transaction() {
    logTransaction("construct"); // Call the virtual function in the constructor
}

Transaction::~Transaction() {
    logTransaction("destruct");
}


void Transaction::logTransaction(const std::string &info) const {
    std::cout << "Logging transaction..." << "\t" << info << std::endl;
}

class ConcreteTransaction : public Transaction {
public:
    void logTransaction(const std::string &info) const override {
        std::cout << "Logging ConcreteTransaction..." << "\t" << info << std::endl;
    }
};

int main() {
    ConcreteTransaction ct; // Create an object of the derived class

    return 0;
}
