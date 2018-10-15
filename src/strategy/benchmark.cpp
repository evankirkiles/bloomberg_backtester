//
// Created by Evan Kirkiles on 10/14/18.
//

// Include corresponding header
#include "benchmark.hpp"

// Initialize the benchmark strategy which simply buys 100% into SPY
Benchmark::Benchmark(const BloombergLP::blpapi::Datetime &start, const BloombergLP::blpapi::Datetime &end,
                     unsigned int capital) :
        Strategy({"SPY US EQUITY"}, capital, start, end) {

    // Order the S&P
    order_target_percent("SPY US EQUITY", 1);
}