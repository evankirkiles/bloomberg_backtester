//
// Created by Evan Kirkiles on 1/25/2019.
//

#ifndef BACKTESTER_MOMENTUM1_HPP
#define BACKTESTER_MOMENTUM1_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"
// Include custom classes
#include "strategy.hpp"

// LOGIC: A simple strategy which buys 10% in SPY, AAPL, and CAT on the first day and then holds for duration.
class ALGO_Momentum1 : public Strategy {
public:
    // Constructor initializes Strategy parent
    ALGO_Momentum1(const BloombergLP::blpapi::Datetime& start, const BloombergLP::blpapi::Datetime& end, unsigned int capital);

    // Trading logic goes here
    void rebalance();
};

#endif //BACKTESTER_MOMENTUM1_HPP
