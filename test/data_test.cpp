//
// Created by Evan Kirkiles on 9/30/2018.
//

// Google test include
#include <gtest/gtest.h>
// Custom library includes
#include "constants.hpp"
#include "data.hpp"

// Unit testing class for the Data Managers.

// MARK: Fixtures
// Initialize the test fixture for Historical Data Manager
class HistoricalDataManagerFixture : public ::testing::Test {
protected:
    void TearDown() override {}
    void SetUp() override {}
public:
    // No construction required
    HistoricalDataManagerFixture() : Test() {}
    // Destructor is default as well
    ~HistoricalDataManagerFixture() override = default;
};

// MARK: TESTS
// Makes sure the HistoricalDataManager fills the empty Event HEAP with MarketEvents
TEST(HistoricalDataManagerFixture, builds_market_events) { // NOLINT(cert-err58-cpp)
    // Build a placeholder Event HEAP onto which the events will be placed
    std::list<std::unique_ptr<events::Event>> fake_heap;
    // Build the start and end dates of the backtest
    BloombergLP::blpapi::Datetime start = BloombergLP::blpapi::Datetime::createDate(2018, 7, 3);
    BloombergLP::blpapi::Datetime end = BloombergLP::blpapi::Datetime::createDate(2018, 8, 3);
    // Also use a fake current
    BloombergLP::blpapi::Datetime current_time = BloombergLP::blpapi::Datetime::createDate(2018, 8, 3);
    // Create the data manager with a reference to the fake time
    backtester::HistoricalDataManager hdm(&current_time);

    // Now try to fill the event HEAP
    EXPECT_NO_THROW(hdm.fillHistory({"IBM US EQUITY", "GOOG"}, start, end, &fake_heap)); // NOLINT(cppcoreguidelines-avoid-goto)

//    // Print out the results of the heap
//    for (auto &i : fake_heap) { i->what(); }
}

// Test for the data pulling capabilities of the Data Manager.
TEST(HistoricalDataManagerFixture, history) { // NOLINT(cert-err58-cpp)
    // Build the fake current time
    BloombergLP::blpapi::Datetime current_time = BloombergLP::blpapi::Datetime::createDate(2018, 8, 3);
    // Create the data manager with a reference to the fake time
    backtester::HistoricalDataManager hdm(&current_time);

    // Now try pulling the historical data
    std::unique_ptr<std::unordered_map<std::string, backtester::SymbolHistoricalData>> data;
    EXPECT_NO_THROW(data = hdm.history({"IBM US EQUITY", "GOOG US EQUITY"}, {"PX_LAST"}, 20, "DAILY")); // NOLINT(cppcoreguidelines-avoid-goto)

//  // Check the data
    auto i = data->operator[]("IBM US EQUITY").data.begin();
    while (i != data->operator[]("IBM US EQUITY").data.end()) {
        std::cout << "DATE: " << i->first << ", PX_LAST: " << i->second["PX_LAST"] << std::endl;
        ++i;
    }
}
