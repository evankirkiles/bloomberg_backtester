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

namespace backtester {

// Base class of DataManagers which simply defines the pure virtual function to pull historical data, which
// will be used by all Data Managers in the future.
class DataManager {
public:
    // Pulls history for N time units back from the current date (given to the function) given the parameters.
    // Simply passes a request to a DataRetriever and takes the new data down from Bloomberg.
    virtual std::unique_ptr<std::string, SymbolHistoricalData> history(
            const std::vector<std::string>& symbols,
            const std::vector<std::string>& fields,
            unsigned int timeunitsback,
            const std::string& frequency) = 0;
protected:
    // A reference to the 'current time' simulated by the backtester
    BloombergLP::blpapi::Datetime* currentTime;
};

// Class for the Historical Data Manager which is the direct link between an algorithm and the Bloomberg API.
// The Historical version is optimized for End of Day data for low-frequency trading algorithms, and does not work
// with other data types.
class HistoricalDataManager : public DataManager {
public:
    // Constructor to build the Historical Data Manager
    HistoricalDataManager(BloombergLP::blpapi::Datetime* currentTime);

    // Function that initializes the MarketEvents onto the event list in chronological order. Should be called before
    // any backtesting takes place, as it enables the Portfolio to calculate returns and holdings.
    void fillHistory(const std::vector<std::string> &symbols,
            const BloombergLP::blpapi::Datetime& start,
            const BloombergLP::blpapi::Datetime& end,
            std::list<std::unique_ptr<events::Event>>* location);

    // The inherited function override to pull history from Bloomberg API.
    std::unique_ptr<std::string, SymbolHistoricalData> history(
            const std::vector<std::string>& symbols,
            const std::vector<std::string>& fields,
            unsigned int timeunitsback,
            const std::string& frequency);
private:
    // The Data Retriever module itself used by the history and buildHistory functions to query Bloomberg API
    DataRetriever dr;
};

}

#endif //BACKTESTER_DATA_HPP
