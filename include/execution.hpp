//
// Created by Evan Kirkiles on 10/8/2018.
//

#ifndef BACKTESTER_EXECUTION_HPP
#define BACKTESTER_EXECUTION_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"
// Custom classes includes
#include "events.hpp"
#include "data.hpp"
#include "portfolio.hpp"

// Class containing the execution handler which performs signal handling and order filling. All simulated
// slippage, commission fees, and other such risks take place here. Signal events come in specifying a percent
// of the portfolio to fill with the given stock, which is then filled to its capacity through the held cash
// and the current holdings of the stock.
class ExecutionHandler {
public:
    // Constructor builds the execution handler with data handler, portfolio, and event list references.
    ExecutionHandler(std::queue<std::unique_ptr<events::Event>>* stack, std::list<std::unique_ptr<events::Event>>* heap,
            DataManager* data_manager, Portfolio* portfolio);

    // Takes in a signal event and converts it into an order based on the portfolio holdings, slippage, and commission
    void process_signal(const events::SignalEvent& event);
};

#endif //BACKTESTER_EXECUTION_HPP
