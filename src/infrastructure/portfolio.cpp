//
// Created by Evan Kirkiles on 10/8/2018.
//

// Include corresponding header
#include "portfolio.hpp"

// Portfolio constructor which simply calls the reset_portfolio function to build the empty
// holdings and positions maps.
Portfolio::Portfolio(std::vector<std::string> p_symbol_list, unsigned int p_initial_capital,
                     const BloombergLP::blpapi::Datetime &p_start) :
                 symbol_list(std::move(p_symbol_list)) {
    reset_portfolio(p_initial_capital, p_start);
}

// Resets (or initializes) the positions and holdings maps. All holdings and all positions will be emptied,
// and current holdings and current positions will only contain the initial capital amount.
void Portfolio::reset_portfolio(unsigned int p_initial_capital, const BloombergLP::blpapi::Datetime &p_start) {

    // Reinitialize the initial cap and start
    initial_capital = p_initial_capital;
    start_date = p_start;

    // Clear all the maps before writing into them
    all_positions.clear();
    all_holdings.clear();
    current_positions.clear();
    current_holdings.clear();

    // Build the empty current map from the symbol list
    for (const auto& symbol : symbol_list) {
        current_positions[symbol] = 0;
        current_holdings[symbol] = 0;
    }

    // Add the non-symbol related rows into the current holdings as well
    current_holdings[portfolio_fields::HELD_CASH] = initial_capital;
    current_holdings[portfolio_fields::COMMISSION] = 0;
    current_holdings[portfolio_fields::SLIPPAGE] = 0;
    current_holdings[portfolio_fields::TOTAL_HOLDINGS] = initial_capital;
    current_holdings[portfolio_fields::RETURNS] = 0;
    current_holdings[portfolio_fields::EQUITY_CURVE] = 0;

    // Write the current holdings and positions into the historical map as first entry
    push_holdings_and_positions(start_date);
}

// Interprets the data from a market event and updates the holdings to reflect the latest price change.
void Portfolio::update_market(const events::MarketEvent &event) {

    // Get the data from the market event and update the current holdings
    for (const auto& symbol : event.symbols) {
        current_holdings[symbol] = current_positions[symbol] * event.data.at(symbol);
    }

    // Update returns and total holdings
    calculate_returns();
    // Finally put this new data into the all_holdings map
    push_holdings_and_positions(event.datetime);
}

// Writes the data in current holdings and current positions into the all_holdings and all_positions objects.
void Portfolio::push_holdings_and_positions(const BloombergLP::blpapi::Datetime &date) {
    all_holdings[date] = current_holdings;
    all_positions[date] = current_positions;
}

// Updates total holdings with the heldcash and all the symbols
void Portfolio::calculate_returns() {
    // Reiterate through the current holdings to get new total
    double total_holdings = current_holdings[portfolio_fields::HELD_CASH];
    for (const auto& i : symbol_list) { total_holdings += current_holdings[i]; }
    // Calculate returns as the ratio of new holdings to previous holdings
    double returns = (total_holdings / current_holdings[portfolio_fields::TOTAL_HOLDINGS]) - 1;
    current_holdings[portfolio_fields::TOTAL_HOLDINGS] = total_holdings;

    // Use the calculated returns and total holdings for equity curve and returns calculation
    current_holdings[portfolio_fields::RETURNS] = returns;
    current_holdings[portfolio_fields::EQUITY_CURVE] =
            (current_holdings[portfolio_fields::EQUITY_CURVE] + 1) * (returns + 1) - 1;
}

// Interprets the data from a fill event and updates the positions (and fill-related holdings). A fill event
// contains information about commission, slippage, and the transaction cost. Also, it relays the quantity
// change in the filled stock.
void Portfolio::update_fill(const events::FillEvent &event) {

    // Update the positions first
    current_positions[event.symbol] += event.quantity;

    // Update the holdings with calculated fill information
    current_holdings[event.symbol] += event.cost;
    current_holdings[portfolio_fields::COMMISSION] += event.commission;
    current_holdings[portfolio_fields::SLIPPAGE] += event.slippage;
    current_holdings[portfolio_fields::HELD_CASH] -= event.cost + event.commission + event.slippage;

    // Calculate returns stream
    calculate_returns();
    // Now push all this data into the historical map
    push_holdings_and_positions(event.datetime);
}