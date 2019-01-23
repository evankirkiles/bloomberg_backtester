//
// Created by Evan Kirkiles on 9/25/2018.
//

#include <iostream>

#include "strategy/custom/src/basic_algo.hpp"

// Main function
int main(int argc, char* argv[]) {

    // Run a Basic Algo
    BasicAlgo alg(date_funcs::get_now(), BloombergLP::blpapi::Datetime(2019, 1, 31, 0, 0, 0), 1000000);
    // Run the algorithm
    alg.run();

    return 0;
}