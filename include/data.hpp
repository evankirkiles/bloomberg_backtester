//
// Created by bloomberg on 9/29/2018.
//

#ifndef BACKTESTER_DATA_HPP
#define BACKTESTER_DATA_HPP
// Include the Bloomberg includes
#include "bloombergincludes.hpp"
// Custom class includes
#include "dataretriever.hpp"
#include "events.hpp"
#include "daterules.hpp"

// Base class of DataManagers which simply defines the pure virtual function to pull historical data, which
// will be used by all Data Managers in the future.
class DataManager {
public:
    // Constructor initializes currentTime
    explicit DataManager(BloombergLP::blpapi::Datetime* p_currentTime) : currentTime(p_currentTime) {}

    // Pulls history for N time units back from the current date (given to the function) given the parameters.
    // Simply passes a request to a HistoricalDataRetriever and takes the new data down from Bloomberg.
    virtual std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> history(
            const std::vector<std::string>& symbols,
            const std::vector<std::string>& fields,
            unsigned int timeunitsback,
            const std::string& frequency) = 0;
protected:
    // A reference to the 'current time' simulated by the backtester
    BloombergLP::blpapi::Datetime* currentTime{};
};

// Class for the Historical Data Manager which is the direct link between an algorithm and the Bloomberg API.
// The Historical version is optimized for End of Day data for low-frequency trading algorithms, and does not work
// with other data types.
//
// To minimize the number of data calls to Bloomberg API, I have implemented a function called "preload" which
// performs a single call for data that includes all fields for every possible day that could be requested by
// the algorithm (specified in the parameters of the function call) Once preloaded, the history() function will draw
// data from this set of data rather than asking the Bloomberg API.
//
class HistoricalDataManager : public DataManager {
public:
    // Constructor to build the Historical Data Manager
    explicit HistoricalDataManager(BloombergLP::blpapi::Datetime* currentTime,
                                   int correlation_id = correlation_ids::HISTORICAL_REQUEST_CID);

    // Function that initializes the MarketEvents onto the event list in chronological order. Should be called before
    // any backtesting takes place, as it enables the Portfolio to calculate returns and holdings.
    void fillHistory(const std::vector<std::string> &symbols,
            const BloombergLP::blpapi::Datetime& start,
            const BloombergLP::blpapi::Datetime& end,
            std::list<std::unique_ptr<events::Event>>* location);

    // Preliminary data pull from Bloomberg API to limit repeated data calls.
    void preload(const std::vector<std::string> &symbols,
             const std::vector<std::string> &fields,
             const BloombergLP::blpapi::Datetime& start,
             const BloombergLP::blpapi::Datetime& end,
             unsigned int maxlookback,
             const std::string& frequency="DAILY");

    // The inherited function override to pull history from Bloomberg API or the preloaded set.
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> history(
            const std::vector<std::string>& symbols,
            const std::vector<std::string>& fields,
            unsigned int timeunitsback,
            const std::string& frequency) override;
private:
    // Specifies whether the data is pre-downloaded or not.
    bool preloaded = false;
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> preloaded_data;
    // The Data Retriever module itself used by the history and buildHistory functions to query Bloomberg API
    HistoricalDataRetriever dr;
};

#endif //BACKTESTER_DATA_HPP
