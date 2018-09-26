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

