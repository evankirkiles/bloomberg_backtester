//
// Created by Evan Kirkiles on 9/27/2018.
//

#ifndef BACKTESTER_STRATEGY_HPP
#define BACKTESTER_STRATEGY_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"
// Custom class includes
#include "dataretriever.hpp"
#include "events.hpp"
#include "data.hpp"

// Base Strategy class to be inherited by all strategies.
//
// When the algorithm is run, each strategy should be run in a different thread. This allows several algorithms
// to be run rather than just a main strategy and a benchmark. Thus, each algo will have its own event list
// and will be completely self-contained. Graphics will be rendered after a backtest to improve performance.
//
// Before running, need to fill the HEAP event list with market events at a set frequency. Probably good enough to
// do this daily, as the market events will only be called for performance reporting. However, I need to make sure that
// the holdings are updated when a scheduled function is called. After the HEAP event list is filled with MarketEvents,
// the scheduled functions are then filled.
class BaseStrategy {
public:
    // List of the Bloomberg access symbols for all securities being run in algo
    const std::vector<std::string> symbol_list;

    // Public portfolio so it can be accessed by graphing components
    // Portfolio portfolio;

    // Runs the strategy itself, should be called on a new thread
    virtual void run()=0;
protected:
    const BloombergLP::blpapi::Datetime start_date, end_date;
    BloombergLP::blpapi::Datetime current_time;

    // Schedules member functions by putting a ScheduledEvent with a reference to the member function and a reference
    // to this strategy class on the HEAP event list. Then, the function is called at a specific simulated date.
    void schedule_function(void (*func));

    // STACK event queue, who must be empty for the HEAP event list to continue to run
    std::queue<std::unique_ptr<events::Event>> stack_eventqueue;
    // HEAP event list, to be run in order and simulate a moving calendar
    std::list<std::unique_ptr<events::Event>> heap_eventlist;

    // Other custom algorithmic components
    DataManager data_manager;
    // ExecutionHandler execution_handler;
};


// The actual strategy class which contains the logic for the strategy. It inherits from BaseStrategy so that the
// user does not have access to the complete back end code behind the functioning and so cannot mess much up.
class Strategy : public BaseStrategy {
public:
    Strategy(unsigned int initial_capital,
            const BloombergLP::blpapi::Datetime& start_date,
            const BloombergLP::blpapi::Datetime& end_date);

    void run();

private:
    const unsigned int initial_capital;
    const BloombergLP::blpapi::Datetime start_date, end_date;
};

#endif //BACKTESTER_STRATEGY_HPP
