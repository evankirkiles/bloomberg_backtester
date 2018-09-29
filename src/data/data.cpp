//
// Created by Evan Kirkiles on 9/27/2018.
//

// Include the correlated header
#include "data.hpp"

namespace backtester {

// Constructor that sets up the connection to the Bloomberg Data API so data can be pulled.
HistoricalDataManager::HistoricalDataManager() : dr("HISTORICAL") {}

// Function that builds the Market Events and puts them onto the HEAP event list in chronological order. Does so
// by first pulling the EOD price data for the securities to be traded by the algorithm and then generating Market
// Events for each date. Goes through each symbol's data one entry at a time at the same time, comparing in case of
// dates not lining up and then catching up the iterators.
void HistoricalDataManager::fillHistory(const std::vector<std::string> &symbols,
                                        const BloombergLP::blpapi::Datetime& start,
                                        const BloombergLP::blpapi::Datetime& end,
                                        std::list<std::unique_ptr<events::Event>>* location) {

    // First retrieve the array of daily end of date prices
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> data =
            dr.pullHistoricalData(symbols, start, end);

    // Now iterate through the arr
}

}