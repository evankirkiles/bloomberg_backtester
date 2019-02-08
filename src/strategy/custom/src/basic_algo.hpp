//
// Created by Evan Kirkiles on 10/11/2018.
//

#ifndef BACKTESTER_BASIC_ALGO_HPP
#define BACKTESTER_BASIC_ALGO_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"
// Include custom classes
#include "strategy.hpp"

// LOGIC: A simple strategy which buys 10% in SPY, AAPL, and CAT on the first day and then holds for duration.
class BasicAlgo : public LiveStrategy {
public:
    // Constructor initializes Strategy parent
    BasicAlgo(const BloombergLP::blpapi::Datetime& start, const BloombergLP::blpapi::Datetime& end, unsigned int capital);

    // Trading logic goes here
    void rebalance();
};

#endif //BACKTESTER_BASIC_ALGO_HPP
