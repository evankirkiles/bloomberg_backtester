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
class BasicAlgo : public Strategy {
public:
    // Constructor initializes Strategy parent
    BasicAlgo(const BloombergLP::blpapi::Datetime& start, const BloombergLP::blpapi::Datetime& end, unsigned long capital);

    // Trading logic goes here
    void check();

// A list of the tickers being traded by the basic strategy
private:
    const std::vector<std::string> symbol_list;

};

#endif //BACKTESTER_BASIC_ALGO_HPP
