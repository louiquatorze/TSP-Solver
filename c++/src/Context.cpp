
#include "Context.h"
#include "interface.h"

#include <iostream>

Context::Context() {
    std::cout << "[C++] Constructing context" << std::endl;
}

Context::~Context() {
    std::cout << "[C++] Destructing context" << std::endl;
}