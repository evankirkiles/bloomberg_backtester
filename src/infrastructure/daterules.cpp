//
// Created by Evan Kirkiles on 10/3/2018.
//

// Include corresponding header
#include "daterules.hpp"

// Initialize a TimeRules instance from which time rules can be built
TimeRules::TimeRules(int p_type, unsigned int p_hours, unsigned int p_minutes) :
    type(p_type), hours(p_hours), minutes(p_minutes) {}

// Builds and returns TimeRules instances for each type of timerule
TimeRules TimeRules::market_open(unsigned int hours, unsigned int minutes) {
    if (hours < 0 || hours > 3) { throw std::runtime_error("Invalid hours arg!"); }
    if (minutes < 0 || minutes > 59) { throw std::runtime_error("Invalid minutes arg!"); }
    return TimeRules(date_time_enums::T_MARKET_OPEN, hours, minutes); }
TimeRules TimeRules::market_close(unsigned int hours, unsigned int minutes) {
    if (hours < 0 || hours > 3) { throw std::runtime_error("Invalid hours arg!"); }
    if (minutes < 0 || minutes > 59) { throw std::runtime_error("Invalid minutes arg!"); }
    return TimeRules(date_time_enums::T_MARKET_CLOSE, hours, minutes); }

// Returns the hour, minute, and second of the time for a given date based on the holidays
BloombergLP::blpapi::Datetime TimeRules::get_time(BloombergLP::blpapi::Datetime date, unsigned int mode) const {
    bool noHoliday = false;
    bool earlyClose = false;
    int tries = 0;
    while(!noHoliday && tries < 10) {
        // First check if there is actually an element for the given date in the holidays
        if (time_holidays::holidays_US.find(date.year() - 1970) !=
            time_holidays::holidays_US.end()) {
            if (time_holidays::holidays_US.at(date.year() - 1970).find(date.month() - 1) !=
                time_holidays::holidays_US.at(date.year() - 1970).end()) {
                if (time_holidays::holidays_US.at(date.year() - 1970).at(date.month() - 1).find(date.day()) !=
                    time_holidays::holidays_US.at(date.year() - 1970).at(date.month() - 1).end()) {
                    // If the market is closed on that day, increment the date by 1 day forwards
                    if (time_holidays::holidays_US.at(date.year() - 1970).at(date.month() - 1).at(date.day()) ==
                        "CLOSED") {
                        date = date_funcs::add_seconds(date, 24 * 60 * 60, true, mode);
                        if (date.year() == 1970) { return date; }

                        tries++;
                    } else if (time_holidays::holidays_US.at(date.year() - 1970).at(date.month() - 1).at(date.day()) ==
                               "EARLY CLOSE") {
                        earlyClose = true;
                        noHoliday = true;
                    } else { noHoliday = true; }
                } else { noHoliday = true; }
            } else { noHoliday = true; }
        } else { noHoliday = true; }
    }

    // Market open type will always be the same open time
    if (type == date_time_enums::T_MARKET_OPEN) {
        if (date_time_enums::US_MARKET_OPEN_MINUTE + minutes >= 60) {
            date.setHours(date_time_enums::US_MARKET_OPEN_HOUR + 1 + hours);
            date.setMinutes(date_time_enums::US_MARKET_OPEN_MINUTE - 60 + minutes);
        } else {
            date.setHours(date_time_enums::US_MARKET_OPEN_HOUR + hours);
            date.setMinutes(date_time_enums::US_MARKET_OPEN_MINUTE + minutes);
        }
        date.setSeconds(0);
        return date;
    // Market close type needs some slight reformatting in case of early close
    } else {
        if (earlyClose) {
            if (date_time_enums::US_MARKET_EARLY_CLOSE_MINUTE - minutes < 0) {
                date.setHours(date_time_enums::US_MARKET_EARLY_CLOSE_HOUR - 1 - hours);
                date.setMinutes(date_time_enums::US_MARKET_EARLY_CLOSE_MINUTE + 60 - minutes);
            } else {
                date.setHours(date_time_enums::US_MARKET_EARLY_CLOSE_HOUR - hours);
                date.setMinutes(date_time_enums::US_MARKET_EARLY_CLOSE_MINUTE - minutes);
            }
            date.setSeconds(0);
            return date;
        } else {
            if (date_time_enums::US_MARKET_CLOSE_MINUTE - minutes < 0) {
                date.setHours(date_time_enums::US_MARKET_CLOSE_HOUR - 1 - hours);
                date.setMinutes(date_time_enums::US_MARKET_CLOSE_MINUTE + 60 - minutes);
            } else {
                date.setHours(date_time_enums::US_MARKET_CLOSE_HOUR - hours);
                date.setMinutes(date_time_enums::US_MARKET_CLOSE_MINUTE - minutes);
            }
            date.setSeconds(0);
            return date;
        }
    }
}

