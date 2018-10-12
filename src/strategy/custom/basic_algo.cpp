//
// Created by Evan Kirkiles on 10/11/2018.
//

// Include corresponding header
#include "strategy/custom/src/basic_algo.hpp"

// Initialize the strategy to backtest
BasicAlgo::BasicAlgo(const BloombergLP::blpapi::Datetime &start, const BloombergLP::blpapi::Datetime &end,
                     unsigned int capital) :
     Strategy({"AAPL US EQUITY", "SPY US EQUITY", "IBM US EQUITY"}, capital, start, end) {

    // Perform function scheduling here.
    // This lambda is annoying but necessary for downcasting the Strategy to the type of your algorithm when
    // it is known, otherwise cannot have schedule_function declared in Strategy base class.
    schedule_function([](Strategy* x)->void { auto b = dynamic_cast<BasicAlgo*>(x); if (b) b->check(); },
            date_rules.week_start(0), TimeRules::market_open(1, 1));

    // Order AAPL, SPY, and IBM
    order_target_percent("AAPL US EQUITY", 0.2);
    order_target_percent("SPY US EQUITY", 0.2);
    order_target_percent("IBM US EQUITY", 0.2);
}

// The test function for the Basic Algo
void BasicAlgo::check() {
    std::cout << "Date: " << current_time <<
                    ", AAPL: " << portfolio.current_holdings["AAPL US EQUITY"] <<
                    ", SPY: " << portfolio.current_holdings["SPY US EQUITY"] <<
                    ", IBM: " << portfolio.current_holdings["IBM US EQUITY"] << std::endl;
}