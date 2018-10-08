//
// Created by Evan Kirkiles on 10/8/2018.
//

#ifndef BACKTESTER_PORTFOLIO_HPP
#define BACKTESTER_PORTFOLIO_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"
// Custom class includes
#include "events.hpp"
#include "constants.hpp"

// Class for the Portfolio object which keeps track of holdings and positions for the strategy. This will
// receive market events and fill events passed in to it by the Strategy event loop, which will be used to recalculate
// the value of the portfolio. Performance statistic calculations also take place in this class.
class Portfolio {
public:
    // Initializes the portfolio given the symbols and initial capital
    Portfolio(std::vector<std::string> symbol_list, unsigned int initial_capital, const BloombergLP::blpapi::Datetime& start);

    // Resets the portfolio with a new initial capital amount and a new start date.
    // This function is used in the constructor as well to initialize the portfolio.
    void reset_portfolio(unsigned int initial_capital, const BloombergLP::blpapi::Datetime& start);

    // Takes a market event and uses it to update the holdings for a specific stock. This will be called for every
    // stock in the portfolio (hopefully). I still do not know what happens when there does not exist data for a stock
    // on a specific day, so hopefully Bloomberg has done all the data cleaning beforehand.
    void update_market(const events::MarketEvent& event);

    // Takes a fill event and uses it to update the positions and holdings for a specific stock. The fill event comes
    // from the execution handler and contains a buy or sell quantity that has already been calculated and optimized.
    void update_fill(const events::FillEvent& event);

private:

    // Writes the current holdings and current positions into their respective all_holdings maps
    void push_holdings_and_positions(const BloombergLP::blpapi::Datetime& date);
    // Calculates total holdings, returns, and equity curve
    void calculate_returns();

    // Portfolio instance member functions
    std::vector<std::string> symbol_list;
    unsigned int initial_capital;
    BloombergLP::blpapi::Datetime start_date;

    // The maps of the positions at their respective Bloomberg datetimes (quantities of each stock)
    std::map<BloombergLP::blpapi::Datetime, std::unordered_map<std::string, int>> all_positions;
    std::unordered_map<std::string, int> current_positions;
    // The maps of the holdings at their respective Bloomberg datetimes (quantity * price of each stock)
    std::map<BloombergLP::blpapi::Datetime, std::unordered_map<std::string, double>> all_holdings;
    std::unordered_map<std::string, double> current_holdings;
};

#endif //BACKTESTER_PORTFOLIO_HPP
