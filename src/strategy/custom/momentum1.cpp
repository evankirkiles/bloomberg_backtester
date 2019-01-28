//
// Created by Evan Kirkiles on 1/25/2019.
//

// Include corresponding header
#include "strategy/custom/src/momentum1.hpp"

// Initialize the strategy to backtest
ALGO_Momentum1::ALGO_Momentum1(const BloombergLP::blpapi::Datetime &start, const BloombergLP::blpapi::Datetime &end,
                     unsigned int capital) :
        Strategy({"DIA US EQUITY", "QQQ US EQUITY", "LQD US EQUITY",
                  "HYG US EQUITY", "USO US EQUITY", "GLD US EQUITY",
                  "VNQ US EQUITY", "RWX US EQUITY", "UNG US EQUITY",
                  "DBA US EQUITY"},
                 capital, start, end) {

    // Perform constant declarations and definitions here.
    context["lookback"] = 126;                                // The lookback for the moving average
    context["maxleverage"] = 0.9;                             // The maximum leverage allowed
    context["multiple"] = 5.0;                                // 1% of return translates to what weight? e.g. 5%
    context["profittake"] = 1.96;                             // Take profits when breaks out of 95% Bollinger Band
    context["slopemin"] = 0.252;                              // Minimum slope on which to be trading on
    context["dailyvolatilitytarget"] = 0.025;                 // Target daily volatility, in percent

    // Dictionary of weights, set for each security
    for (const std::string& sym : symbol_list) {
        symbolspecifics[sym]["weight"] = 0.0;
        symbolspecifics[sym]["stopprice"] = nan("idk");
    }

    // Perform function scheduling here.
    // This lambda is annoying but necessary for downcasting the Strategy to the type of your algorithm when
    // it is known, otherwise cannot have schedule_function declared in Strategy base class.

    // Every 5 minutes during market hours
    // 1. Checks for any exit conditions in securities where weight != 0
    schedule_function([](Strategy* x)->void { auto a = dynamic_cast<ALGO_Momentum1*>(x); if (a) a->exitconditions(); },
                      date_rules.every_day(), TimeRules::market_open(0, 10));

    // 28 minutes after market opens
    // 2. Performs the regression and calculates the weights for any new trends
    schedule_function([](Strategy* x)->void { auto b = dynamic_cast<ALGO_Momentum1*>(x); if (b) b->regression(); },
                      date_rules.every_day(), TimeRules::market_open(0, 28));

    // 30 minutes after market opens
    // 3. Performs trades and notifies us of any required buys/sells
    schedule_function([](Strategy* x)->void { auto c = dynamic_cast<ALGO_Momentum1*>(x); if (c) c->trade(); },
                      date_rules.every_day(), TimeRules::market_open(0, 30));
}

// Checks for new trends available to go in on. Conditions:
//  1. Normalized slope over past [lookback] days is greater than [minslope]
//  2. Price has crossed the regression line.
void ALGO_Momentum1::regression() {
    log("progress");
    // Pull the past [lookback] of data from Bloomberg
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> prices =
            data->history(symbol_list, {"PX_OPEN"}, (unsigned int) std::ceil(context["lookback"]*1.6), "DAILY");

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

        // Get the price series fo
        // With the price vector built, perform the regression
        std::pair<double, double> results = calcreg(x);
        // Get the normalized slope (return per year)
        const double slope = results.first / results.second * 252.0;
        // Calculate the difference in actual price vs regression over the past 2 days
        auto priceiter = prices->at(symbol).data.rbegin();
        const double delta1 = (*priceiter).second["PX_OPEN"] - (results.first*context["lookback"] + results.second);
        priceiter++;
        const double delta2 = (*priceiter).second["PX_OPEN"] - (results.first*(context["lookback"]-1) + results.second);
        // Also get the standard deviation of the price series
        const double sd = sqrt((sum_x2 / context["lookback"]) - ((sum_x1 / context["lookback"]) * (sum_x1 / context["lookback"])));

        log("1");

        // If long but the slope turns down, exit
        if (symbolspecifics[symbol]["weight"] > 0 && slope < 0) {
            symbolspecifics[symbol]["weight"] = 0.0;
            log("v Slope turned bull " + symbol);
        // If shortbut the slope turns up, exit
        } else if (symbolspecifics[symbol]["weight"] < 0 && slope > 0) {
            symbolspecifics[symbol]["weight"] = 0.0;
            log("v Slope turned bear " + symbol);
        }

        // If the trend is up enough
        if (slope > context["slopemin"]) {
            // If the price crosses the regression line and not already in a position for the stock
            if (delta1 > 0 && delta2 < 0 && symbolspecifics[symbol]["weight"] == 0) {
                // Set the weight to be ordered at the end of the day, and clear the stopprice
                symbolspecifics[symbol]["stopprice"] = nan("idk");
                symbolspecifics[symbol]["weight"] = slope;
                log(std::string("---------- Long  a = ") + std::to_string(slope * 100) + "% for " + symbol);
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
                log(std::string("---------- Short  a = ") + std::to_string(slope * 100) + "% for " + symbol);
                // If the price is less than the profit take Bollinger Band and we are short in it
            } else if (delta1 < -context["profittake"] * sd && symbolspecifics[symbol]["weight"] < 0) {
                // Exit the position by setting the weight to 0
                symbolspecifics[symbol]["weight"] = 0.0;
                log("---- Exit short in " + symbol);
            }
        }
    }

    log("2");
}

