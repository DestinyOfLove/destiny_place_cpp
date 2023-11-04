#include <iostream>

// Base class
class Base {
public:
    void publicMethod() {
        std::cout << "This is a public method from Base class." << std::endl;
    }

protected:
    void protectedMethod() {
        std::cout << "This is a protected method from Base class." << std::endl;
    }

private:
    void privateMethod() {
        std::cout << "This is a private method from Base class." << std::endl;
    }
};

// Public inheritance
class PublicDerived : public Base {
public:
    void accessBaseMembers() {
        publicMethod();    // Accessible, as it's a public member of Base
        protectedMethod(); // Accessible, as it's a protected member of Base
        // privateMethod(); // Not accessible, as it's a private member of Base
    }
};

// Protected inheritance
class ProtectedDerived : protected Base {
public:
    void accessBaseMembers() {
        publicMethod();    // Accessible, as it's a public member of Base
        protectedMethod(); // Accessible, as it's a protected member of Base
        // privateMethod(); // Not accessible, as it's a private member of Base
    }
};

// Private inheritance
class PrivateDerived : private Base {
public:
    void accessBaseMembers() {
        publicMethod();    // Accessible, as it's a public member of Base
        protectedMethod(); // Accessible, as it's a protected member of Base
        // privateMethod(); // Not accessible, as it's a private member of Base
    }
};

int main() {
    PublicDerived publicDerived;
    publicDerived.publicMethod();    // Accessible, as it's a public member of PublicDerived
//     publicDerived.protectedMethod(); // Not accessible, as it's a protected member of PublicDerived
    // publicDerived.privateMethod();   // Not accessible, as it's a private member of PublicDerived

    ProtectedDerived protectedDerived;
    // protectedDerived.publicMethod();    // Not accessible, as it's a protected member of ProtectedDerived
    // protectedDerived.protectedMethod(); // Not accessible, as it's a protected member of ProtectedDerived
    // protectedDerived.privateMethod();   // Not accessible, as it's a private member of ProtectedDerived

    PrivateDerived privateDerived;
//     privateDerived.publicMethod();    // Not accessible, as it's a private member of PrivateDerived
    // privateDerived.protectedMethod(); // Not accessible, as it's a private member of PrivateDerived
    // privateDerived.privateMethod();   // Not accessible, as it's a private member of PrivateDerived

    return 0;
}
