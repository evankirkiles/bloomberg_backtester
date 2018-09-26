//
// Created by Evan Kirkiles on 9/25/2018.
//

#ifndef BACKTESTER_DATARETRIEVER_HPP
#define BACKTESTER_DATARETRIEVER_HPP
// Bloomberg API includes
#include "bloombergincludes.hpp"
// Project constant includes
#include "constants.hpp"

// Global namespace
namespace backtester {

// The single-symbol type which is returned from a history call. It contains a single security with dates and
// various fields in an ordered map of unsigned longs and Elements holding all the data for the day.
struct SymbolHistoricalData {
    std::string symbol;
    std::map<BloombergLP::blpapi::Datetime, BloombergLP::blpapi::Element> data;
};

// Class that contains the methods for data retrieval from Bloomberg API. In the future it will be
// modified to support subscriptions, but at the moment is only needed for backtesting and thus
// only gets historical data.
//
class DataRetriever {
public:
    // Constructor which initializes the session connection to Bloomberg API through which data will be requested.
    // The type parameter works to specify a type of data which will be handled by the instance of the DataRetriever.
    // Options currently include HISTORICAL_DATA, but will eventually support INTRADAY_DATA and REALTIME_DATA.
    explicit DataRetriever(const std::string& type);

    // Pulls data for the given stocks at the
    BloombergLP::blpapi::Message pullHistoricalData(
            const std::vector<std::string>& securities,
            const std::string& start_date,
            const std::string& end_Date,
            const std::vector<std::string>& fields = {"PX_LAST"},
            const std::string& frequency = "DAILY");

private:
    // The type of Data Retriever (HISTORICAL_DATA, INTRADAY_DATA, or REALTIME_DATA
    const std::string type;
    // The session across which member functions will pull data from Bloomberg. Is a unique pointer bc of RAII.
    std::unique_ptr<BloombergLP::blpapi::Session> session;
    // The handler for the messages from Bloomberg servers.
    std::unique_ptr<BloombergLP::blpapi::EventHandler> event_handler;
};

// Class which is the direct link between the program and the messages received by the Bloomberg API. Is passed
// in to the session instance so all messages being sent by the session are interpreted by this class. Inherits
// from the base Bloomberg Event Handler class. This version of the data handler is specific for historical data
// and thus should only be used with DataRetrievers of type HISTORICAL_DATA.
class HistoricalDataHandler : public BloombergLP::blpapi::EventHandler {
public:
    // Default constructor
    explicit HistoricalDataHandler(std::unordered_map<std::string, SymbolHistoricalData>* target);
    // The event handler logic function which receives data packets from Bloomberg API.
    bool processEvent(const BloombergLP::blpapi::Event &event, BloombergLP::blpapi::Session *session) override;

    // Makes sure the message is valid before parsing the fields from it
    bool processExceptions(BloombergLP::blpapi::Message msg);
    bool processErrors(BloombergLP::blpapi::Message msg);
private:
    // A pointer to the object into which historical data is filled.
    std::unordered_map<std::string, SymbolHistoricalData> *target;
};

}

#endif //BACKTESTER_DATARETRIEVER_HPP
