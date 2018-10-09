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
#include "slippage.hpp"

// Class containing the execution handler which performs signal handling and order filling. All simulated
// slippage, commission fees, and other such risks take place here. Signal events come in specifying a percent
// of the portfolio to fill with the given stock, which is then filled to its capacity through the held cash
// and the current holdings of the stock.
class ExecutionHandler {
public:
    // Constructor builds the execution handler with data handler, portfolio, and event list references.
    ExecutionHandler(std::queue<std::unique_ptr<events::Event>>* stack, std::list<std::unique_ptr<events::Event>>* heap,
            std::shared_ptr<DataManager> data_manager, Portfolio* portfolio);

    // Takes in a Signal Event and converts it into an order based on the portfolio holdings, slippage, and commission
    void process_signal(const events::SignalEvent& event);
    // Takes in an Order Event and converts it into a FillEvent based on fill limits (may also split it into several orders)
    void process_order(const events::OrderEvent& event);

    // Calculates the IB commission on an order based on a quantity
    double calculate_commission(int quantity);
    // Calculates the slippage on an order
    double calculate_slippage(double cost);

private:
    // Pointers to the external event list stack and heap
    std::queue<std::unique_ptr<events::Event>>* stack_eventlist;
    std::list<std::unique_ptr<events::Event>>* heap_eventlist;
    // Other references needed for data retrieval and portfolio fitting
    std::shared_ptr<DataManager> data_manager;
    Portfolio* portfolio;
};

#endif //BACKTESTER_EXECUTION_HPP
