//
// Created by Evan Kirkiles on 10/10/2018.
//

// Include corresponding header
#include "transactioncosts.hpp"

// Function to get IB commission rates for a quantity and cost of an order
double TransactionCosts::get_IB_transaction_cost(int quantity) {
    // LOGIC:
    //  1. Each share has a commission fee of $0.001
    //  2. There is a $1.00 minimum cost per order.
    return std::max(1.00, quantity * 0.001);
}