//
// Created by Evan Kirkiles on 10/4/2018.
//

// Google Test include
#include <gtest/gtest.h>
// Custom library includs
#include "daterules.hpp"

// Test class for the Date Rules object which will be used for scheduling functions.

// MARK: Fixtures
// Initialize the test fixture for the Time Rules
class TimeRulesFixture : public ::testing::Test {
protected:
    void TearDown() override {}
    void SetUp() override {}
public:
    // No construction required
    TimeRulesFixture() : Test() {}
    // Destructor is default as well
    ~TimeRulesFixture() override = default;
};

// Initialize the test fixture for the Date Rules
class DateRulesFixture : public ::testing::Test {
protected:
    void TearDown() override {}
    void SetUp() override {}
public:
    // No construction required
    DateRulesFixture() : Test() {}
    // Destructor is default as well
    ~DateRulesFixture() override = default;
};

// MARK: TESTS
// Tests the DateRules date selector
TEST(DateRulesFixture, gets_dates) { // NOLINT(cert-err58-cpp)
    // Create a reference start date and end date
    DateRules dr(BloombergLP::blpapi::Datetime(2014, 11, 1, 0, 0, 0),
                    BloombergLP::blpapi::Datetime(2014, 12, 30, 0, 0, 0), 0);
    // Create the time rules as well
    TimeRules tr = TimeRules::market_close();

    // Now get a vector of the dates
    std::vector<BloombergLP::blpapi::Datetime> dates = dr.get_date_times(tr);
//    // Print out the dates retrieved
    for (const auto &i : dates) {
        // Print the date
        std::cout << "Date: " << i << std::endl;
    }

    // Expect the dates object to not be empty
    EXPECT_NE(0, dates.size());
}