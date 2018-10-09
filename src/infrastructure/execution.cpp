#include <utility>

//
// Created by Evan Kirkiles on 10/8/2018.
//

// Include corresponding header
#include "execution.hpp"

// Execution constructor is just an initializer list
ExecutionHandler::ExecutionHandler(std::queue<std::unique_ptr<events::Event>> *p_stack,
                                   std::list<std::unique_ptr<events::Event>> *p_heap,
                                   std::shared_ptr<DataManager> p_data_manager,
                                   Portfolio *p_portfolio) :
       stack_eventlist(p_stack),
       heap_eventlist(p_heap),
       data_manager(std::move(p_data_manager)),
       portfolio(p_portfolio) {}

// Takes a signal event and converts it into an Order Event. It first performs a MarketEvent to make sure the
// portfolio holdings are as up-to-date as possible.
void ExecutionHandler::process_signal(const events::SignalEvent &event) {

    // Before doing anything, recalculate portfolio holdings with a simulated MarketEvent
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> recentprice =
            std::move(data_manager->history({event.symbol}, {"PX_LAST"}, 1, "RECENT"));
    auto data = recentprice->at(event.symbol).data.rbegin();
    portfolio->update_market(events::MarketEvent({event.symbol}, {{event.symbol, data->second["PX_LAST"]}}, data->first));

    // Determine what percentage of the portfolio must be filled based on the totalholdings, heldcash, and current holdings.
    double current_percent = portfolio->current_holdings[event.symbol] / portfolio->current_holdings[portfolio_fields::TOTAL_HOLDINGS];
    double percent_needed = event.percentage - current_percent;
    // Convert the percent to a quantity of the stock, chopping off any decimals
    int quantity = (event.percentage == 0) ?
            portfolio->current_positions[event.symbol] * -1 : static_cast<int>(percent_needed / data->second["PX_LAST"]);

    // Now build the order event and place it onto the STACK to be filled as soon as possible
    stack_eventlist->emplace(std::make_unique<events::OrderEvent>(event.symbol, quantity, event.datetime));
}

// Processes an Order Event to convert it into a fill which will be used to update the portfolio and holdings. All
// slippage and transaction cost simulation will be performed here.
void ExecutionHandler::process_order(const events::OrderEvent &event) {

    // First, get the price of the stock (should be the most recent one as this event is run on the STACK after
    // the stock data has already been updated for the signal order).
    double price = portfolio->current_holdings[event.symbol] / portfolio->current_positions[event.symbol];

    // Make sure the market can handle the order as well. Orders should not get filled if they exceed a
    // certain amount of the market volume in a stock.
    // TODO: Implement market volume limit here



}