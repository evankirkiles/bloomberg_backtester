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
BloombergLP::blpapi::Datetime TimeRules::get_time(BloombergLP::blpapi::Datetime date) {
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
                        date = date_funcs::add_seconds(date, 24 * 60 * 60);
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

BloombergLP::blpapi::Datetime add_seconds(const BloombergLP::blpapi::Datetime& currentTime, int seconds, bool weekDaysOnly) {
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
    time_t date_seconds = mktime(&timeinfo) + seconds;
    // Put the updated date back into a Bloomberg::blpapi::Datetime
    timeinfo = *localtime(&date_seconds);

    // If looking for weekdays only continue checking, can only run twice
    while (weekDaysOnly && (timeinfo.tm_wday == 0 || timeinfo.tm_wday == 6)) {
        date_seconds = mktime(&timeinfo) + seconds;
        timeinfo = *localtime(&date_seconds);
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

// Define all holidays here for necessary years
namespace holidays {
    const std::unordered_map<
            unsigned int, std::unordered_map<
                    unsigned int, std::unordered_map<
                            unsigned int, std::string>>> holidays_US = {
            // 2020
            {50, {
                    // January 1, New Year's Day
                    {0, {{1, "CLOSED"},
                            // January 20, Martin Luther King Jr. Day
                         {20, "CLOSED"}}},
                    // February 17, Presidents Day/Washington's Birthday
                    {1, {{17, "CLOSED"}}},
                    // April 10, Good Friday
                    {3, {{10, "CLOSED"}}},
                    // May 25, Memorial Day
                    {4, {{25, "CLOSED"}}},
                    // July 3, Independence Day (observed)
                    {6, {{3, "CLOSED"}}},
                    // September 7, Labor Day
                    {8, {{7, "CLOSED"}}},
                    // November 28, Thanksgiving Day
                    {10, {{26, "CLOSED"},
                            // November 23, Day After Thanksgiving Day
                          {27, "EARLY CLOSE"}}},
                    // December 24, Christmas Eve
                    {11, {{24, "EARLY CLOSE"},
                            // December 25, Christmas Day
                          {25, "CLOSED"}}}
            }
            },
            // 2019
            {49, {
                    // January 1, New Year's Day
                    {0, {{1, "CLOSED"},
                            // January 21, Martin Luther King Jr. Day
                         {21, "CLOSED"}}},
                    // February 18, Presidents Day/Washington's Birthday
                    {1, {{18, "CLOSED"}}},
                    // April 19, Good Friday
                    {3, {{19, "CLOSED"}}},
                    // May 27, Memorial Day
                    {4, {{27, "CLOSED"}}},
                    // July 3, Day Before Independence Day
                    {6, {{3, "EARLY CLOSE"},
                            // July 4, Independence Day
                         {4, "CLOSED"}}},
                    // September 2, Labor Day
                    {8, {{2, "CLOSED"}}},
                    // November 28, Thanksgiving Day
                    {10, {{28, "CLOSED"},
                            // November 23, Day After Thanksgiving Day
                          {29, "EARLY CLOSE"}}},
                    // December 24, Christmas Eve
                    {11, {{24, "EARLY CLOSE"},
                            // December 25, Christmas Day
                          {25, "CLOSED"}}}
            }
            },
            // 2018
            {48, {
                        // January 1, New Year's Day
                        {0, {{1, "CLOSED"},
                                    // January 15, Martin Luther King Jr. Day
                                    {15, "CLOSED"}}},
                        // February 19, Presidents Day/Washington's Birthday
                        {1, {{19, "CLOSED"}}},
                        // March 30, Good Friday
                        {2, {{30, "CLOSED"}}},
                        // May 28, Memorial Day
                        {4, {{28, "CLOSED"}}},
                        // July 3, Day Before Independence Day
                        {6, {{3, "EARLY CLOSE"},
                                    // July 4, Independence Day
                                    {4, "CLOSED"}}},
                        // September 3, Labor Day
                        {8, {{3, "CLOSED"}}},
                        // November 22, Thanksgiving Day
                        {10, {{22, "CLOSED"},
                                    // November 23, Day After Thanksgiving Day
                                    {23, "EARLY CLOSE"}}},
                        // December 24, Christmas Eve
                        {11, {{24, "EARLY CLOSE"},
                                    // December 25, Christmas Day
                                    {25, "CLOSED"}}}
                }
            },
            // 2017
            {47, {
                         // January 2, New Year's Day
                         {0, {{2, "CLOSED"},
                                     // January 15, Martin Luther King Jr. Day
                                     {16, "CLOSED"}}},
                         // February 20, Presidents Day/Washington's Birthday
                         {1, {{20, "CLOSED"}}},
                         // April 14, Good Friday
                         {3, {{14, "CLOSED"}}},
                         // May 29, Memorial Day
                         {4, {{29, "CLOSED"}}},
                         // July 3, Day Before Independence Day
                         {6, {{3, "EARLY CLOSE"},
                                     // July 4, Independence Day
                                     {4, "CLOSED"}}},
                         // September 4, Labor Day
                         {8, {{4, "CLOSED"}}},
                         // November 23, Thanksgiving Day
                         {10, {{23, "CLOSED"},
                                      // November 24, Day After Thanksgiving Day
                                      {24, "EARLY CLOSE"}}},
                         // December 25, Christmas Day
                         {11, {{25, "CLOSED"}}}
                 }
            },
            // 2016
            {46, {
                         // January 1, New Year's Day
                         {0, {{1, "CLOSED"},
                                     // January 18, Martin Luther King Jr. Day
                                     {18, "CLOSED"}}},
                         // February 15, Presidents Day/Washington's Birthday
                         {1, {{15, "CLOSED"}}},
                         // March 25, Good Friday
                         {2, {{25, "CLOSED"}}},
                         // May 30, Memorial Day
                         {4, {{30, "CLOSED"}}},
                         // July 4, Independence Day
                         {6, {{4, "CLOSED"}}},
                         // September 5, Labor Day
                         {8, {{5, "CLOSED"}}},
                         // November 24, Thanksgiving Day
                         {10, {{24, "CLOSED"},
                                      // November 25, Day After Thanksgiving Day
                                      {25, "EARLY CLOSE"}}},
                         // December 26, Christmas Day
                         {11, {{26, "CLOSED"}}}
                 }
            },
            // 2015
            {45, {
                         // January 1, New Year's Day
                         {0, {{1, "CLOSED"},
                                     // January 19, Martin Luther King Jr. Day
                                     {19, "CLOSED"}}},
                         // February 16, Presidents Day/Washington's Birthday
                         {1, {{16, "CLOSED"}}},
                         // April 3, Good Friday
                         {3, {{3, "CLOSED"}}},
                         // May 25, Memorial Day
                         {4, {{25, "CLOSED"}}},
                         // July 3, Day Before Independence Day
                         {6, {{3, "CLOSED"},
                                     // July 4, Independence Day
                                     {4, "CLOSED"}}},
                         // September 7, Labor Day
                         {8, {{7, "CLOSED"}}},
                         // November 26, Thanksgiving Day
                         {10, {{26, "CLOSED"},
                                      // November 27, Day After Thanksgiving Day
                                      {27, "EARLY CLOSE"}}},
                         // December 24, Christmas Eve
                         {11, {{24, "EARLY CLOSE"},
                                      // December 25, Christmas Day
                                      {25, "CLOSED"}}}
                 }
            },
            // 2014
            {44, {
                         // January 1, New Year's Day
                         {0, {{1, "CLOSED"},
                                     // January 20, Martin Luther King Jr. Day
                                     {20, "CLOSED"}}},
                         // February 17, Presidents Day/Washington's Birthday
                         {1, {{17, "CLOSED"}}},
                         // April 18, Good Friday
                         {3, {{18, "CLOSED"}}},
                         // May 26, Memorial Day
                         {4, {{26, "CLOSED"}}},
                         // July 3, Day Before Independence Day
                         {6, {{3, "EARLY CLOSE"},
                                     // July 4, Independence Day
                                     {4, "CLOSED"}}},
                         // September 1, Labor Day
                         {8, {{1, "CLOSED"}}},
                         // November 27, Thanksgiving Day
                         {10, {{27, "CLOSED"},
                                      // November 28, Day After Thanksgiving Day
                                      {28, "EARLY CLOSE"}}},
                         // December 24, Christmas Eve
                         {11, {{24, "EARLY CLOSE"},
                                      // December 25, Christmas Day
                                      {25, "CLOSED"}}}
                 }
            },
            // 2013
            {43, {
                         // January 1, New Year's Day
                         {0, {{1, "CLOSED"},
                                     // January 21, Martin Luther King Jr. Day
                                     {21, "CLOSED"}}},
                         // February 18, Presidents Day/Washington's Birthday
                         {1, {{18, "CLOSED"}}},
                         // March 29, Good Friday
                         {2, {{29, "CLOSED"}}},
                         // May 27, Memorial Day
                         {4, {{27, "CLOSED"}}},
                         // July 3, Day Before Independence Day
                         {6, {{3, "EARLY CLOSE"},
                                     // July 4, Independence Day
                                     {4, "CLOSED"}}},
                         // September 2, Labor Day
                         {8, {{2, "CLOSED"}}},
                         // November 28, Thanksgiving Day
                         {10, {{28, "CLOSED"},
                                      // November 29, Day After Thanksgiving Day
                                      {29, "EARLY CLOSE"}}},
                         // December 24, Christmas Eve
                         {11, {{24, "EARLY CLOSE"},
                                      // December 25, Christmas Day
                                      {25, "CLOSED"}}}
                 }
            },
            // 2012
            {42, {
                         // January 2, New Year's Day
                         {0, {{2, "CLOSED"},
                                     // January 16, Martin Luther King Jr. Day
                                     {16, "CLOSED"}}},
                         // February 20, Presidents Day/Washington's Birthday
                         {1, {{20, "CLOSED"}}},
                         // April 6, Good Friday
                         {3, {{6, "CLOSED"}}},
                         // May 28, Memorial Day
                         {4, {{28, "CLOSED"}}},
                         // July 3, Day Before Independence Day
                         {6, {{3, "EARLY CLOSE"},
                                     // July 4, Independence Day
                                     {4, "CLOSED"}}},
                         // September 3, Labor Day
                         {8, {{3, "CLOSED"}}},
                         // November 22, Thanksgiving Day
                         {10, {{22, "CLOSED"},
                                      // November 23, Day After Thanksgiving Day
                                      {23, "EARLY CLOSE"}}},
                         // December 24, Christmas Eve
                         {11, {{24, "EARLY CLOSE"},
                                      // December 25, Christmas Day
                                      {25, "CLOSED"}}}
                 }
            },
            // 2011
            {41, {
                         // January 17, Martin Luther King Jr. Day
                         {0, {{17, "CLOSED"}}},
                         // February 21, Presidents Day/Washington's Birthday
                         {1, {{21, "CLOSED"}}},
                         // April 22, Good Friday
                         {3, {{22, "CLOSED"}}},
                         // May 30, Memorial Day
                         {4, {{30, "CLOSED"}}},
                         // July 3, Day Before Independence Day
                         {6, {{3, "EARLY CLOSE"},
                                     // July 4, Independence Day
                                     {4, "CLOSED"}}},
                         // September 5, Labor Day
                         {8, {{5, "CLOSED"}}},
                         // November 24, Thanksgiving Day
                         {10, {{24, "CLOSED"},
                                      // November 25, Day After Thanksgiving Day
                                      {25, "EARLY CLOSE"}}},
                         // December 25, Christmas Eve
                         {11, {{25, "EARLY CLOSE"},
                                      // December 26, Christmas Day
                                      {26, "CLOSED"}}}
                 }
            },
            // 2010
            {40, {
                    // January 2, New Year's Day
                    {0, {{1, "CLOSED"},
                            // January 16, Martin Luther King Jr. Day
                         {18, "CLOSED"}}},
                    // February 15, Presidents Day/Washington's Birthday
                    {1, {{15, "CLOSED"}}},
                    // April 2, Good Friday
                    {3, {{2, "CLOSED"}}},
                    // May 31, Memorial Day
                    {4, {{31, "CLOSED"}}},
                    // July 4, Independence Day
                    {6, {{4, "EARLY CLOSE"},
                            // July 5, Independence Day (observed)
                         {5, "CLOSED"}}},
                    // September 6, Labor Day
                    {8, {{6, "CLOSED"}}},
                    // November 25, Thanksgiving Day
                    {10, {{25, "CLOSED"},
                            // November 26, Day After Thanksgiving Day
                          {26, "EARLY CLOSE"}}},
                    // December 24, Christmas Day (observed)
                    {11, {{24, "CLOSED"}}}
                }
            },
    };
}