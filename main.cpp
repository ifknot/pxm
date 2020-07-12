#define CATCH_CONFIG_RUNNER
#include "catch2.h"

#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

int main(int argc, char* argv[]) {
    
    std::vector<std::thread> workers;
    for (int i = 0; i < 5; i++) {
        workers.push_back(std::thread([i]() {
            std::cout << "thread function " << i << "\n";
        }));
    }

    std::cout << "main thread\n";

    std::for_each(workers.begin(), workers.end(), [](std::thread& t) {
        t.join();
    });

    return Catch::Session().run(argc, argv);

}