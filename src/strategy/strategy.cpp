//
// Created by Evan Kirkiles on 9/27/2018.
//

// Include the corresponding header
#include "strategy.hpp"

// Builds the parent abstract class
BaseStrategy::BaseStrategy(std::vector<std::string> p_symbol_list, unsigned int p_initial_capital,
                           const BloombergLP::blpapi::Datetime &p_start, const BloombergLP::blpapi::Datetime &p_end,
                           const std::string& type) :
            symbol_list(std::move(p_symbol_list)),
            initial_capital(p_initial_capital),
            start_date(p_start),
            current_time(p_start),
            end_date(p_end),
            date_rules(p_start, p_end),
            time_rules(),
            portfolio(p_symbol_list, p_initial_capital, p_start) {}

// Orders stocks up to the target percentage, simply by converting the params into signal events
void BaseStrategy::order_target_percent(const std::string &symbol, double percent) {
    stack_eventqueue.emplace(std::make_unique<events::SignalEvent>(symbol, percent, current_time));
}

// Builds the Strategy object with the given initial capital and start and end. To reformat the strategy,
// probably should just reconstruct it.
Strategy::Strategy(const std::vector<std::string>& p_symbol_list,
                unsigned int p_initial_capital, const BloombergLP::blpapi::Datetime &p_start_date,
                   const BloombergLP::blpapi::Datetime &p_end_date) :
           BaseStrategy(p_symbol_list, p_initial_capital, p_start_date, p_end_date),
            data(std::make_shared<HistoricalDataManager>(&current_time)),
           execution_handler(&stack_eventqueue, &heap_eventlist, data, &portfolio) {

    // Make sure to fill the HEAP event list with the MarketEvents.
    data->fillHistory(symbol_list, start_date, end_date, &heap_eventlist);
}

// Runs the strategy by iterating through the HEAP event list until it is empty
void Strategy::run() {
    // Place the iterator onto the heap eventlist
    currentEvent = heap_eventlist.begin();
    running = true;

    // Use a boolean value to allow for exiting after a loop
    while(running && (currentEvent != heap_eventlist.end() || !stack_eventqueue.empty())) {
        std::unique_ptr<events::Event> event;
        // If the STACK is empty, take in events on the HEAP and process them, otherwise go through
        // the events on the STACK until it is empty. STACk always starts out empty.
        if (!stack_eventqueue.empty()) {
            // Get the first event
            event = std::move(stack_eventqueue.front());
            stack_eventqueue.pop();
        } else {
            // Iterate the currentEvent forwards and retrieve its object
            event = std::move(*currentEvent);
            currentEvent++;
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
            events::ScheduledEvent event_scheduled = *dynamic_cast<events::ScheduledEvent *>(event.release());
            // Run the function referenced to in the schedule event
            event_scheduled.run();
        }
    }
}

// Scheduled function check
void Strategy::check() { std::cout << "Function ran on " << current_time << std::endl; }

// Inherited function which schedules an event onto the event HEAP. It provides a reference to the strategy's function
// and the current class passing it in.
void Strategy::schedule_function(void (Strategy::*func)(), const DateRules& dateRules, const TimeRules& timeRules) {
    // Get the datetimes at which the functions should be scheduled
    std::vector<BloombergLP::blpapi::Datetime> dates = dateRules.get_date_times(timeRules);
    for (const auto& i : dates) {
        // Put the scheduled function onto the heap with a reference to the function and the strategy object to call it
        auto toInsertBefore = std::find_if(heap_eventlist.begin(), heap_eventlist.end(), first_date_greater(i));
        // If no object is found with a later date, the object is put on the end of the heap list
        heap_eventlist.insert(toInsertBefore, std::make_unique<events::ScheduledEvent>(func, this, i));
    }
}


// MARK: SCHEDULED EVENT INITIALIZATION
// Event initialization for ScheduledEvent
namespace events {
// Scheduled Event initializer list
ScheduledEvent::ScheduledEvent(void (Strategy::*p_func)(), Strategy* p_strat,
                               const BloombergLP::blpapi::Datetime &p_when) :
        Event("SCHEDULED", p_when),
        function(p_func),
        instance(p_strat) {}
// Print function for Scheduled Event
void ScheduledEvent::what() {
    std::cout << "Event: SCHEDULED\nDatetime: " << datetime << "\n";
}
// Runs the function specified with a call to invoke
void ScheduledEvent::run() {
    std::invoke(function, instance);
}
}