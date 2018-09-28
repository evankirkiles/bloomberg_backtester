//
// Created by Evan Kirkiles on 9/27/2018.
//

#ifndef BACKTESTER_DATA_HPP
#define BACKTESTER_DATA_HPP

// Bloomberg includes
#include "bloombergincludes.hpp"
// Custom class includes
#include "dataretriever.hpp"
#include "events.hpp"

namespace backtester {

// Base class for the Data Managers which will be implemented eventually. Right now, only inherited by Historical.
struct DataManager {
    // Virtual function to pull history N days back from a given date at a specified interval for specific symbols.
    // Needs to be inherited because will have different implementation for every Data Handler type.
    virtual std::unordered_map<std::string, SymbolHistoricalData> history(
            const BloombergLP::blpapi::Datetime& currentTime, const std::vector<std::string>& symbol_list,
            const std::vector<std::string>& fields, const std::string& interval, unsigned int days) = 0;
};

// Class for the data interface which will be used by Strategies. Components needed include a function to
// get market data N days back from a given date (currentDate will be passed in by strategy)
//
// @member dr           The Data Retriever for the Data Manager
//
class HistoricalDataManager : public DataManager {
public:
    // Constructor builds the data manager with a data retriever connected to the Bloomberg API.
    explicit HistoricalDataManager();

    // Function that builds the Market Events and puts them onto the HEAP event list in chronological order. Does so
    // by first pulling the EOD price data for the securities to be traded by the algorithm and then generating Market
    // Events for each date. Goes through each symbol's data one entry at a time at the same time, comparing in case of
    // dates not lining up and then catching up the iterators.
    void fillHistory(std::list<std::unique_ptr<events::Event>>* map);

    // Inherited function from abstract base class DataManager. Gets data from N days back, pulled dynamically onto
    // the actual application stack (not the Event STACK) and then deleted afterwards.
    std::unordered_map<std::string, SymbolHistoricalData> history(
            const BloombergLP::blpapi::Datetime& currentTime, const std::vector<std::string>& symbol_list,
            const std::vector<std::string>& fields, const std::string& interval, unsigned int days);

private:
    // Instance of the Data Retriever which will pull all information from Bloomberg.
    DataRetriever dr;
};

}

#endif //BACKTESTER_DATA_HPP
