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
BloombergLP::blpapi::Datetime TimeRules::get_time(BloombergLP::blpapi::Datetime date, unsigned int mode) {
    bool noHoliday = false;
    bool earlyClose = false;
    int tries = 0;
    while(!noHoliday && tries < 10) {
        // First check if there is actually an element for the given date in the holidays
        if (time_holidays::holidays_US.find(date.year() - 1900) !=
            time_holidays::holidays_US.end()) {
            if (time_holidays::holidays_US.at(date.year() - 1900).find(date.month() - 1) !=
                time_holidays::holidays_US.at(date.year() - 1900).end()) {
                if (time_holidays::holidays_US.at(date.year() - 1900).at(date.month() - 1).find(date.day()) !=
                    time_holidays::holidays_US.at(date.year() - 1900).at(date.month() - 1).end()) {
                    // If the market is closed on that day, increment the date by 1 day forwards
                    if (time_holidays::holidays_US.at(date.year() - 1900).at(date.month() - 1).at(date.day()) ==
                        "CLOSED") {
                        date = date_funcs::add_seconds(date, 24 * 60 * 60, true);

                        tries++;
                    } else if (time_holidays::holidays_US.at(date.year() - 1900).at(date.month() - 1).at(date.day()) ==
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
            date.setHours(date_time_enums::US_MARKET_OPEN_HOUR + 1);
            date.setMinutes(date_time_enums::US_MARKET_OPEN_MINUTE + minutes - 60);
        } else {
            date.setHours(date_time_enums::US_MARKET_OPEN_HOUR);
            date.setMinutes(date_time_enums::US_MARKET_OPEN_MINUTE + minutes);
        }
        date.setSeconds(0);
        return date;
    // Market close type needs some slight reformatting in case of early close
    } else {
        if (earlyClose) {
            if (date_time_enums::US_MARKET_EARLY_CLOSE_MINUTE - minutes < 0) {
                date.setHours(date_time_enums::US_MARKET_EARLY_CLOSE_HOUR - 1);
                date.setMinutes(date_time_enums::US_MARKET_EARLY_CLOSE_MINUTE - minutes + 60);
            } else {
                date.setHours(date_time_enums::US_MARKET_EARLY_CLOSE_HOUR);
                date.setMinutes(date_time_enums::US_MARKET_EARLY_CLOSE_MINUTE - minutes);
            }
            date.setSeconds(0);
            return date;
        } else {
            if (date_time_enums::US_MARKET_CLOSE_MINUTE - minutes < 0) {
                date.setHours(date_time_enums::US_MARKET_CLOSE_HOUR - 1);
                date.setMinutes(date_time_enums::US_MARKET_CLOSE_MINUTE - minutes + 60);
            } else {
                date.setHours(date_time_enums::US_MARKET_CLOSE_HOUR);
                date.setMinutes(date_time_enums::US_MARKET_CLOSE_MINUTE - minutes);
            }
            date.setSeconds(0);
            return date;
        }
    }
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
            // to get the range of ydays for the week.
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
                        timeinfo.tm_yday > initialtimeinfo.tm_yday - initialtimeinfo.tm_wday) {
                return BloombergLP::blpapi::Datetime(1970, 1, 1, 0, 0, 0, 0);
            }
            break;
        }
        case 3:
        case 4: {

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