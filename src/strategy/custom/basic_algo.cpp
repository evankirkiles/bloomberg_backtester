//
// Created by Evan Kirkiles on 10/11/2018.
//

// Include corresponding header
#include "strategy/custom/src/basic_algo.hpp"

// Initialize the strategy to backtest
BasicAlgo::BasicAlgo(const BloombergLP::blpapi::Datetime &start, const BloombergLP::blpapi::Datetime &end,
                     unsigned int capital) :
     Strategy({"SPY US EQUITY", "EFA US EQUITY", "BND US EQUITY",
               "VNQ US EQUITY", "GSG US EQUITY", "BIL US EQUITY"},
                       capital, start, end) {

    // Perform function scheduling here.
    // This lambda is annoying but necessary for downcasting the Strategy to the type of your algorithm when
    // it is known, otherwise cannot have schedule_function declared in Strategy base class.
    schedule_function([](Strategy* x)->void { auto b = dynamic_cast<BasicAlgo*>(x); if (b) b->rebalance(); },
            date_rules.month_start(), TimeRules::market_open());

    order_target_percent("SPY US EQUITY", 0.2);
    order_target_percent("EFA US EQUITY", 0.2);
    order_target_percent("BND US EQUITY", 0.2);
    order_target_percent("VNQ US EQUITY", 0.2);
}

// The test function for the Basic Algo
void BasicAlgo::rebalance() {
    std::cout << "Date: " << current_time <<
                    ", EFA: " << portfolio.current_holdings["EFA US EQUITY"] <<
                    ", BND: " << portfolio.current_holdings["BND US EQUITY"] <<
                    ", VNQ: " << portfolio.current_holdings["VNQ US EQUITY"] <<
                    ", SPY: " << portfolio.current_holdings["SPY US EQUITY"] << std::endl;
}