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
DataRetriever::DataRetriever(const std::string &p_type) : type(p_type) {
    // First initialize the session options with global session run settings.
    BloombergLP::blpapi::SessionOptions session_options;
    session_options.setServerHost(bloomberg_session::HOST);
    session_options.setServerPort(bloomberg_session::PORT);
    // Also initialize the Session connection to Bloomberg Communications
    session = std::make_unique<BloombergLP::blpapi::Session>(session_options);
    // Open up the session in preparation for data requests
    if (!session->start()) {
        throw std::runtime_error("Failed to start session! Aborting.");
    };
}

// Destructor which simply closes the connection to the Bloomberg API
DataRetriever::~DataRetriever() { session->stop(); }

// Generates a request to Bloomberg for the data specified in the parameters. This function is only for
// historical data retrievers, it should NOT be run on subscription-based or intraday retrievers.
//
// @param securities       A vector of the securities to request data from, ex. "IBM US EQUITY"
// @param start_date       The date from which to begin pulling data.
// @param end_date         The upper limit on pulled dates.
// @param fields           The fields of data desired, ex. "PX_LAST", "OPEN"
// @param frequency        The frequency of the data, ex. "DAILY", "MONTHLY"
//
std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>>
DataRetriever::pullHistoricalData(const std::vector<std::string> &securities,
                                                               const BloombergLP::blpapi::Datetime& start_date,
                                                               const BloombergLP::blpapi::Datetime& end_date,
                                                               const std::vector<std::string> &fields,
                                                               const std::string &frequency) {

    // Ensure that this instance of DataRetriever is able to take historical data
    if (type != "HISTORICAL_DATA") { return nullptr; }

    // First open the pipeline for getting historical data by using the Reference Data market service.
    // Then proceed to build a request based on the given parameters.
    BloombergLP::blpapi::Service histDataService = session->getService(bloomberg_services::REFDATA);
    BloombergLP::blpapi::Request request = histDataService.createRequest("HistoricalDataRequest");
    // Append the parameters to their respective fields in the request
    for (const std::string& i : securities) { request.append("securities", i.c_str()); }
    for (const std::string& i : fields) { request.append("fields", i.c_str()); }
    request.set("startDate", start_date);
    request.set("endDate", end_date);
    request.set("periodicitySelection", frequency.c_str());

    // Build an Event queue onto which to receive the data which will be handled
    BloombergLP::blpapi::EventQueue queue;
    session->sendRequest(request, BloombergLP::blpapi::CorrelationId(1), &queue);
    // Handle events as they come in to the queue
    HistoricalDataHandler handler;
    bool responseFinished = false;
    while(!responseFinished) {
        // Check if the getting the next event is possible. If it is, pull it through the handler. If not,
        // continue running loop.
        BloombergLP::blpapi::Event event;
        if (queue.tryNextEvent(&event) == 0) {
            responseFinished = handler.processResponseEvent(event);
        }
    }
    // When event finishes, return the handler's data map pointer
    return std::move(handler.target);
}

// Data processing done using the request responses sent from Bloomberg API. In this case, historical data
// can be very large, so it may be split up into PARTIAL_RESPONSE objects instead of one RESPONSE. Either way,
// the data is interpreted and then returned as an Element object which contains the historical data requested.
// In case of an error, the object will not be filled. This function returns false
bool HistoricalDataHandler::processResponseEvent(const BloombergLP::blpapi::Event &event) {

    // Iterates through the messages returned by the event
    BloombergLP::blpapi::MessageIterator msgIter(event);
    while (msgIter.next()) {
        // Get the message object
        BloombergLP::blpapi::Message msg = msgIter.message();
        // Make sure the message is either a partial response or a full response
        if ((event.eventType() != BloombergLP::blpapi::Event::PARTIAL_RESPONSE) &&
            (event.eventType() != BloombergLP::blpapi::Event::RESPONSE)) { continue; }

        // Make sure did not run into any errors
        if (!processExceptions(msg)) {
            if (!processErrors(msg)) {
                // Now process the fields and load them into the pointed to target object
                BloombergLP::blpapi::Element security_data = msg.getElement(element_names::SECURITY_DATA);
                // Build an empty SymbolHistoricalData for the symbol of the event
                SymbolHistoricalData shd;
                shd.symbol = security_data.getElementAsString(element_names::SECURITY_NAME);

                // Now get the field data and put it into the target map
                BloombergLP::blpapi::Element field_data = security_data.getElement(element_names::FIELD_DATA);
                if (field_data.numValues() > 0) {
                    for (int i = 0; i < field_data.numValues(); ++i) {
                        // Get the element and its date and put them into the SymbolHistoricalData object
                        BloombergLP::blpapi::Element element = field_data.getValueAsElement(static_cast<size_t>(i));
                        BloombergLP::blpapi::Datetime date = element.getElementAsDatetime(element_names::DATE);

                        // Put the information into the SymbolHistoricalData
                        shd.data[date] = element;
                    }
                }

                // Once all the data has been entered, append the SymbolHistoricalData to the target
                target->operator[](shd.symbol) = shd;
            } else {
                // Log that an error occurred for the event
                std::cout << "Error occurred! Cannot pull security data." << std::endl;
            }
        } else {
            // Log that an exception occurred for the event
            std::cout << "Exception occurred! Cannot pull security data." << std::endl;
        }
    }

    // If the event processed was of type RESPONSE, then it was the last one and thus it is no longer
    // necessary to check for responses
    return event.eventType() == BloombergLP::blpapi::Event::RESPONSE;
}

// Handles any exceptions in the message received from Bloomberg.
bool HistoricalDataHandler::processExceptions(BloombergLP::blpapi::Message msg) {
    BloombergLP::blpapi::Element security_data = msg.getElement(element_names::SECURITY_DATA);
    BloombergLP::blpapi::Element field_exceptions = security_data.getElement(element_names::FIELD_EXCEPTIONS);

    // Ensure there are no field exceptions
    if (field_exceptions.numValues() > 0) {
        BloombergLP::blpapi::Element element = field_exceptions.getValueAsElement(0);
        BloombergLP::blpapi::Element field_id = element.getElement(element_names::FIELD_ID);
        BloombergLP::blpapi::Element error_info = element.getElement(element_names::ERROR_INFO);
        BloombergLP::blpapi::Element error_message = error_info.getElement(element_names::ERROR_MESSAGE);
        // Log the exceptions found to the console and return true for errors found
        std::cout << field_id << std::endl;
        std::cout << error_message << std::endl;
        return true;
    }
    return false;
}

// Handles any errors in the data received from Bloomberg.
bool HistoricalDataHandler::processErrors(BloombergLP::blpapi::Message msg) {
    BloombergLP::blpapi::Element security_data = msg.getElement(element_names::SECURITY_DATA);

    // If a security error occurred, notify the user
    if (security_data.hasElement(element_names::SECURITY_ERROR)) {
        BloombergLP::blpapi::Element security_error = security_data.getElement(element_names::SECURITY_ERROR);
        BloombergLP::blpapi::Element error_message = security_error.getElement(element_names::ERROR_MESSAGE);
        // Log the errors found to the console and return true for errors found
        std::cout << error_message << std::endl;
        return true;
    }
    return false;
}

}
