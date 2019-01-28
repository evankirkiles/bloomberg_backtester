//
// Created by Evan Kirkiles on 1/25/2019.
//

#ifndef BACKTESTER_MOMENTUM1_HPP
#define BACKTESTER_MOMENTUM1_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"
// Include custom classes
#include "strategy.hpp"

// Strategy-specific includes
#include <algorithm>
#include <numeric>
#include <tuple>
#include <deque>

// LOGIC: A simple strategy which buys 10% in SPY, AAPL, and CAT on the first day and then holds for duration.
class ALGO_Momentum1 : public Strategy {
public:
    // Constructor initializes Strategy parent
    ALGO_Momentum1(const BloombergLP::blpapi::Datetime& start, const BloombergLP::blpapi::Datetime& end, unsigned int capital);

    // Trading logic goes here
    void regression();
    // Exit conditions go here
    void exitconditions();
    // Trade execution goes here
    void trade();

private:
    // Holds the weights in each algo
    std::unordered_map<std::string, std::unordered_map<std::string, double>> symbolspecifics;
    // Calculates the slope and intercept beginning at a certain value until the end
    std::pair<double, double> calcreg(std::vector<double> x);
    // Calcualtes the volatility scalar for each stock
    std::unordered_map<std::string, double> volatilityscalars();
};

#endif //BACKTESTER_MOMENTUM1_HPP
