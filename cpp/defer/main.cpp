#include <iostream>

#include "Defer.hpp"

void func() {
    defer( std::cout << "deferred something 1 in function" << std::endl; );
    defer( std::cout << "deferred something 2 in function" << std::endl; );
    std::cout << "entering function" << std::endl;
    std::cout << "exiting function" << std::endl;
}

int main() {
    defer( std::cout << "deferred something... 1" << std::endl; );
    defer( std::cout << "deferred something... 2" << std::endl; );

    func();

    std::cout << "bye..." << std::endl;
}