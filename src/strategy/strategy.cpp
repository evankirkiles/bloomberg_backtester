#include <utility>

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
            time_rules() {}

// Builds the Strategy object with the given initial capital and start and end. To reformat the strategy,
// probably should just reconstruct it.
Strategy::Strategy(const std::vector<std::string>& p_symbol_list,
                unsigned int p_initial_capital, const BloombergLP::blpapi::Datetime &p_start_date,
                   const BloombergLP::blpapi::Datetime &p_end_date) :
           BaseStrategy(p_symbol_list, p_initial_capital, p_start_date, p_end_date),
            data(std::make_unique<HistoricalDataManager>(&current_time)) {

    // Make sure to fill the HEAP event list with the MarketEvents.
    data->fillHistory(symbol_list, start_date, end_date, &heap_eventlist);
}

// Runs the strategy by iterating through the HEAP event list until it is empty
void Strategy::run() {  }

// Scheduled function check
void Strategy::check() { std::cout << "CHECKED" << std::endl; }

// Inherited function which schedules an event onto the event HEAP. It provides a reference to the strategy's function
// and the current class passing it in.
void Strategy::schedule_function(void *func, const DateRules& dateRules, const TimeRules& timeRules) {
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
ScheduledEvent::ScheduledEvent(void *p_function, Strategy* p_strat,
                               const BloombergLP::blpapi::Datetime &p_when) :
        Event("SCHEDULED", p_when),
        function(p_function),
        instance(p_strat) {}
// Print function for Scheduled Event
void ScheduledEvent::what() {
    std::cout << "Event: SCHEDULED\nDatetime: " << datetime << "\n";
}
}