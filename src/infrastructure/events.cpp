//
// Created by Evan Kirkiles on 9/27/2018.
//

// Include corresponding header
#include "events.hpp"

// This file simply contains all the initializer lists for each Event object

// Scheduled Event initializer list
events::ScheduledEvent::ScheduledEvent(void *p_function, Strategy &p_strat, const BloombergLP::blpapi::Datetime &p_when) :
    Event("SCHEDULED", p_when),
    function(p_function),
    instance(p_strat)  {}

// Market Event initializer list
events::MarketEvent::MarketEvent(const std::vector<std::string> &p_symbols,
                                 const std::unordered_map<std::string, double> &p_data,
                                 const BloombergLP::blpapi::Datetime &p_when) :
    Event("MARKET", p_when),
    symbols(p_symbols),
    data(p_data) {}

 // Signal Event initializer list
events::SignalEvent::SignalEvent(const std::string &p_symbol, double p_percentage,
                                  const BloombergLP::blpapi::Datetime &p_when) :
    Event("SIGNAL", p_when),
    symbol(p_symbol),
    percentage(p_percentage) {}

// Order Event initializer list
events::OrderEvent::OrderEvent(const std::string &p_symbol, int p_quantity, const BloombergLP::blpapi::Datetime &p_when) :
    Event("ORDER", p_when),
    symbol(p_symbol),
    quantity(p_quantity) {}

// Fill Event initializer list
events::FillEvent::FillEvent(const std::string &p_symbol, int p_quantity, double p_cost, double p_slippage, double p_commission,
                             const BloombergLP::blpapi::Datetime &p_when) :
    Event("FILL", p_when),
    symbol(p_symbol),
    quantity(p_quantity),
    cost(p_cost),
    slippage(p_slippage),
    commission(p_commission) {}