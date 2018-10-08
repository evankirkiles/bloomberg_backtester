#include <utility>

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
void Portfolio::reset_portfolio(unsigned int initial_capital, const BloombergLP::blpapi::Datetime &start) {

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
    current_holdings["heldcash"] = initial_capital;
    current_holdings["commission"] = 0;
    current_holdings["slippage"] = 0;
    current_holdings["totalholdings"] = initial_capital;
}

//