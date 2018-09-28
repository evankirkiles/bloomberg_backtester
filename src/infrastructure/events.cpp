//
// Created by Evan Kirkiles on 9/27/2018.
//

// Include corresponding header
#include "events.hpp"

// This file simply contains all the initializer lists for each Event object

// Scheduled Event initializer list
events::ScheduledEvent::ScheduledEvent(void *p_function, Strategy &p_strat, const BloombergLP::blpapi::Datetime &p_when) :
    datetime(p_when), type("SCHEDULED"), function(p_function), instance(p_strat)  {}