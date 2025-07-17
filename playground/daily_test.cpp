#include <iostream>

struct TmpObj {
    int a;
    bool b;
};

int main() {
    std::cout << "test" << std::endl;
    using TmpObjStorage = std::aligned_storage<sizeof(TmpObj), alignof(TmpObj)>::type;
    TmpObjStorage tmpObjStorage;
    auto tmpObj = new (&tmpObjStorage) TmpObj();

    std::cout << "size of TmpObj: " << sizeof(TmpObj) << std::endl;
    std::cout << "alignof of TmpObj: " << alignof(TmpObj) << std::endl;
    std::cout << "size of TmpObjStorage: " << sizeof(TmpObjStorage) << std::endl;
}
