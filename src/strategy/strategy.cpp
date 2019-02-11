#include <utility>

//
// Created by Evan Kirkiles on 9/27/2018.
//

// Include the corresponding header
#include "strategy.hpp"

// Builds the parent abstract class
BaseStrategy::BaseStrategy(std::vector<std::string> p_symbol_list, unsigned int p_initial_capital,
                           const BloombergLP::blpapi::Datetime &p_start, const BloombergLP::blpapi::Datetime &p_end) :
            symbol_list(std::move(p_symbol_list)),
            initial_capital(p_initial_capital),
            start_date(p_start),
            current_time(p_start),
            end_date(p_end),
            date_rules(p_start, p_end),
            time_rules(),
            portfolio(symbol_list, p_initial_capital, p_start) {
}

// Orders stocks up to the target percentage, simply by converting the params into signal events
void BaseStrategy::order_target_percent(const std::string &symbol, double percent) {
    stack_eventqueue.emplace(std::make_unique<events::SignalEvent>(symbol, percent, current_time));
}

// Saves the state of the strategy to the text file specified in JSON format
void BaseStrategy::save_state(const std::string &filepath) {
    // Open the file, truncating first
    std::ofstream file;
    file.open(filepath, std::ios_base::out | std::ios_base::trunc);
    // Each row represents a different set of variables
    // 0: portfolio current holdings
    // 1: portfolio current positions
    // 2: context map
    // 3: symbolspecifics map
    nlohmann::json currentholds(portfolio.current_holdings);
    file << currentholds << "\n";
}

// Logs a message to the console with the current time
void BaseStrategy::log(const std::string &message) { std::cout << "[" << current_time << "] " << message << std::endl; }
// Logs a message to Slack with the current time (to be done later)
void BaseStrategy::message(const std::string &message) {
    system(std::string(R"(C:\python27\python.exe C:\Users\bloomberg\CLionProjects\bloomberg_backtester\src\python\slackmanagement.py -m=")").append(message + "\"").c_str());
}

// Builds the Strategy object with the given initial capital and start and end. To reformat the strategy,
// probably should just reconstruct it.
Strategy::Strategy(const std::vector<std::string>& p_symbol_list,
                   unsigned int p_initial_capital,
                   const BloombergLP::blpapi::Datetime &p_start_date,
                   const BloombergLP::blpapi::Datetime &p_end_date,
                   const std::string& p_backtest_type) :
           BaseStrategy(p_symbol_list, p_initial_capital, p_start_date, p_end_date),
           backtest_type(p_backtest_type),
           data(std::make_shared<HistoricalDataManager>(&current_time,
                   // Ternary used for setting the correlation ID
                   (p_backtest_type == "HISTORICAL" ? correlation_ids::HISTORICAL_REQUEST_CID :
                    p_backtest_type == "INTRADAY" ? correlation_ids::INTRADAY_REQUEST_CID : correlation_ids::LIVE_REQUEST_CID)
           )),
           execution_handler(&stack_eventqueue, &heap_eventlist, data, &portfolio) {

    // Depending on type of data, do different actions to upon initialization
    if (backtest_type == "HISTORICAL") {
        // Make sure to fill the HEAP event list with the MarketEvents.
        auto hist_data = dynamic_cast<HistoricalDataManager*>(data.get());
        hist_data->fillHistory(symbol_list, start_date, end_date, &heap_eventlist);
    }
}

