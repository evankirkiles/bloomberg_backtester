//
// Created by Evan Kirkiles on 9/25/2018.
//

#ifndef BACKTESTER_DATARETRIEVER_HPP
#define BACKTESTER_DATARETRIEVER_HPP\
// Include stringstream for inline date formatter
#include <sstream>
#include <iomanip>
// Bloomberg API includes
#include "bloombergincludes.hpp"
// Project constant includes
#include "constants.hpp"

// Inline function to parse the Bloomberg Historical Data formatted date from a normal Datetime.
inline std::string get_date_formatted(const BloombergLP::blpapi::Datetime& date) {
    std::stringstream str;
    str << date.year() << std::setw(2) << std::setfill('0') << date.month() << std::setw(2) << std::setfill('0') << date.day();
    return str.str();
}

// The single-symbol type which is returned from a history call. It contains a single security with dates and
// various fields in an ordered map of unsigned longs and Elements holding all the data for the day.
struct SymbolHistoricalData {
    std::string symbol;
    std::map<BloombergLP::blpapi::Datetime, std::unordered_map<std::string, double>> data;

    // Function to append historical datas together from the same symbol
    void append(const SymbolHistoricalData& other) {
        // Ensure the data is for the same symbol
        if (other.symbol != symbol) { throw std::runtime_error("Cannot append two SymbolHistoricalData's of different symbols!"); }
        data.insert(other.data.begin(), other.data.end()); }
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

    // On destruction, close the session before releasing the object
    ~DataRetriever();

    // Pulls data for the given stocks at the
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> pullHistoricalData(
            const std::vector<std::string>& securities,
            const BloombergLP::blpapi::Datetime& start_date,
            const BloombergLP::blpapi::Datetime& end_Date,
            const std::vector<std::string>& fields = {"PX_LAST"},
            const std::string& frequency = "DAILY");

private:
    // The type of Data Retriever (HISTORICAL_DATA, INTRADAY_DATA, or REALTIME_DATA
    const std::string type;
    // The session across which member functions will pull data from Bloomberg. Is a unique pointer bc of RAII.
    std::unique_ptr<BloombergLP::blpapi::Session> session;
};

// Class which is the direct link between the program and the messages received by the Bloomberg API. Is passed
// in to the session instance so all messages being sent by the session are interpreted by this class. Inherits
// from the base Bloomberg Event Handler class. This version of the data handler is specific for historical data
// and thus should only be used with DataRetrievers of type HISTORICAL_DATA.
struct HistoricalDataHandler {
    // Constructor initializes the unique ptr to empty unordered map
    HistoricalDataHandler();
    // The event handler logic function which receives data packets from Bloomberg API. Returns false until
    // the event passed in is a Response object, at which point the data is done streaming.
    bool processResponseEvent(const BloombergLP::blpapi::Event &event);

    // Makes sure the message is valid before parsing the fields from it
    bool processExceptionsAndErrors(BloombergLP::blpapi::Message msg);

    // A pointer to the object into which historical data is filled.
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> target;
};

#endif //BACKTESTER_DATARETRIEVER_HPP
