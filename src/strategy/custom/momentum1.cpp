//
// Created by Evan Kirkiles on 1/25/2019.
//

// Include corresponding header
#include "strategy/custom/src/momentum1.hpp"

// Initialize the strategy to backtest
ALGO_Momentum1::ALGO_Momentum1(const BloombergLP::blpapi::Datetime &start, const BloombergLP::blpapi::Datetime &end,
                     unsigned int capital) :
        Strategy({"SPY US EQUITY", "EFA US EQUITY", "BND US EQUITY",
                  "VNQ US EQUITY", "GSG US EQUITY", "BIL US EQUITY"},
                 capital, start, end) {

    // Perform constant declarations and definitions here.
    context["lookback"] = 126;                                // The lookback for the moving average
    context["maxleverage"] = 0.9;                             // The maximum leverage allowed
    context["multiple"] = 5.0;                                // 1% of return translates to what weight? e.g. 5%
    context["profittake"] = 1.96;                             // Take profits when breaks out of 95% Bollinger Band
    context["slopemin"] = 0.252;                              // Minimum slope on which to be trading on

    // Dictionary of weights, set for each security
    for (const std::string& sym : symbol_list) {
        symbolspecifics[sym]["weight"] = 0.0;
        symbolspecifics[sym]["stopprice"] = nan("idk");
    }

    // Perform function scheduling here.
    // This lambda is annoying but necessary for downcasting the Strategy to the type of your algorithm when
    // it is known, otherwise cannot have schedule_function declared in Strategy base class.
    schedule_function([](Strategy* x)->void { auto b = dynamic_cast<ALGO_Momentum1*>(x); if (b) b->rebalance(); },
                      date_rules.every_day(), TimeRules::market_open(0, 28));
}

// The test function for the Basic Algo
void ALGO_Momentum1::rebalance() {
    // Pull the past [lookback] of data from Bloomberg
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> prices =
            data->history(symbol_list, {"PX_OPEN"}, (unsigned int) std::ceil(context["lookback"]*1.4), "daily");

    // Iterate through each symbol to perform logic for each
    for (const std::string& symbol : symbol_list) {
        // First build the vector for which regression can be calculated, along with a sd vairable
        std::vector<double> x;
        double sum_x1=0, sum_x2=0;
        auto iter = prices->at(symbol).data.begin();
        for (std::advance(iter, (prices->at(symbol).data.size() - (int)context["lookback"]));
                iter != prices->at(symbol).data.end(); iter++) {
            double val = (*iter).second["PX_OPEN"];
            x.emplace_back(val);
            sum_x1 += val;
            sum_x2 += val * val;
        }

        // With the price vector built, perform the regression
        std::pair<double, double> results = regression(x);

        // Get the normalized slope (return per year)
        const double slope = results.first / results.second * 252.0;
        // Calculate the difference in actual price vs regression over the past 2 days
        const double delta1 = (*prices->at(symbol).data.rbegin()).second["PX_OPEN"] - (results.first*context["lookback"] - results.second);
        const double delta2 = (*(prices->at(symbol).data.rbegin() + 1)).second["PX_OPEN"] - (results.first*(context["lookback"]-1) - results.second);
        // Also get the standard deviation of the price series
        const double sd = sqrt((sum_x2 / context["lookback"]) - ((sum_x1 / context["lookback"]) * (sum_x1 / context["lookback"])));

        // If long but the slope turns down, exit
        if (symbolspecifics[symbol]["weight"] > 0 && slope < 0) {
            symbolspecifics[symbol]["weight"] = 0.0;
            log("v Slope turned bull " + symbol)
        // If shortbut the slope turns up, exit
        } else if (symbolspecifics[symbol]["weight"] < 0 && slope > 0) {
            symbolspecifics[symbol]["weight"] = 0.0;
            log("v Slope turned bear " + symbol)
        }

        // If the trend is up enough
        if (slope > context["slopemin"]) {
            // If the price crosses the regression line and not already in a position for the stock
            if (delta1 > 0 && delta2 < 0 && symbolspecifics[symbol]["weight"] == 0) {
                // Set the weight to be ordered at the end of the day, and clear the stopprice
                symbolspecifics[symbol]["stopprice"] = nan("idk");
                symbolspecifics[symbol]["weight"] = slope;
                log("---------- Long  a = " + (slope * 100) + "% for " + symbol);
            // If the price is greater than the profit take Bollinger Band and we are long in it
            } else if (delta1 > context["profittake"] * sd && symbolspecifics[symbol]["weight"] > 0) {
                // Exit the position by setting the weight to 0
                symbolspecifics[symbol]["weight"] = 0.0;
                log("---- Exit long in " + symbol);
            }
        // If the trend is down enough
        } else if (slope < -context["slopemin"]) {
            // If the price crosses the regression line and not already in a position for the stock
            if (delta1 < 0 && delta2 > 0 && symbolspecifics[symbol]["weight"] == 0) {
                // Set the weight to be ordered at the end of the day, and clear the stopprice
                symbolspecifics[symbol]["stopprice"] = nan("idk");
                symbolspecifics[symbol]["weight"] = slope;
                log("---------- Short  a = " + (slope * 100) + "% for " + symbol);
                // If the price is greater than the profit take Bollinger Band and we are long in it
            } else if (delta1 < -context["profittake"] * sd && symbolspecifics[symbol]["weight"] < 0) {
                // Exit the position by setting the weight to 0
                symbolspecifics[symbol]["weight"] = 0.0;
                log("---- Exit short in " + symbol);
            }
        }
    }
}

// Performs a linear regression to get the slope and intercept of a line for a given vector
// First element of tuple is the slope, second is the intercept.
std::pair<double, double> ALGO_Momentum1::regression(std::vector<double> x) {
    double xSum=0, ySum=0, xxSum=0, xySum=0, slope, intercept;
    // Build a vector of increasing-by-one integers to act as the Y array
    std::vector<int> y(x.size());
    std::iota(std::begin(y), std::end(y), 0);
    // Now find the slope and intercept
    for (int i = 0; i < x.size(); i++) {
        xSum += x[i];
        ySum += y[i];
        xxSum += x[i] * x[i];
        xySum += x[i] * y[i];
    }
    slope = (y.size()*xySum - xSum*ySum) / (y.size()*xxSum - xSum*xSum);
    intercept = (ySum - slope*xSum) / y.size();
    return std::make_pair(slope, intercept);
}