//
// Created by Evan Kirkiles on 9/27/2018.
//

// Include the corresponding header
#include "strategy.hpp"

// Inherited function which schedules an event onto the event HEAP. It provides a reference to the strategy's function
// and the current class passing it in.
void Strategy::schedule_function(void *func, const DateRules& dateRules, const TimeRules& timeRules) {
    // Get the datetimes at which the functions should be scheduled
    std::vector<BloombergLP::blpapi::Datetime> dates = dateRules.get_date_times(timeRules);
    for (const auto& i : dates) {
        // Put the scheduled function onto the heap with a reference to the function and the strategy object to call it
        auto toInsertBefore = std::find_if(heap_eventlist.begin(), heap_eventlist.end(), first_date_greater(i));
        heap_eventlist.insert(toInsertBefore, std::make_unique<events::ScheduledEvent>(func, this, i));
    }
}


// MARK: SCHEDULED EVENT INITIALIZATION
// Event initialization for ScheduledEvent
namespace events {
// Scheduled Event initializer list
ScheduledEvent::ScheduledEvent(void *p_function, Strategy &p_strat,
                               const BloombergLP::blpapi::Datetime &p_when) :
        Event("SCHEDULED", p_when),
        function(p_function),
        instance(p_strat) {}
// Print function for Scheduled Event
void ScheduledEvent::what() {
    std::cout << "Event: SCHEDULED\nDatetime: " << datetime << "\n";
}
}