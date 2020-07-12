#define CATCH_CONFIG_RUNNER
#include "catch2.h"

#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

#include "active_object.h"

int main(int argc, char* argv[]) {

    std::cout << "test program execution model\n";

    pxm::active_object<15> aob1;
    pxm::active_object<15> aob2;
    aob1.send([]() { std::cout << "a" << std::endl; });
    for (int i{ 0 }; i < 10; ++i) {
        aob1.send([&i]() { std::cout << "b" << i << std::endl; });
        aob2.send([&i]() { std::cout << "B" << i << std::endl; });
    }
    aob1.send([]() { std::cout << "c" << std::endl; });
    aob2.send([]() { std::cout << "A" << std::endl; });
    aob2.send([]() { std::cout << "C" << std::endl; });

    return Catch::Session().run(argc, argv);

}