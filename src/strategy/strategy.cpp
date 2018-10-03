//
// Created by Evan Kirkiles on 9/27/2018.
//

// Include the corresponding header
#include "strategy.hpp"

// Inherited function which schedules an event onto the event HEAP. It provides a reference to the strategy's function
// and the current class passing it in.
void BaseStrategy::schedule_function(void *func) {

}

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