// Initializes a Date Rules instance to be used for scheduling algorithms at specific dates relative to market times.
DateRules::DateRules(const BloombergLP::blpapi::Datetime &p_start_date, const BloombergLP::blpapi::Datetime &p_end_date,
                     int p_type, int p_days_offset) :
         start_date(p_start_date), end_date(p_end_date), days_offset(p_days_offset), type(p_type) {}
// Different types of DateRules retrievers which simply change the type, for easier user use in algorithm
DateRules DateRules::every_day() const { return DateRules(start_date, end_date, 0); }
DateRules DateRules::week_start(int days_offset) const { return DateRules(start_date, end_date, 1, days_offset); }
DateRules DateRules::week_end(int days_offset) const { return DateRules(start_date, end_date, 2, days_offset); }
DateRules DateRules::month_start(int days_offset) const { return DateRules(start_date, end_date, 3, days_offset); }
DateRules DateRules::month_end(int days_offset) const { return DateRules(start_date, end_date, 4, days_offset); }

// Returns a vector of datetimes at which to schedule the function calls.
std::vector<BloombergLP::blpapi::Datetime> DateRules::get_date_times(const TimeRules &time_rules) const {
    // First, get the timeinfos for the start and end dates
    struct tm start_time = {0, 0, 0};
    start_time.tm_year = start_date.year() - 1900;
    start_time.tm_mon = start_date.month() - 1;
    start_time.tm_mday = start_date.day();
    time_t starttimet = mktime(&start_time);
    struct tm end_time = {0, 0, 0};
    end_time.tm_year = end_date.year() - 1900;
    end_time.tm_mon = end_date.month() - 1;
    end_time.tm_mday = end_date.day();
    time_t endtimet = mktime(&end_time);
    // Temporary vector to return eventually
    std::vector<BloombergLP::blpapi::Datetime> temp;

    // Now iterate through the days between start and end, adding a checked date depending on the type
    switch (type) {
        case 0: {
            // Iterate through each date, incremented by day
            while (starttimet < endtimet) {
                // Make sure the datetime is a weekday, so functions are not run on weekends
                struct tm tempTime = *localtime(&starttimet);
                if (tempTime.tm_wday > 0 && tempTime.tm_wday < 6) {
                    BloombergLP::blpapi::Datetime dateToPush = time_rules.get_time(
                            BloombergLP::blpapi::Datetime(
                                    static_cast<unsigned int>(tempTime.tm_year + 1900),
                                    static_cast<unsigned int>(tempTime.tm_mon + 1),
                                    static_cast<unsigned int>(tempTime.tm_mday),
                                    0, 0, 0), (unsigned int) type);

                    // If the date is not bad, then put it into the vector
                    if (dateToPush.year() != 1970) {
                        temp.emplace_back(dateToPush);
                    }
                }

                starttimet += 24 * 60 * 60;
            }
            break;
        }
        case 1: {
            // Iterate through each date, incremented by day
            while (starttimet < endtimet) {
                // Make sure the datetime is the right offset date
                struct tm tempTime = *localtime(&starttimet);
                if (tempTime.tm_wday == 1 + days_offset) {
                    BloombergLP::blpapi::Datetime dateToPush = time_rules.get_time(
                            BloombergLP::blpapi::Datetime(
                                    static_cast<unsigned int>(tempTime.tm_year + 1900),
                                    static_cast<unsigned int>(tempTime.tm_mon + 1),
                                    static_cast<unsigned int>(tempTime.tm_mday),
                                    0, 0, 0), (unsigned int) type);

                    // If the date is not bad, then put it into the vector
                    if (dateToPush.year() != 1970) {
                        temp.emplace_back(dateToPush);
                    }
                }

                starttimet += 24 * 60 * 60;
            }
            break;
        }
        case 2: {
            // Iterate through each date, incremented by day
            while (starttimet < endtimet) {
                // Make sure the datetime is on the right day of the week
                struct tm tempTime = *localtime(&starttimet);
                 if (tempTime.tm_wday == 5 - days_offset) {
                    BloombergLP::blpapi::Datetime dateToPush = time_rules.get_time(
                            BloombergLP::blpapi::Datetime(
                                    static_cast<unsigned int>(tempTime.tm_year + 1900),
                                    static_cast<unsigned int>(tempTime.tm_mon + 1),
                                    static_cast<unsigned int>(tempTime.tm_mday),
                                    0, 0, 0), (unsigned int) type);

                    // If the date is not bad, then put it into the vector
                    if (dateToPush.year() != 1970) {
                        temp.emplace_back(dateToPush);
                    }
                }

                starttimet += 24 * 60 * 60;
            }
            break;
        }
        case 3: {
            // Iterate through each date, incremented by day
            while (starttimet < endtimet) {
                // Make sure the datetime is on the right day of the week
                struct tm tempTime = *localtime(&starttimet);
                if (tempTime.tm_mday == days_offset) {
                    BloombergLP::blpapi::Datetime dateToPush = time_rules.get_time(
                            BloombergLP::blpapi::Datetime(
                                    static_cast<unsigned int>(tempTime.tm_year + 1900),
                                    static_cast<unsigned int>(tempTime.tm_mon + 1),
                                    static_cast<unsigned int>(tempTime.tm_mday),
                                    0, 0, 0), (unsigned int) type);

                    // If the date is not bad, then put it into the vector
                    if (dateToPush.year() != 1970) {
                        temp.emplace_back(dateToPush);
                    }
                }

                starttimet += 24 * 60 * 60;
            }
            break;
        }
        case 4: {
            // Iterate through each date, incremented by day
            while (starttimet < endtimet) {
                // Make sure the datetime is on the right day of the week
                struct tm tempTime = *localtime(&starttimet);

                // To get days from end of month, need to specify for certain dates
                int daysInMonth = 31;
                switch (tempTime.tm_mon) {
                    case 1:
                        daysInMonth = 28;
                        break;
                    case 3:
                    case 5:
                    case 8:
                    case 10:
                        daysInMonth = 30;
                        break;
                    default: break;
                }

                if (tempTime.tm_mday == daysInMonth - days_offset) {
                    BloombergLP::blpapi::Datetime dateToPush = time_rules.get_time(
                            BloombergLP::blpapi::Datetime(
                                    static_cast<unsigned int>(tempTime.tm_year + 1900),
                                    static_cast<unsigned int>(tempTime.tm_mon + 1),
                                    static_cast<unsigned int>(tempTime.tm_mday),
                                    0, 0, 0), (unsigned int) type);

                    // If the date is not bad, then put it into the vector
                    if (dateToPush.year() != 1970) {
                        temp.emplace_back(dateToPush);
                    }
                }

                starttimet += 24 * 60 * 60;
            }
            break;
        }
        default: break;
    }

    // Return the vector of dates
    return temp;
}

