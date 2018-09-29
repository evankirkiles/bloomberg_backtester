//
// Created by Evan Kirkiles on 9/27/2018.
//

#ifndef BACKTESTER_DATA_HPP
#define BACKTESTER_DATA_HPP

// Class for the data interface which will be used by Strategies. Components needed include a function to
// get market data N days back from a given date (currentDate will be passed in by strategy)
class HistoricalDataManager {
public:
    // Constructor builds the data manager with a data retriever connected to the Bloomberg API.
    explicit HistoricalDataManager();

    // Function that builds the Market Events and puts them onto the HEAP event list in increasing order of
    // dates.

};


#endif //BACKTESTER_DATA_HPP
