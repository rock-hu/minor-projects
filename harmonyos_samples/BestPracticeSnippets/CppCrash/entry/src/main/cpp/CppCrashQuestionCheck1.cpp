//
// Created on 2025/5/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

/**
 * 最佳实践：CppCrash问题排查方法
 */

// [Start getStackReference]
#include <iostream>

int &getStackReference() {
    int x = 5;
    return x; // Return the reference of x
}


int main() {
    int &ref = getStackReference(); // Obtain the reference of x
    // x is released after the getStackReference function returns
    // ref is now a dangling reference. Continuing to access it will result in undefined behavior
    std::cout << ref << std::endl; // Attempting to output the value of x is an undefined behavior
    return 0;
}
// [End getStackReference]