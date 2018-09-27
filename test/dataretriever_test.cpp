//
// Created by Evan Kirkiles on 9/26/2018.
//

// Google Test include
#include <gtest/gtest.h>
// Custom library includes
#include "constants.hpp"
#include "dataretriever.hpp"

// This class contains the unit tests for dataretriever.cpp / .hpp, the module which pulls data from Bloomberg.
// Tests are run using Google Test.

// MARK: Fixtures
// Initialize the test fixture for the data retriever
class DataRetrieverFixture : public ::testing::Test {
protected:
    void TearDown() override {}
    void SetUp() override {}

public:
    // No construction required
    DataRetrieverFixture() : Test() {}
    // Destructor is default as well
    ~DataRetrieverFixture() override = default;
};

// MARK: Tests
// Makes sure the Data Retriever opens a session with Bloomberg upon construction
TEST(DataRetrieverFixture, opens_session) { // NOLINT(cert-err58-cpp)
    // Build a DataRetrieverFixture and make sure session does not throw an error
    try { backtester::DataRetriever dr("HISTORICAL_DATA");
    } catch (std::runtime_error const & err) {
        FAIL() << "Session failed to start.";
    }
}

// Tests the Data Retriever's data access capabilities
TEST(DataRetrieverFixture, pulls_data) { // NOLINT(cert-err58-cpp)
    // Build a DataRetrieverFixture
    backtester::DataRetriever dr("HISTORICAL_DATA");
    std::unique_ptr<std::unordered_map<std::string, backtester::SymbolHistoricalData>> data = dr.pullHistoricalData(
            {"IBM US EQUITY"},
            BloombergLP::blpapi::Datetime(2005, 3, 3, 0, 0, 0, 0),
            BloombergLP::blpapi::Datetime(2006, 3, 3, 0, 0, 0, 0));
    // Uncomment this code to print out the data retrieved
//    auto i = data->operator[]("IBM US EQUITY").data.begin();
//    while (i != data->operator[]("IBM US EQUITY").data.end()) {
//        std::cout << "DATE: " << i->first << ", PX_LAST: " << i->second["PX_LAST"] << std::endl;
//        ++i;
//    }
    EXPECT_EQ(92.41, data->operator[]("IBM US EQUITY").data.begin()->second["PX_LAST"]);
}

// Tests the inline function's data formatting
TEST(DataRetrieverFixture, date_format) { // NOLINT(cert-err58-cpp)
    // Format a date
    BloombergLP::blpapi::Datetime date(2005, 3, 3, 0, 0, 0, 0);
    EXPECT_EQ("20050303", backtester::get_date_formatted(date));
}

