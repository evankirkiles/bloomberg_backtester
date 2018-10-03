//
// Created by Evan Kirkiles on 9/27/2018.
//

// Include corresponding header
#include "events.hpp"

// This file simply contains all the initializer lists for each Event object
namespace events {

// Market Event initializer list
MarketEvent::MarketEvent(const std::vector<std::string> &p_symbols,
                         const std::unordered_map<std::string, double> &p_data,
                         const BloombergLP::blpapi::Datetime &p_when) :
        Event("MARKET", p_when),
        symbols(p_symbols),
        data(p_data) {}

// Print function for MarketEvent
void MarketEvent::what() {
    std::cout << "Event: MARKET\nDatetime: " << datetime << "\nSymbols: ";
    for (const std::string &i : symbols) { std::cout << i << ", "; };
    std::cout << "\b\n";
}

// Signal Event initializer list
SignalEvent::SignalEvent(const std::string &p_symbol, double p_percentage,
                         const BloombergLP::blpapi::Datetime &p_when) :
        Event("SIGNAL", p_when),
        symbol(p_symbol),
        percentage(p_percentage) {}

// Print function for the SignalEvent
void SignalEvent::what() {
    std::cout << "Event: SIGNAL\nDatetime: N/A\nSymbol: " << symbol << "\nPercentage: " << percentage << "%\n";
}

// Order Event initializer list
OrderEvent::OrderEvent(const std::string &p_symbol, int p_quantity) :
        Event("ORDER", BloombergLP::blpapi::Datetime(1970, 1, 1, 0, 0, 0, 0)),
        symbol(p_symbol),
        quantity(p_quantity) {}

// Print function for the OrderEvent
void OrderEvent::what() {
    std::cout << "Event: ORDER\nDatetime: " << datetime << "\nSymbol: " << symbol << "\nQuantity: "
              << quantity << "\n";
}

// Fill Event initializer list
FillEvent::FillEvent(const std::string &p_symbol, int p_quantity, double p_cost, double p_slippage,
                     double p_commission) :
        Event("FILL", BloombergLP::blpapi::Datetime(1970, 1, 1, 0, 0, 0, 0)),
        symbol(p_symbol),
        quantity(p_quantity),
        cost(p_cost),
        slippage(p_slippage),
        commission(p_commission) {}

// Print function for the FillEvent
void FillEvent::what() {
    std::cout << "Event: FILL\nDatetime: N/A\nSymbol: " << symbol << "\nQuantity: " << quantity
              << "\nCost: " << cost << "\nSlippage: " << slippage << "\nCommission: " << commission << "\n";
}

}