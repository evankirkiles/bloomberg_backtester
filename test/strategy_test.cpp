//
// Created by Evan Kirkiles on 10/5/2018.
//

// Include Google Test
#include <gtest/gtest.h>
// Include custom classes
#include "constants.hpp"
#include "strategy.hpp"

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
    for (const auto& i : strat.heap_eventlist) {
        // Roll through all events
        i->concise_what();
    }
}