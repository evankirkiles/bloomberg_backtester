//
// Created by Evan Kirkiles on 10/14/18.
//

#ifndef BACKTESTER_BENCHMARK_HPP
#define BACKTESTER_BENCHMARK_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"
// Custom class includes
#include "strategy.hpp"

// Contains the logic for a simple Benchmark strategy, in which 100% of the strategy will be invested
// into SPY upon initialization and left to grow over the period of the backtest.
class Benchmark : public Strategy {
public:
    // Constructor initializes the Strategy parent and buys the SPY shares with the same amount of capital
    Benchmark(const BloombergLP::blpapi::Datetime& start, const BloombergLP::blpapi::Datetime& end, unsigned int capital);
};

#endif //BACKTESTER_BENCHMARK_HPP
