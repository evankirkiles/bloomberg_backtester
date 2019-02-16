//
// Created by Evan Kirkiles on 9/25/2018.
//

#include <iostream>

#include "strategy/custom/src/momentum1.hpp"
#include "strategy/custom/src/basic_algo.hpp"

// Main function
int main(int argc, char* argv[]) {

    // Run a Basic Algo
    ALGO_Momentum1 alg(BloombergLP::blpapi::Datetime(2018, 1, 31, 0, 0, 0), date_funcs::get_now(), 1000000);
//    alg.message("Beginning live paper trading of momentum algorithm...");
    // Run the algorithm
    alg.run();

    return 0;
}