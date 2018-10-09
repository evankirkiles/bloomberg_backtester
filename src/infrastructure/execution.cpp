//
// Created by Evan Kirkiles on 10/8/2018.
//

// Include corresponding header
#include "execution.hpp"

// Execution constructor is just an initializer list
ExecutionHandler::ExecutionHandler(std::queue<std::unique_ptr<events::Event>> *p_stack,
                                   std::list<std::unique_ptr<events::Event>> *p_heap, DataManager *p_data_manager,
                                   Portfolio *p_portfolio) :
       stack_eventlist(p_stack),
       heap_eventlist(p_heap),
       data_manager(p_data_manager),
       portfolio(p_portfolio) {}

// Takes a signal event and converts it into an Order Event. It first performs a MarketEvent to make sure the
// portfolio holdings are as up-to-date as possible.
void ExecutionHandler::process_signal(const events::SignalEvent &event) {

    // Before doing anything, recalculate portfolio holdings with a simulated MarketEvent
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> recentprice =
            std::move(data_manager->history({event.symbol}, {"PX_LAST"}, 1, "RECENT"));
    auto data = recentprice->at(event.symbol).data.rbegin();
    portfolio->update_market(events::MarketEvent({event.symbol}, {{event.symbol, data->second["PX_LAST"]}}, data->first));

    // Determine what percentage of the portfolio must be filled based on the totalholdings, heldcash, and current holdings
    // in the stock.
    double current_percent = portfolio->current_holdings[event.symbol] / portfolio->current_holdings[portfolio_fields::TOTAL_HOLDINGS];
}