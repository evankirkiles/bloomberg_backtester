//
// Created by Evan Kirkiles on 10/9/2018.
//

// Include corresponding header
#include "slippage.hpp"

// Function to randomly sample from the slippage distribution as laid out by Quantopian and IB:
// https://blog.quantopian.com/accurate-slippage-model-comparing-real-simulated-transaction-costs/
// Uses a Log-Normal distribution with mean = 0 and standard deviation = 1, so slippage can't go negative
// but other than that looks fairly similar to IB's slippage statistics.
double Slippage::get_slippage(double order_cost) {
    // Build the lognormal distribution with a time-based seed:
    unsigned seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine generator (seed);
    std::lognormal_distribution<double> distribution (simulation::SLIPPAGE_LN_MEAN, simulation::SLIPPAGE_LN_SD);

    // Randomly sample a bps value from the distribution and return it as the slippage
    return order_cost * distribution(generator) / 10000;
}