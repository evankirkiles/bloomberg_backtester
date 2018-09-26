//
// Created by Evan Kirkiles on 9/25/2018.
//

#ifndef BACKTESTER_DATARETRIEVER_HPP
#define BACKTESTER_DATARETRIEVER_HPP
// Bloomberg API includes
#include "bloombergincludes.hpp"

// Global namespace
namespace backtester {

// Class that contains the methods for data retrieval from Bloomberg API. In the future it will be
// modified to support subscriptions, but at the moment is only needed for backtesting and thus
// only gets historical data.
//
class DataRetriever {

    // Pulls
    static BloombergLP::blpapi::Message pullHistoricalData();

};

}

#endif //BACKTESTER_DATARETRIEVER_HPP
