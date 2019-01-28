//
// Created by Evan Kirkiles on 9/25/2018.
//

#include <iostream>

#include "strategy/custom/src/momentum1.hpp"

// Main function
int main(int argc, char* argv[]) {

    // Run a Basic Algo
    ALGO_Momentum1 alg(BloombergLP::blpapi::Datetime(2014, 1, 31, 0, 0, 0), BloombergLP::blpapi::Datetime(2016, 1, 31, 0, 0, 0), 1000000);
    // Run the algorithm
    alg.run();

    return 0;
}