// Runs the strategy by iterating through the HEAP event list until it is empty
void Strategy::run() {
    // Place the iterator onto the heap eventlist
    running = true;

    // Use a boolean value to allow for exiting after a loop
    while(running && (!heap_eventlist.empty() || !stack_eventqueue.empty())) {
        std::unique_ptr<events::Event> event;
        // If the STACK is empty, take in events on the HEAP and process them, otherwise go through
        // the events on the STACK until it is empty. STACk always starts out empty.
        if (!stack_eventqueue.empty()) {
            // Get the first event
            event = std::move(stack_eventqueue.front());
            stack_eventqueue.pop();
        } else {
            // Iterate the currentEvent forwards and retrieve its object
            event = std::move(heap_eventlist.front());
            heap_eventlist.pop_front();
        }

        // Set the current time to the datetime of the event
        current_time = event->datetime;
        // Now downcast the event and perform whatever function it requires
        if (event->type == "MARKET") {
            events::MarketEvent event_market = *dynamic_cast<events::MarketEvent *>(event.release());
            // Pass the market event into the portfolio to update holdings
            portfolio.update_market(event_market);

        } else if (event->type == "SIGNAL") {
            events::SignalEvent event_signal = *dynamic_cast<events::SignalEvent *>(event.release());
            // Pass the signal event into the execution handler to generate orders
            execution_handler.process_signal(event_signal);

        } else if (event->type == "ORDER") {
            events::OrderEvent event_order = *dynamic_cast<events::OrderEvent *>(event.release());
            // Pass the order event into the execution handler to generate a fill
            execution_handler.process_order(event_order);

        } else if (event->type == "FILL") {
            events::FillEvent event_fill = *dynamic_cast<events::FillEvent *>(event.release());
            // Pass the fill event into the portfolio to update holdings
            portfolio.update_fill(event_fill);

        } else if (event->type == "SCHEDULED") {
            events::ScheduledEvent<Strategy> event_scheduled = *dynamic_cast<events::ScheduledEvent<Strategy>*>(event.release());
            // Run the function referenced to in the schedule event
            event_scheduled.run();
        }
    }

    // Print out performance
    std::string mess = std::string("Backtest finished. Total return: ") + std::to_string(portfolio.current_holdings[portfolio_fields::EQUITY_CURVE] * 100) + "%";
    if (sendStatusMessage) { message(mess); }
    std::cout << mess << std::endl;
}

// Schedules member functions by putting a ScheduledEvent with a reference to the member function and a reference
// to this strategy class on the HEAP event list. Then, the function is called at a specific simulated date.
void Strategy::schedule_function(std::function<void(Strategy*)> func, const DateRules& dateRules, const TimeRules& timeRules) {
    // Get the datetimes at which the functions should be scheduled
    std::vector<BloombergLP::blpapi::Datetime> dates = dateRules.get_date_times(timeRules);
    for (const auto& i : dates) {
        // Put the scheduled function onto the heap with a reference to the function and the strategy object to call it
        auto toInsertBefore = std::find_if(heap_eventlist.begin(), heap_eventlist.end(), first_date_greater(i));
        // If no object is found with a later date, the object is put on the end of the heap list
        heap_eventlist.insert(toInsertBefore, std::make_unique<events::ScheduledEvent<Strategy>>(func, this, i));
    }
}

// Turns on Slack messaging at end of algo run
void Strategy::turnOnSlackPerformanceReporting() { sendStatusMessage = true; }

// Scheduled function check
void Strategy::check() { std::cout << "Function ran on " << current_time << std::endl; }

// MARK: LIVE STRATEGY FUNCTIONALITY
// Constructs a live strategy with the mutex for running it on two threads
LiveStrategy::LiveStrategy(const std::vector<std::string> &p_symbol_list,
                           unsigned int p_initial_capital,
                           const BloombergLP::blpapi::Datetime &p_start_date,
                           const BloombergLP::blpapi::Datetime &p_end_date) :
        BaseStrategy(p_symbol_list, p_initial_capital, p_start_date, p_end_date),
        mtx(PTHREAD_MUTEX_INITIALIZER),
        data(std::make_shared<HistoricalDataManager>(&current_time)),
        execution_handler(&stack_eventqueue, &heap_eventlist, data, &portfolio),
        live_data(std::make_unique<RealTimeDataRetriever>(&mtx)) { }

