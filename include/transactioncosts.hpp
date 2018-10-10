//
// Created by Evan Kirkiles on 10/10/2018.
//

#ifndef BACKTESTER_TRANSACTIONCOSTS_HPP
#define BACKTESTER_TRANSACTIONCOSTS_HPP
// Standard library includes
#include <cmath>
// Custom bloomberg includes
#include "bloombergincludes.hpp"

// Class containing simulation parameters for transaction costs. Custom transaction costs can be used here
// and then implemented into the backtester for different models of transaction costs.
// All members of this should be static, as there is no need to implement an instance of this struct.
struct TransactionCosts {

    // Gets the standard IB transaction costs for a quantity of stock, again as depicted by Quantopian:
    // https://blog.quantopian.com/accurate-slippage-model-comparing-real-simulated-transaction-costs/
    static double get_IB_transaction_cost(int quantity);
};

#endif //BACKTESTER_TRANSACTIONCOSTS_HPP
