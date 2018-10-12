//
// Created by Evan Kirkiles on 10/5/2018.
//

// Include Google Test
#include <gtest/gtest.h>
// Include custom classes
#include "constants.hpp"
#include "strategy.hpp"
#include "strategy/custom/src/basic_algo.hpp"

// Test class for strategy-related functions

// MARK: Fixtures
// Fixture on which to run strategy tests
class StrategyFixture : public ::testing::Test {
protected:
    void TearDown() override {}
    void SetUp() override {}
public:
    // No construction required
    StrategyFixture() : Test() {}
    // Destructor is default as well
    ~StrategyFixture() override = default;
};

// MARK: Tests
// Checks the scheduling function to place the events on the stack correctly
TEST(StrategyFixture, schedule_functions) { // NOLINT(cert-err58-cpp)
    // Initialize a Strategy object
    Strategy strat({"IBM US EQUITY", "AAPL US EQUITY"}, 100000,
            BloombergLP::blpapi::Datetime(2014, 1, 1, 0, 0, 0),
            BloombergLP::blpapi::Datetime(2015, 1, 1, 0, 0, 0));

    // Schedule a function onto the heap event list
    strat.schedule_function(&Strategy::check, strat.date_rules.every_day(), TimeRules::market_open(1, 1));

    // Now check if the function was successful
//    for (const auto& i : strat.heap_eventlist) {
//        // Roll through all events
//        i->concise_what();
//    }
}

// Checks the run function of the base strategy with a placeholder check function scheduled every week open
TEST(StrategyFixture, run) { // NOLINT(cert-err58-cpp)
    // Initialize a Strategy object
    Strategy strat({"IBM US EQUITY", "AAPL US EQUITY"}, 100000,
                   BloombergLP::blpapi::Datetime(2014, 1, 1, 0, 0, 0),
                   BloombergLP::blpapi::Datetime(2015, 1, 1, 0, 0, 0));

    // Schedule a function onto the heap event list
    strat.schedule_function(&Strategy::check, strat.date_rules.every_day(), TimeRules::market_open(1, 1));

    // Run the function, which should print out "Function ran on DATE" several times
    EXPECT_NO_THROW(strat.run()); // NOLINT(cppcoreguidelines-avoid-goto)
}

// Check the run function of a derived strategy
TEST(StrategyFixture, run_derived) { // NOLINT(cert-err58-cpp)
    // Initialize a BasicAlgo object
    BasicAlgo strat(BloombergLP::blpapi::Datetime(2014, 1, 1, 0, 0, 0),
                    BloombergLP::blpapi::Datetime(2015, 1, 1, 0, 0, 0),
                    100000);

    // Run the function which should print out "CHECKED" several times
    EXPECT_NO_THROW(strat.run()); // NOLINT(cppcoreguidelines-avoid-goto)
}