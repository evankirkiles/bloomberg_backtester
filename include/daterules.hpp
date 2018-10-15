//
// Created by Evan Kirkiles on 10/3/2018.
//

#ifndef BACKTESTER_DATERULES_HPP
#define BACKTESTER_DATERULES_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"
// Include datetime for date offsetting
#include <ctime>
// Custom class includes
#include "constants.hpp"
#include "holidays.hpp"

// Contains the date rules for scheduling functions, almost exactly like how Quantopian does it. Dynamically
// schedules based on date params for every_day, week_open, week_end, month_open, month_end. You
// can then specify the time, either market_open or market_close, given hours and minutes.
//
// Time Rules class which contains the functions for specifying at what times to run the algorithm. For each day
// specified by the algorithm, it is checked against the vector of holidays to see if it matches any, at which it will
// use the time rule for that specific day.
class TimeRules {
public:
    // Important: Minutes must be in range of 0 to 59, and hours must be in range of 0 to 3
    static TimeRules market_open(unsigned int hours = 0, unsigned int minutes = 0);
    static TimeRules market_close(unsigned int hours = 0, unsigned int minutes = 0);

    // Default constructor for timerules
    explicit TimeRules(int type = -1, unsigned int hours = 0, unsigned int minutes = 0);

    // Returns the specified time for a given day as a tuple of <hours, minutes, seconds> by checking
    // against the standard closing times and then against all the holidays. If the mode is weekly and
    // there are no more trading days left in the week, then the function is not scheduled for that week.
    // The datetime returned in that case will be in 1970, which must be checked for and not scheduled.
    BloombergLP::blpapi::Datetime get_time(BloombergLP::blpapi::Datetime date, unsigned int mode) const;

private:
    const int type;
    const unsigned int hours, minutes;
};

// Date Rules class which contains the functions for specifying on which days to run the algorithm. Used
// in conjunction with time_rules to get a list of dates between the start date and end date. Constructor
// builds the date rules, but to receive specific Datetimes needs to be given a time rule.
class DateRules {
public:
    DateRules every_day() const;
    DateRules week_start(int days_offset = 0) const;
    DateRules week_end(int days_offset = 0) const;
    DateRules month_start(int days_offset = 0) const;
    DateRules month_end(int days_offset = 0) const;

    // Builds a date rules object with the start and end date of strategies to enable scheduling. All date rules
    // objects will be derived from this one to preserve start and end date, but this should never be called by user.
    explicit DateRules(const BloombergLP::blpapi::Datetime& start_date, const BloombergLP::blpapi::Datetime& end_date,
            int type = -1, int days_offset = 0);

    // Gets the schedule of dates and times at which the algorithm will be run
    std::vector<BloombergLP::blpapi::Datetime> get_date_times(const TimeRules& time_rules) const;

private:
    const BloombergLP::blpapi::Datetime start_date, end_date;
    const int type, days_offset;
};

// Declare the function which adds a set number of seconds to a datetime object and returns a copy
namespace date_funcs {
    // The current time is the date which will use the offset seconds. When weekDaysOnly is true, the function
    // will continue to add the number of seconds until it is no longer on a weekend. Finally, mode is used to
    // specify different time masks. For example, when we are scheduling every day functions (0), if the time
    // would transfer into another day we simply do not want to schedule a function. When weekly (1 || 2), we do not
    // want the seconds added to exceed the current week. Finally, when monthly (3||4), we do not want the seconds
    // to escape the current month.
    BloombergLP::blpapi::Datetime add_seconds(const BloombergLP::blpapi::Datetime& currentTime, int seconds,
            bool weekDaysOnly = false, int mode = -1);

    // Gets the current time
    BloombergLP::blpapi::Datetime get_now();

    // Compares two dates, returning true if the first is greater
    bool is_greater(const BloombergLP::blpapi::Datetime& first, const BloombergLP::blpapi::Datetime& second);
}


#endif //BACKTESTER_DATERULES_HPP
