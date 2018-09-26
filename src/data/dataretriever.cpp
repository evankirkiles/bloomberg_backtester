//
// Created by Evan Kirkiles on 9/25/2018.
//

// Include header
#include "dataretriever.hpp"
// Global backtester namespace
namespace backtester {

// Constructor to build an instance of the DataRetriever for the given type of data.
//
// @param type             The type of data which will be used for this data retriever.
//                          -> HISTORICAL_DATA, INTRADAY_DATA, REALTIME_DATA
//
DataRetriever::DataRetriever(const std::string &p_type) {
    // First initialize the session options with global session run settings.
    BloombergLP::blpapi::SessionOptions session_options;
    session_options.setServerHost(bloomberg_session::HOST);
    session_options.setServerPort(bloomberg_session::PORT);
    // Also build the Event Handler to receive all incoming events

}

// Constructor for the Data Handler, simply saving the object to fill as a local pointer member.
HistoricalDataHandler::HistoricalDataHandler(BloombergLP::blpapi::Element *p_target) : target(p_target) {}

// Data processing done using the request responses sent from Bloomberg API. In this case, historical data
// can be very large, so it may be split up into PARTIAL_RESPONSE objects instead of one RESPONSE. Either way,
// the data is interpreted and then returned as an Element object which contains the historical data requested.
// In case of an error, the object will not be filled.
bool HistoricalDataHandler::processEvent(const BloombergLP::blpapi::Event &event,
                                         BloombergLP::blpapi::Session *session) {

    // Iterates through the messages returned by the event
    BloombergLP::blpapi::MessageIterator msgIter(event);
    while (msgIter.next()) {
        // Get the message object
        BloombergLP::blpapi::Message msg = msgIter.message();
        // Make sure the message is either a partial response or a full response
        if ((event.eventType() != BloombergLP::blpapi::Event::PARTIAL_RESPONSE) &&
            (event.eventType() != BloombergLP::blpapi::Event::RESPONSE)) { continue; }

        // Get the elements from the message
        // Todo: figure out what is up with "SECURITY_DATA"
        BloombergLP::blpapi::Element securityData = msg.getElement(SECURITY_DATA);
    }
}

}