// Runs the live strategy by simply continually updating the current time, checking if the object in the front of
// the event heap has a datetime less than or equal to the current time, and if so, interpreting that event. Once
// the event is finished, the first event is popped off and the loop continues.
void LiveStrategy::run() {

    // Runs the subscription with Bloomberg realtime data (already in a separate thread)
    live_data->runSubscription(symbol_list);

    // Sets the start date and current time to the current DateTime
    BloombergLP::blpapi::Datetime initial = date_funcs::get_now();
    start_date = initial;
    portfolio.reset_portfolio(initial_capital, initial);

    // The datetime incrementing loop which continuously updates the current time
    for (current_time = initial; date_funcs::is_greater(end_date, current_time); current_time = date_funcs::get_now()) {

        // When there are new market events, put them into the heap
        if (!live_data->buffer_queue.empty()) {
            pthread_mutex_lock(&mtx);

            // Pulls all the data from the queue in the live data feed into the event HEAP
            while (!live_data->buffer_queue.empty()) {
                // Get the first-in market event from the buffer queue
                std::unique_ptr<events::Event> new_event = std::move(live_data->buffer_queue.front());
                live_data->buffer_queue.pop();

                // Put the scheduled function onto the heap with a reference to the function and the strategy object to call it
                auto toInsertBefore = std::find_if(heap_eventlist.begin(), heap_eventlist.end(), first_date_greater(new_event->datetime));
                // If no object is found with a later date, the object is put on the end of the heap list
                heap_eventlist.insert(toInsertBefore, std::move(new_event));
            }

            // Once the buffer queue is empty, unlock the mutex
            pthread_mutex_unlock(&mtx);
        }

        // The event object to process
        std::unique_ptr<events::Event> event = nullptr;
        // Now process the events similarly to how a normal strategy does it. First go through the stack,
        // and then compare the datetimes to see if any events are available to be run
        if (!stack_eventqueue.empty()) {
            // Grab the first event on the stack and remove it from the queue
            event = std::move(stack_eventqueue.front());
            stack_eventqueue.pop();
        } else {
            // If there is not event on the heap, keep updating the time and looking for new MarketEvents
            if (heap_eventlist.empty()) {
                continue;
            } else {
                // Compare the current date time to the date of the event on the front of the HEAP
                if (date_funcs::is_greater(current_time, heap_eventlist.front()->datetime)) {
                    event = std::move(heap_eventlist.front());
                    heap_eventlist.pop_front();
                } else {
                    continue;
                }
            }
        }

        // Now downcast the event and perform whatever function it requires
        if (event->type == "MARKET") {
            events::MarketEvent event_market = *dynamic_cast<events::MarketEvent *>(event.release());
            // Pass the market event into the portfolio to update holdings
            portfolio.update_market(event_market);

        } else if (event->type == "SIGNAL") {
            events::SignalEvent event_signal = *dynamic_cast<events::SignalEvent *>(event.release());
            // Pass the signal event into the execution handler to generate orders
            execution_handler.process_signal(event_signal);

        } else if (event->type == "ORDER") {
            events::OrderEvent event_order = *dynamic_cast<events::OrderEvent *>(event.release());
            // Pass the order event into the execution handler to generate a fill
            execution_handler.process_order(event_order);

        } else if (event->type == "FILL") {
            events::FillEvent event_fill = *dynamic_cast<events::FillEvent *>(event.release());
            // Pass the fill event into the portfolio to update holdings
            portfolio.update_fill(event_fill);

        } else if (event->type == "SCHEDULED") {
            events::ScheduledEvent<LiveStrategy> event_scheduled = *dynamic_cast<events::ScheduledEvent<LiveStrategy>*>(event.release());
            // Run the function referenced to in the schedule event
            event_scheduled.run();
        }
    }
}

// Schedules a function in a LiveStrategy event loop. Is the exact same function as the normal Strategy's.
void LiveStrategy::schedule_function(std::function<void(LiveStrategy *)> func, const DateRules &dateRules,
                                     const TimeRules &timeRules) {
    // Get the datetimes at which the functions should be scheduled
    std::vector<BloombergLP::blpapi::Datetime> dates = dateRules.get_date_times(timeRules);
    for (const auto& i : dates) {
        // Only schedule for dates after the start
        if (date_funcs::is_greater(start_date, i)) { continue; }
        // Put the scheduled function onto the heap with a reference to the function and the strategy object to call it
        auto toInsertBefore = std::find_if(heap_eventlist.begin(), heap_eventlist.end(), first_date_greater(i));
        // If no object is found with a later date, the object is put on the end of the heap list
        heap_eventlist.insert(toInsertBefore, std::make_unique<events::ScheduledEvent<LiveStrategy>>(func, this, i));
    }
}

// Scheduled function check
void LiveStrategy::check() { std::cout << "Function ran on " << current_time << std::endl; }