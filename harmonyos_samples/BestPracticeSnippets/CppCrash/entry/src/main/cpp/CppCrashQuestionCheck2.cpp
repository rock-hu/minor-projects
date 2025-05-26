//
// Created on 2025/5/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

/**
 * 最佳实践：CppCrash问题排查方法
 */

// [Start RecursiveClass]
# include <iostream>

class RecursiveClass {
public:
    RecursiveClass() {
        std::cout << "Constructing RecursiveClass" << std::endl;
    }


    ~RecursiveClass() {
        std::cout << "Destructing RecursiveClass" << std::endl;
        // Make recursive calls in the destructor
        RecursiveClass obj;
    }
};


int main() {
    RecursiveClass obj;
    return 0;
}
// [End RecursiveClass]
