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
// STL includes
#include <thread>
#include <mutex>
// Project includes
#include "constants.hpp"
#include "events.hpp"

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
class HistoricalDataRetriever {
public:
    // Constructor which initializes the session connection to Bloomberg API through which data will be requested.
    // The type parameter works to specify a type of data which will be handled by the instance of the HistoricalDataRetriever.
    // Options currently include HISTORICAL_DATA, but will eventually support INTRADAY_DATA.
    HistoricalDataRetriever(const std::string& type, int correlation_id = correlation_ids::HISTORICAL_REQUEST_CID);

    // On destruction, close the session before releasing the object
    ~HistoricalDataRetriever();

    // Pulls data for the given stocks at the
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> pullHistoricalData(
            const std::vector<std::string>& securities,
            const BloombergLP::blpapi::Datetime& start_date,
            const BloombergLP::blpapi::Datetime& end_Date,
            const std::vector<std::string>& fields = {"PX_LAST"},
            const std::string& frequency = "DAILY");

private:
    // The correlation ID for requests
    const int correlation_id;
    // The type of Data Retriever (HISTORICAL_DATA, INTRADAY_DATA)
    const std::string type;
    // The session across which member functions will pull data from Bloomberg. Is a unique pointer bc of RAII.
    std::unique_ptr<BloombergLP::blpapi::Session> session;
};

// Class for subscription-based data retrieval from the Bloomberg API. When the event calculations are finished
// and the stack is empty, the algorithm will unlock the event heap and wait for a new market event to be filled in
// or for the realtime date counter to exceed the datetime of the next ScheduledEvent.
class RealTimeDataRetriever {
public:
    // Constructor initializes the session subscription to Bloomberg API through which data will be passed.
    // The session and subscription will run the lifetime of the object in another thread, writing to a queue
    // which will be written into the main event HEAP whenever there is a free moment. This is done through
    // use of a mutex which locks the HEAP in the main thread until the event is finished processing, at which
    // the HEAP in the main thread is unlocked and LOCKED in the session thread while the queue is copied over.
    // Once that is finished, the queue in the other thread is emptied and the HEAP is locked back in the main thread.
    RealTimeDataRetriever(std::list<std::unique_ptr<events::Event>>* heap_eventlist, std::mutex* p_mtx,
                          int correlation_id = correlation_ids::LIVE_REQUEST_CID);

    // On destruction, close the session and end the subscription before releasing the object
    ~RealTimeDataRetriever();

    // Runs the subscription for a vector of stocks, placing the tick level data into a queue which acts as a
    // buffer to store temporary tick data until the mutex is unlocked and this class is able to write again.
    void runSubscription(const std::vector<std::string>& symbols);

    // Stops all subscriptions
    void stopSubscriptions();

private:
    // A pointer to the mutex which will be used for locking the HEAP
    std::mutex* mtx;
    // A buffer queue which holds the market events received until the mutex is unlocked
    std::queue<std::unique_ptr<events::Event>> buffer_queue;
    // A pointer to the heap which will be filled by the buffer queue
    std::list<std::unique_ptr<events::Event>>* heap_list;
    // The correlation ID for requests
    const int correlation_id;
    // The session through which the subscription will be run
    std::unique_ptr<BloombergLP::blpapi::Session> session;
    // The symbols subscribed to
    BloombergLP::blpapi::SubscriptionList subscriptions;
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
