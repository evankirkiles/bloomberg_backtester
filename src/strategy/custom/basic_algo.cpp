//
// Created by Evan Kirkiles on 10/11/2018.
//

// Include corresponding header
#include "strategy/custom/src/basic_algo.hpp"

// Initialize the strategy to backtest
BasicAlgo::BasicAlgo(const BloombergLP::blpapi::Datetime &start, const BloombergLP::blpapi::Datetime &end,
                     unsigned long capital) :
     symbol_list({"AAPL US EQUITY",
                  "SPY US EQUITY",
                  "IBM US EQUITY"}),
     Strategy(symbol_list, capital, start, end) {

    // Perform function scheduling here
    schedule_function(&BasicAlgo::check, date_rules, time_rules);
}