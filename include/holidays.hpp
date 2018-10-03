//
// Created by Evan Kirkiles on 10/3/2018.
//

#ifndef BACKTESTER_HOLIDAYS_HPP
#define BACKTESTER_HOLIDAYS_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"

// Class containing the implementations of all the holidays on the US stock exchange for every year I do.

// Declare all market holidays in this map for each year. (Inefficient, I know, but I don't know any way around this).
namespace time_holidays {
    // Access like: holidays[${YEARS SINCE 1900}][${MONTH - 1}][${DAY OF MONTH}]
    extern const std::unordered_map<
            unsigned int, std::unordered_map<
                    unsigned int, std::unordered_map<
                                  unsigned int, std::string>>> holidays_US;
}

#endif //BACKTESTER_HOLIDAYS_HPP
