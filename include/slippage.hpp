//
// Created by Evan Kirkiles on 10/9/2018.
//

#ifndef BACKTESTER_SLIPPAGE_HPP
#define BACKTESTER_SLIPPAGE_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"
// STL includes
#include <random>
#include <chrono>
// Custom class includes
#include "constants.hpp"

// Class including the module for simulating slippage in the execution handler. All of its functions will be
// static so no local instances are needed to call them
struct Slippage {
    // Returns a cost of slippage based on the size of an order
    static double get_slippage(double order_cost);
};

#endif //BACKTESTER_SLIPPAGE_HPP
