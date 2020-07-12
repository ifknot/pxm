#include "catch2.h"

#include <iostream>
#include <atomic>

#include "active_object.h"

SCENARIO("active object", "[pxm]") {
    GIVEN("two active objects") {
        pxm::active_object<31> aob1;
        pxm::active_object<31> aob2;
        WHEN("both fight over an atomic variable") {
            std::atomic_char sum{ 0 };
            for (int i{ 0 }; i < 10; ++i) {
                aob1.send([i, &sum]() {
                    sum += i;
                    //std::cout << +sum << std::endl; 
                    });
                aob2.send([i, &sum]() {
                    sum -= i;
                    //std::cout << +sum << std::endl; 
                    });
            }
            THEN("they cancel out") {
                while (aob1.busy() || aob2.busy()) {} // needed to cooperate with catch2
                REQUIRE(sum == 0);
            }
        }
           
        std::cout << std::endl;
    }
}