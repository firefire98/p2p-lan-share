// Necessary header files for input output functions
#include <iostream>
#include <asio.hpp>
#include "p2p-lan-share/main.h"

// main() function: where the execution of
// C++ program begins
int main() {
    asio::io_context io_context;
    // This statement prints "Hello World"
    std::cout << "ASIO is working!" << std::endl;

    return 0;
}