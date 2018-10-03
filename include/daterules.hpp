//
// Created by Evan Kirkiles on 10/3/2018.
//

#ifndef BACKTESTER_DATERULES_HPP
#define BACKTESTER_DATERULES_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"
// Custom class includes
#include "constants.hpp"


// Contains the date rules for scheduling functions, almost exactly like how Quantopian does it. Dynamically
// schedules based on date params for every_day, week_open, week_end, month_open, month_end. You
// can then specify the time, either market_open or market_close, given hours and minutes.
//
// Time Rules class which contains the functions for specifying at what times to run the algorithm. For each day
// specified by the algorithm, it is checked against the vector of holidays to see if it matches any, at which it will
// use the time rule for that specific day.
class TimeRules {
public:
    static TimeRules market_open(unsigned int hours = 0, unsigned int minutes = 0);
    static TimeRules market_close(unsigned int hours = 0, unsigned int minutes = 0);

    // Returns the specified time for a given day as a tuple of <hours, minutes, seconds> by checking
    // against the standard closing times and then against all the holidays
    std::tuple<int, int, int> get_time(BloombergLP::blpapi::Datetime date);

private:
    const int type;
    const unsigned int hours, minutes;
    // This is private just for consistency so passing a timerules into schedule function is consistent like daterules
    explicit TimeRules(int type = -1, unsigned int hours = 0, unsigned int minutes = 0);
};

// Date Rules class which contains the functions for specifying on which days to run the algorithm. Used
// in conjunction with time_rules to get a list of dates between the start date and end date. Constructor
// builds the date rules, but to receive specific Datetimes needs to be given a time rule.
class DateRules {
public:
    DateRules every_day();
    DateRules week_open(int days_offset = 0);
    DateRules week_end(int days_offset = 0);
    DateRules month_open(int days_offset = 0);
    DateRules month_end(int days_offset = 0);

    // Builds a date rules object with the start and end date of strategies to enable scheduling. All date rules
    // objects will be derived from this one to preserve start and end date, but this should never be called by user.
    explicit DateRules(const BloombergLP::blpapi::Datetime& start_date, const BloombergLP::blpapi::Datetime& end_date,
            int type = -1, int days_offset = 0);

    // Gets the schedule of dates and times at which the algorithm will be run
    std::vector<BloombergLP::blpapi::Datetime> getDateTimes(const TimeRules& time_rules);

private:
    const int type;
    const unsigned int days_offset;
};

// Declare the function which adds a set number of seconds to a datetime object and returns a copy
namespace date_funcs {

}

// Declare all market holidays in this map for each year. (Inefficient, I know, but I don't know any way around this).
namespace time_holidays {
    // Access like: holidays[${YEARS SINCE 1900}][${MONTH - 1}][${DAY OF MONTH}]
    extern const std::unordered_map<
            unsigned int, std::unordered_map<
            unsigned int, std::unordered_map<
            unsigned int, std::string>>> holidays_US;
}

#endif //BACKTESTER_DATERULES_HPP
