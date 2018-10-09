//
// Created by Evan Kirkiles on 10/9/2018.
//

// Google Test include
#include <gtest/gtest.h>
// Include custom classes
#include "portfolio.hpp"

// MARK: Fixtures
// Fixture to run the portfolio tests with
class PortfolioFixture : public ::testing::Test {
protected:
    void TearDown() override {}
    void SetUp() override {}
public:
    // No construction required
    PortfolioFixture() : Test() {}
    // Destructor is default as well
    ~PortfolioFixture() override = default;
};

// MARK: Tests
// Checks the construction of the portfolio
TEST(PortfolioFixture, builds_empty_portfolio) { // NOLINT(cert-err58-cpp)
    // Build a Strategy in which the portfolio will be contained.
    // Initialize a Strategy object
    Portfolio portfolio({"IBM US EQUITY"}, 100000, BloombergLP::blpapi::Datetime(2010, 1, 1, 0, 0, 0));

    // Check that the portfolio in the strategy has only one entry
    EXPECT_EQ(portfolio.all_holdings.size(), 1);
    EXPECT_EQ(portfolio.all_holdings[BloombergLP::blpapi::Datetime(2010, 1, 1, 0, 0, 0)]["IBM US EQUITY"], 0);
}