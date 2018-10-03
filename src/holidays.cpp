//
// Created by Evan Kirkiles on 10/3/2018.
//

// Include correlated header
#include "holidays.hpp"

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