// Functions to add time to a date
namespace date_funcs {

BloombergLP::blpapi::Datetime add_seconds(const BloombergLP::blpapi::Datetime& currentTime,
        int seconds, bool weekDaysOnly, int mode) {
    struct tm timeinfo = {0, 0, 0};
    timeinfo.tm_year = currentTime.year() - 1900;
    timeinfo.tm_mon = currentTime.month() - 1;
    timeinfo.tm_mday = currentTime.day();
    // If these parts are set, use them as well
    if (currentTime.hasParts(BLPAPI_DATETIME_HOURS_PART & BLPAPI_DATETIME_MINUTES_PART & BLPAPI_DATETIME_SECONDS_PART)) {
        timeinfo.tm_hour = currentTime.hours();
        timeinfo.tm_min = currentTime.minutes();
        timeinfo.tm_sec = currentTime.seconds();
    }
    // Convert to secs since epoch and go back the specified number of days
    time_t initial_date = mktime(&timeinfo);
    time_t date_seconds = mktime(&timeinfo) + seconds;
    // Put the updated date back into a Bloomberg::blpapi::Datetime
    timeinfo = *localtime(&date_seconds);
    struct tm initialtimeinfo = *localtime(&initial_date);

    // If looking for weekdays only continue checking, should only run twice
    while (weekDaysOnly && (timeinfo.tm_wday == 0 || timeinfo.tm_wday == 6)) {
        date_seconds = mktime(&timeinfo) + seconds;
        timeinfo = *localtime(&date_seconds);
    }

    // Compare the initial time and the updated time against the mode
    switch (mode) {
        case 0: {
            // In case of an every day mode, do not return dates not in the same days (1970 dates will be caught).
            // It is assumed that the seconds added do not exceed a full month, because they are not set by user.
            if (initialtimeinfo.tm_mday != timeinfo.tm_mday) {
                return BloombergLP::blpapi::Datetime(1970, 1, 1, 0, 0, 0);
            }
            break;
        }
        case 1:
        case 2: {
            // In case of a weekly mode, do not return dates not in the same week (1970 dates will be caught).
            // When tm_wday is 0, the day is Sunday, so week begins when tm_wday is 1. We can then use tm_yday
            // to get the range of ydays for the week. Weekends do not matter because they are already handled
            // with the weekday only boolean parameter.
            bool weeklyrangeLowerModifier = (initialtimeinfo.tm_yday - initialtimeinfo.tm_wday) < 0;
            bool weeklyrangeUpperModifier = (initialtimeinfo.tm_yday + 6 - initialtimeinfo.tm_wday) > 365;
            if (weeklyrangeLowerModifier &&
                !(timeinfo.tm_yday > 365 + (initialtimeinfo.tm_yday - initialtimeinfo.tm_wday) ||
                  (timeinfo.tm_yday > 0 &&
                   timeinfo.tm_yday < initialtimeinfo.tm_yday + 6 - initialtimeinfo.tm_wday))) {
                return BloombergLP::blpapi::Datetime(1970, 1, 1, 0, 0, 0, 0);
            } else if (weeklyrangeUpperModifier &&
                !(timeinfo.tm_yday < (initialtimeinfo.tm_yday + 6 - initialtimeinfo.tm_wday) - 365 ||
                      (timeinfo.tm_yday < 365 &&
                       timeinfo.tm_yday > initialtimeinfo.tm_yday - initialtimeinfo.tm_wday))) {
                return BloombergLP::blpapi::Datetime(1970, 1, 1, 0, 0, 0, 0);
            } else if (!(timeinfo.tm_yday < initialtimeinfo.tm_yday + 6 - initialtimeinfo.tm_wday &&
                        timeinfo.tm_yday > initialtimeinfo.tm_yday - initialtimeinfo.tm_wday)) {
                return BloombergLP::blpapi::Datetime(1970, 1, 1, 0, 0, 0, 0);
            }
            break;
        }
        case 3:
        case 4: {
            // In case of monthly mode, do not return dates not in the same month (1970 dates will be caught). To do this,
            // we can simply compare the year and month of the timeinfos. Much easier than the weeks.
            if (initialtimeinfo.tm_year != timeinfo.tm_year) {
                return BloombergLP::blpapi::Datetime(1970, 1, 1, 0, 0, 0, 0);
            } else {
                if (initialtimeinfo.tm_mon != timeinfo.tm_mon) {
                    return BloombergLP::blpapi::Datetime(1970, 1, 1, 0, 0, 0, 0);
                }
            }
            break;
        }
        default:
            break;
    }

    return BloombergLP::blpapi::Datetime(
            static_cast<unsigned int>(timeinfo.tm_year) + 1900,
            static_cast<unsigned int>(timeinfo.tm_mon + 1),
            static_cast<unsigned int>(timeinfo.tm_mday),
            static_cast<unsigned int>(timeinfo.tm_hour),
            static_cast<unsigned int>(timeinfo.tm_min),
            static_cast<unsigned int>(timeinfo.tm_sec), 0);
}

}