// Exits positions where the trend seems to be fading. Conditions:
//  1. Stop price is hit (the estimated price before the lookback period according to the regression)
void ALGO_Momentum1::exitconditions() {
    // Again, iterate through the symbols to perform the calculations for each stock
    for (const std::string& symbol : symbol_list) {
        // Get the mean price over the past couple of days as a more robust statistic
        std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> prices =
                data->history(symbol_list, {"PX_LAST"}, 4, "DAILY");
        double price=0;
        for (auto &iter : prices->at(symbol).data) {
            price += iter.second["PX_LAST"];
        }

        // Now calculate the stop loss percentage as the estimated return over the lookback
        double stoploss = std::abs(symbolspecifics[symbol]["weight"] * context["lookback"] / 252.0) + 1;

        // Check if we should exit depending on whether we are long or short
        if (symbolspecifics[symbol]["weight"] > 0) {
            // If the stop price is negative, we need to recalculate it
            if (symbolspecifics[symbol]["stopprice"] == nan("idk") || symbolspecifics[symbol]["stopprice"] < 0) {
                symbolspecifics[symbol]["stopprice"] = price / stoploss;
            // Otherwise, check the current price against the stop price
            } else {
                // Only update the stop price if it has increased so we are safer and do not have a low stop price.
                symbolspecifics[symbol]["stopprice"] = std::max(price/stoploss, symbolspecifics[symbol]["stopprice"]);
                // Make sure the price is outside of the stop price anyways, if not then sell all shares
                if (price < symbolspecifics[symbol]["stopprice"]) {
                    // Notify us of the exiting of the position
                    message("x Long stop loss for " + symbol+ ", sell all shares.");
                    symbolspecifics[symbol]["weight"] = 0;
                    // We just use order percent here because we want to exit the trend immediately (not end of day)
                    order_target_percent(symbol, 0);
                }
            }
        } else if (symbolspecifics[symbol]["weight"] < 0) {
            // If the stop price is negative, we need to recalculate it
            if (symbolspecifics[symbol]["stopprice"] == nan("idk") || symbolspecifics[symbol]["stopprice"] < 0) {
                symbolspecifics[symbol]["stopprice"] = price / stoploss;
                // Otherwise, check the current price against the stop price
            } else {
                // Only update the stop price if it has decreased so we are safer and do not have a high stop price.
                symbolspecifics[symbol]["stopprice"] = std::min(price/stoploss, symbolspecifics[symbol]["stopprice"]);
                // Make sure the price is outside of the stop price anyways, if not then sell all shares
                if (price > symbolspecifics[symbol]["stopprice"]) {
                    // Notify us of the exiting of the position
                    message("x Short stop loss for " + symbol+ ", sell all shares.");
                    symbolspecifics[symbol]["weight"] = 0;
                    // We just use order percent here because we want to exit the trend immediately (not end of day)
                    order_target_percent(symbol, 0);
                }
            }
        } else {
            symbolspecifics[symbol]["stopprice"] = nan("idk");
        }
    }
}

