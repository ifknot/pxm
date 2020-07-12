#define CATCH_CONFIG_RUNNER
#include "catch2.h"

#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

#include "active_object.h"

int main(int argc, char* argv[]) {

    std::cout << "test program execution model\n";
    
    return Catch::Session().run(argc, argv);

}