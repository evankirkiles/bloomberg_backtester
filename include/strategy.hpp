//
// Created by Evan Kirkiles on 9/27/2018.
//

#ifndef BACKTESTER_STRATEGY_HPP
#define BACKTESTER_STRATEGY_HPP
// Bloomberg includes
#include "bloombergincludes.hpp"
// Include Google Test for production
#include <gtest/gtest_prod.h>
// Custom class includes
#include "events.hpp"
#include "dataretriever.hpp"
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

    // Initializes the base strategy params
    BaseStrategy(std::vector<std::string> symbol_list,
                unsigned int initial_capital,
                const BloombergLP::blpapi::Datetime& start,
                const BloombergLP::blpapi::Datetime& end,
                const std::string& type = "HISTORICAL DATA");

    // Public portfolio so it can be accessed by graphing components
    // Portfolio portfolio;

    // Runs the strategy itself, should be called on a new thread
    virtual void run()=0;
protected:
    const unsigned int initial_capital;
    const BloombergLP::blpapi::Datetime start_date, end_date;
    BloombergLP::blpapi::Datetime current_time;

    // Instances of the daterules for scheduling functions
    const DateRules date_rules;
    const TimeRules time_rules;

    // STACK event queue, who must be empty for the HEAP event list to continue to run
    std::queue<std::unique_ptr<events::Event>> stack_eventqueue;
    // HEAP event list, to be run in order and simulate a moving calendar
    std::list<std::unique_ptr<events::Event>> heap_eventlist;

    // Other custom algorithmic components

    // Each different type of strategy requires its OWN type of datahandler, so it is not initialized here
    // ExecutionHandler execution_handler;
};


// The actual strategy class which contains the logic for the strategy. It inherits from BaseStrategy so that the
// user does not have access to the complete back end code behind the functioning and so cannot mess much up.
class Strategy : public BaseStrategy {
public:
    Strategy(const std::vector<std::string>& symbol_list,
            unsigned int initial_capital,
            const BloombergLP::blpapi::Datetime& start_date,
            const BloombergLP::blpapi::Datetime& end_date);

    void run();

    // Functions to schedule
    void check();

    // Schedules member functions by putting a ScheduledEvent with a reference to the member function and a reference
    // to this strategy class on the HEAP event list. Then, the function is called at a specific simulated date.
    void schedule_function(void Strategy::* func, const DateRules& dateRules, const TimeRules& timeRules);

    // GTest friend funcs
    FRIEND_TEST(StrategyFixture, schedule_functions);

private:
    // The Data Manager for a Historical Strategy
    std::unique_ptr<HistoricalDataManager> data;
};

// Returns an iterator pointing to the first date on the event HEAP which is greater than the specified date. Will be
// used in scheduling functions t   o place the ScheduleEvents in between the MarketEvents
struct first_date_greater : public std::unary_function<BloombergLP::blpapi::Datetime, bool> {
    explicit first_date_greater(const BloombergLP::blpapi::Datetime& p_date) : date(p_date) {}
    const BloombergLP::blpapi::Datetime date;
    inline bool operator()(const std::unique_ptr<events::Event>& data) {
        // Returns true for the first element whose date is later by checking all datetime fields
        // If all fields are equal, still returns false because do not want to schedule before the same date
        if (data->datetime.year() > date.year()) return true;
        else if (data->datetime.year() < date.year()) return false;
        else {
            if (data->datetime.month() > date.month()) return true;
            else if (data->datetime.month() < date.month()) return false;
            else {
                if (data->datetime.day() > date.day()) return true;
                else if (data->datetime.day() < date.day()) return false;
                else {
                    if (data->datetime.hours() > date.hours()) return true;
                    else if (data->datetime.hours() < date.hours()) return false;
                    else {
                        if (data->datetime.minutes() > date.minutes()) return true;
                        else if (data->datetime.minutes() < date.minutes()) return false;
                        else {
                            if (data->datetime.seconds() > date.seconds()) return true;
                            else if (data->datetime.seconds() < date.seconds()) return false;
                            else {
                                return data->datetime.milliseconds() > date.milliseconds();
                            }
                        }
                    }
                }
            }
        }
    }
};

// Implementation of ScheduledFunction is in strategy class because needs to have a reference to Startegy object
// and do not want circular dependencies.
namespace events {
// Scheduled Event that is placed onto the stack depending on when the algorithm has scheduled it to run. This
// is the module that allows for functions to be run at certain times during the calendar. Rather than have a running
// clock which determines the time the function runs, the functions are simply placed in order on the heap.
//
// @member function        A reference to the strategy's function which is to be called upon event consumption
// @member instance        A reference to the strategy itself so its member function can be called
//
    struct ScheduledEvent : public Event {
        void Strategy::* function;
        Strategy* instance;

        // Print function
        void what() override;

        // Constructor for the ScheduledEvent
        ScheduledEvent(void Strategy::* function, Strategy* strat, const BloombergLP::blpapi::Datetime &when);

        // Runs the scheduled event
        void run();
    };

}

#endif //BACKTESTER_STRATEGY_HPP