// Performs trades based on the weights in our symbolspecifics object.
// Note the volatility scalar which changes our positions based on how volatile the asset is.
void ALGO_Momentum1::trade() {
    // First, calculate the volatility scalar
    std::unordered_map<std::string, double> vol_mult = volatilityscalars();
    // Also keep track of how many securities we do not have a position in
    int nopositions = 0;
    for (const std::string& sym : symbol_list) { if (symbolspecifics[sym]["weight"] == 0) { nopositions++; } }

    // Now iterate through the weights and perform the trades
    for (const std::string& symbol : symbol_list) {
        if (symbolspecifics[symbol]["weight"] == 0) {
            // Log the exiting of the position
            order_target_percent(symbol, 0);
        } else if (symbolspecifics[symbol]["weight"] > 0) {
            double percent = (std::min(symbolspecifics[symbol]["weight"] * context["multiple"], context["maxlever"]) / nopositions) * vol_mult[symbol];
            message(std::string("^ Go long ") + std::to_string(percent) + "% in " + symbol);
            order_target_percent(symbol, percent);
        } else if (symbolspecifics[symbol]["weight"] < 0) {
            double percent = (std::max(symbolspecifics[symbol]["weight"] * context["multiple"], -context["maxlever"]) / nopositions) * vol_mult[symbol];
            message(std::string("v Go short ") + std::to_string(percent) + "% in " + symbol);
            order_target_percent(symbol, percent);
        }
    }
}

// Performs a linear regression to get the slope and intercept of a line for a given vector
// First element of tuple is the slope, second is the intercept.
std::pair<double, double> ALGO_Momentum1::calcreg(std::vector<double> x) {
    double xSum=0, ySum=0, xxSum=0, xySum=0, slope=0, intercept=0;
    // Build a vector of increasing-by-one integers to act as the Y array
    std::vector<int> y(x.size());
    std::iota(std::begin(y), std::end(y), 0);
    // Now find the slope and intercept
    for (int i = 0; i < y.size(); i++) {
        xSum += y[i];
        ySum += x[i];
        xxSum += y[i] * y[i];
        xySum += y[i] * x[i];
    }
    slope = (y.size()*xySum - xSum*ySum) / (y.size()*xxSum - xSum*xSum);
    intercept = (xxSum*ySum - xySum*xSum) / (y.size()*xxSum - xSum*xSum);
    return std::make_pair(slope, intercept);
}

// Calculates the volatility scalar for each stock, based on the standard deviation of the log returns.
std::unordered_map<std::string, double> ALGO_Momentum1::volatilityscalars() {
    // First, retrieve all the prices over the lookback
    std::unique_ptr<std::unordered_map<std::string, SymbolHistoricalData>> prices =
            data->history(symbol_list, {"PX_OPEN"}, (unsigned int) std::ceil(context["lookback"]*1.6), "DAILY");

    // The map to return with each symbol's volatility scalar
    std::unordered_map<std::string, double> vol_mult;
    // Iterate through each symbol to perform logic for each
    for (const std::string& symbol : symbol_list) {
        // First build vectors for each symbol
        std::vector<double> x;
        auto iter = prices->at(symbol).data.begin();
        double previous = 0;
        for (std::advance(iter, (prices->at(symbol).data.size() - (int) context["lookback"]));
             iter != prices->at(symbol).data.end(); iter++) {
            double val = (*iter).second["PX_OPEN"];
            if (previous != 0) { x.emplace_back(std::log(val) - previous); }
            previous = std::log(val);
        }

        // Now find the exponentially weighted moving standard deviation of the last month of log differences in x
        double alpha = 2.0 / (21 + 1);
        std::vector<double> ema = x;
        std::vector<double> ewmstd = {0};
        // Then we can calculate the ewmst recursively
        for (int i = 1; i < ema.size(); i++) {
            ema[i] = x[i] + (1 - alpha)*ema[i-1];
            ewmstd.emplace_back((1 - alpha)*(ema[i-1] + alpha*pow(x[i]-ema[i-1], 2)));
        }
        // Finally, return the last value of the ewmstd
        vol_mult[symbol] = context["dailyvolatilitytarget"] / sqrt(ewmstd[ewmstd.size() - 1]);
    }
    return vol_mult;
}