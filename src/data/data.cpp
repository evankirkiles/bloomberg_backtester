//
// Created by Evan Kirkiles on 9/27/2018.
//

// Include the correlated header
#include "data.hpp"

namespace backtester {

// Constructor that sets up the connection to the Bloomberg Data API so data can be pulled.
HistoricalDataManager::HistoricalDataManager(BloombergLP::blpapi::Datetime* p_currentTime) :
        currentTime(p_currentTime), dr("HISTORICAL") {}

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

    // Build an iterator off of the first stock's data to get all dates. Then, we can use the dates to build
    // packaged MarketEvents of multiple symbol data.
    // Potential data issues: what happens when a stock started trading after start date? Ends before end date?
    // Also, I don't know how Bloomberg handles missing values, which will be a weird occurrence when it shows up.
    for (auto i = data->operator[](symbols[0]).data.begin(); i != data->operator[](symbols[0]).data.end(); ++i) {
        // Placeholder data unordered map which will contain the update information
        std::unordered_map<std::string, double> temp = {{symbols[0], i->second["PX_LAST"]}};
        for (int j = 1; j < symbols.size(); ++j) {
            temp[symbols[j]] = data->operator[](symbols[j]).data[i->first]["PX_LAST"];
        }

        // Now build the Market Event and place it onto the HEAP as a unique ptr
        location->emplace_back(std::make_unique<events::MarketEvent>(symbols, temp, i->first));
    }
}

}