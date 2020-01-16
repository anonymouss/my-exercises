#include "type_info.h"

#include <iostream>

class Foo {};

int main() {
    int value = 0;
    int *ptr = &value;
    const int &ref = value;

    std::cout << my_type_info<decltype(value)>() << std::endl;
    std::cout << my_type_info<decltype(ptr)>() << std::endl;
    std::cout << my_type_info<decltype(ref)>() << std::endl;

    std::cout << my_type_info<int(*)[]>() << std::endl;
    std::cout << my_type_info<const volatile void * const*&>() << std::endl;
    std::cout << my_type_info<const volatile void *(&)[10]>() << std::endl;
    std::cout << my_type_info<int [1][2][3]>() << std::endl;
    std::cout << my_type_info<char(* (* const)(const int(&)[10]) )[10]>() << std::endl;
    std::cout << my_type_info<int (Foo::* const)(int, Foo&&, int) volatile>() << std::endl;
}