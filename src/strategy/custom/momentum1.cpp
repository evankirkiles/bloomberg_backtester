//
// Created by Evan Kirkiles on 1/25/2019.
//

// Include corresponding header
#include "strategy/custom/src/momentum1.hpp"

// Initialize the strategy to backtest
ALGO_Momentum1::ALGO_Momentum1(const BloombergLP::blpapi::Datetime &start, const BloombergLP::blpapi::Datetime &end,
                     unsigned int capital) :
        Strategy({"SPY US EQUITY", "EFA US EQUITY", "BND US EQUITY",
                  "VNQ US EQUITY", "GSG US EQUITY", "BIL US EQUITY"},
                 capital, start, end) {

    // Perform constant declarations and definitions here.
    context["lookback"] = 126;                                // The lookback for the moving average
    context["maxleverage"] = 0.9;                             // The maximum leverage allowed
    context["multiple"] = 5.0;                                // 1% of return translates to what weight? e.g. 5%
    context["profittake"] = 1.96;                             // Take profits when breaks out of 95% Bollinger Band

    // Perform function scheduling here.
    // This lambda is annoying but necessary for downcasting the Strategy to the type of your algorithm when
    // it is known, otherwise cannot have schedule_function declared in Strategy base class.
    schedule_function([](Strategy* x)->void { auto b = dynamic_cast<ALGO_Momentum1*>(x); if (b) b->rebalance(); },
                      date_rules.month_start(), TimeRules::market_open());
}

// The test function for the Basic Algo
void ALGO_Momentum1::rebalance() {
    // Pull the past [lookback] of data from Bloomberg
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> prices =
            data->history(symbol_list, {"PX_OPEN"}, (unsigned int)context["lookback"], "daily");

    // First sell all positions
    for (const std::string& symbol : symbol_list)
        order_target_percent(symbol, 0);

    std::cout << "Date: " << current_time <<
              ", EFA: " << portfolio.current_holdings["EFA US EQUITY"] <<
              ", BND: " << portfolio.current_holdings["BND US EQUITY"] <<
              ", VNQ: " << portfolio.current_holdings["VNQ US EQUITY"] <<
              ", SPY: " << portfolio.current_holdings["SPY US EQUITY"] << std::endl;
}