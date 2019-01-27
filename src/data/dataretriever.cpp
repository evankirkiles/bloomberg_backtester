//
// Created by Evan Kirkiles on 9/25/2018.
//

// Include header
#include <mutex>
#include "dataretriever.hpp"

// Constructor to build an instance of the HistoricalDataRetriever for the given type of data.
//
// @param type             The type of data which will be used for this data retriever.
//                          -> HISTORICAL_DATA, INTRADAY_DATA, REALTIME_DATA
//
HistoricalDataRetriever::HistoricalDataRetriever(const std::string &p_type, int p_correlation_id) :
        type(p_type), correlation_id(p_correlation_id) {
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
HistoricalDataRetriever::~HistoricalDataRetriever() { session->stop(); }

// Generates a request to Bloomberg for the data specified in the parameters. This function is only for
// historical data retrievers, it should NOT be run on subscription-based or intra-day retrievers.
//
// @param securities       A vector of the securities to request data from, ex. "IBM US EQUITY"
// @param start_date       The date from which to begin pulling data.
// @param end_date         The upper limit on pulled dates.
// @param fields           The fields of data desired, ex. "PX_LAST", "PX_OPEN"
// @param frequency        The frequency of the data, ex. "DAILY", "MONTHLY"
//
std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>>
HistoricalDataRetriever::pullHistoricalData(const std::vector<std::string> &securities,
                                                               const BloombergLP::blpapi::Datetime& start_date,
                                                               const BloombergLP::blpapi::Datetime& end_date,
                                                               const std::vector<std::string> &fields,
                                                               const std::string &frequency) {

    // Ensure that this instance of HistoricalDataRetriever is able to take historical data
    if (type != "HISTORICAL_DATA") { throw std::runtime_error("Not historical data retriever!"); }

    // First open the pipeline for getting historical data by using the Reference Data market service.
    // Then proceed to build a request based on the given parameters.
    session->openService(bloomberg_services::REFDATA);
    BloombergLP::blpapi::Service histDataService = session->getService(bloomberg_services::REFDATA);
    BloombergLP::blpapi::Request request = histDataService.createRequest("HistoricalDataRequest");
    // Append the parameters to their respective fields in the request
    for (const std::string& i : securities) { request.append("securities", i.c_str()); }
    for (const std::string& i : fields) { request.append("fields", i.c_str()); }
    request.set("startDate", get_date_formatted(start_date).c_str());
    request.set("endDate", get_date_formatted(end_date).c_str());
    request.set("periodicitySelection", frequency.c_str());

    // Build an Event queue onto which to receive the data which will be handled
    BloombergLP::blpapi::EventQueue queue;
    session->sendRequest(request, BloombergLP::blpapi::CorrelationId(correlation_id), &queue);
    // Handle events as they come in to the queue
    HistoricalDataHandler handler;
    bool responseFinished = false;
    while(!responseFinished) {
        // Check if the getting the next event is possible. If it is, pull it through the handler. If not,
        // continue running loop.
        BloombergLP::blpapi::Event event;
        if (queue.tryNextEvent(&event) == 0) { responseFinished = handler.processResponseEvent(event); }
    }
    // When event finishes, return the handler's data map pointer
    return std::move(handler.target);
}

// Builds the Real Time data retriever for sessions and subscriptions of data. This constructor initializes
// members and builds the session which will be run when runSubscription is called.
RealTimeDataRetriever::RealTimeDataRetriever(pthread_mutex_t* p_mtx, int p_correlation_id) :
        correlation_id(p_correlation_id),
        data_handler(&buffer_queue, p_mtx) {

    // First initialize the session options with global session run settings.
    BloombergLP::blpapi::SessionOptions session_options;
    session_options.setServerHost(bloomberg_session::HOST);
    session_options.setServerPort(bloomberg_session::PORT);
    // Also initialize the Session connection to Bloomberg Communications
    session = std::make_unique<BloombergLP::blpapi::Session>(session_options, &data_handler);
    // Open up the session in preparation for data requests
    if (!session->start()) {
        throw std::runtime_error("Failed to start session! Aborting.");
    };
}

// Destructor which closes the connection to the Bloomberg API.
RealTimeDataRetriever::~RealTimeDataRetriever() { stopSubscriptions(); session->stop(); }

// Begins the subscription for a vector of symbols, getting the PX_LAST data for each one and building it
// into a MarketEvent which is put onto the buffer queue. Once the mutex unlocks, the queue is filled
// into the heap event list and then emptied.
void RealTimeDataRetriever::runSubscription(const std::vector<std::string>& symbols) {
    // Add all the tickers to the subscription
    for (const auto& i : symbols) {
        subscriptions.add(i.c_str(), "LAST_PRICE", "", BloombergLP::blpapi::CorrelationId((char*)i.c_str()));
    }
    // Run the subscription
    session->openService(bloomberg_services::MKTDATA);
    session->subscribe(subscriptions);
}

// Stops all subscriptions
void RealTimeDataRetriever::stopSubscriptions() {
    session->unsubscribe(subscriptions);
}

// Constructor for the EventHandler for realtime data
RealTimeDataHandler::RealTimeDataHandler(std::queue<std::unique_ptr<events::Event>> *p_queue, pthread_mutex_t* p_mtx) :
        queue(p_queue), mtx(p_mtx) {}

// Process the events received through the subscription into the queue, only when the mutex is unlocked. Otherwise,
// the events remain in the session until the EventHandler is able to be unlocked and retrieve them.
bool RealTimeDataHandler::processEvent(const BloombergLP::blpapi::Event &event,
                                       BloombergLP::blpapi::Session *session) {

    // Only interested in market data events related to the subscription
    if (event.eventType() == BloombergLP::blpapi::Event::SUBSCRIPTION_DATA) {
        // Iterate through the messages returned
        BloombergLP::blpapi::MessageIterator msgIter(event);
        while (msgIter.next()) {
            // Get one message and store it in message
            BloombergLP::blpapi::Message msg = msgIter.message();
            // Get the symbol
            std::string ticker = (char*)msg.correlationId().asPointer();
            // If the elements are present, then get the last price and write it to the queue
            if (msg.hasElement("LAST_TRADE", true)) {

                // Block until the mutex is unlocked so the queue is editable by this.
                // The mutex is automatically unlocked once this unique lock is destroyed (at end of emplace).
                pthread_mutex_lock(mtx);

                // Build the MarketEvent to place onto the queue
                queue->emplace(std::unique_ptr<events::MarketEvent>(new events::MarketEvent({ticker},
                                {{ticker, msg.getElementAsFloat64("LAST_TRADE")}},
                                msg.getElementAsDatetime("TIME"))));

                // Unblock the threads
                pthread_mutex_unlock(mtx);
            }
        }
    }

    // Returns true if processed event
    return true;
}

// Initializes the unique ptr to the unordered map to be returned
HistoricalDataHandler::HistoricalDataHandler() :
    target(std::make_unique<std::unordered_map<std::string, SymbolHistoricalData>>()) {}

// Data processing done using the request responses sent from Bloomberg API. In this case, historical data
// can be very large, so it may be split up into PARTIAL_RESPONSE objects instead of one RESPONSE. Either way,
// the data is interpreted and then returned as an Element object which contains the historical data requested.
// In case of an error, the object will not be filled. This function returns false
bool HistoricalDataHandler::processResponseEvent(const BloombergLP::blpapi::Event &event) {

    // Make sure the message is either a partial response or a full response
    if ((event.eventType() != BloombergLP::blpapi::Event::PARTIAL_RESPONSE) &&
        (event.eventType() != BloombergLP::blpapi::Event::RESPONSE)) { return false; }

    // Iterates through the messages returned by the event
    BloombergLP::blpapi::MessageIterator msgIter(event);
    while (msgIter.next()) {
        // Get the message object
        BloombergLP::blpapi::Message msg = msgIter.message();

        // Make sure did not run into any errors
        if (!processExceptionsAndErrors(msg)) {
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

                        // Set the times on the dates to be after the market closes (5:00 P.M. suffices)
                        date.setHours(17);
                        date.setMinutes(0);
                        date.setSeconds(0);

                        // Put the information into the SymbolHistoricalData
                        for (int j = 1; j < element.numElements(); ++j) {
                            // I haven't seen any documentation for missing values, which could potentially throw
                            // a stick in the wheels of the backtester. If I run into them, I will update the code here.
                            BloombergLP::blpapi::Element e = element.getElement(static_cast<size_t>(j));
                            shd.data[date][e.name().string()] = e.getValueAsFloat64();
                        }
                    }
                }

                // Once all the data has been entered, append the SymbolHistoricalData to the target
                if (target->find(shd.symbol) != target->end()) {
                    target->operator[](shd.symbol).append(shd);
                } else {
                    target->operator[](shd.symbol) = shd;
                }

        } else {
            // Log that an exception occurred for the event
            std::cout << "Exception or error occurred! Cannot pull security data." << std::endl;
        }
    }

    // If the event processed was of type RESPONSE, then it was the last one and thus it is no longer
    // necessary to check for responses
    return event.eventType() == BloombergLP::blpapi::Event::RESPONSE;
}

// Handles any exceptions in the message received from Bloomberg.
bool DataHandler::processExceptionsAndErrors(BloombergLP::blpapi::Message msg) {
    // If there is no security data, return a call to processErrors
    if (!msg.hasElement(element_names::SECURITY_DATA)) {
        if (msg.hasElement(element_names::RESPONSE_ERROR)) {
            // Get the response error element as it contains the error code
            BloombergLP::blpapi::Element response_data = msg.getElement(element_names::RESPONSE_ERROR);

            // Log the error message, code, and category/subcategory
            BloombergLP::blpapi::Element error_category = response_data.getElement(element_names::CATEGORY);
            BloombergLP::blpapi::Element error_message = response_data.getElement(element_names::MESSAGE);
            BloombergLP::blpapi::Element error_code = response_data.getElement(element_names::CODE);
            BloombergLP::blpapi::Element error_subcategory = response_data.getElement(element_names::SUBCATEGORY);
            std::cout << "Error pulling data: \"" << error_message.getValueAsString() << "\" with code " <<
                      error_code.getValueAsInt32() << " and category " << error_category.getValueAsString() << "::" <<
                      error_subcategory.getValueAsString() << std::endl;

        }
        return true;
    }

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
