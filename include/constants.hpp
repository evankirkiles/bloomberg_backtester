//
// Created by bloomberg on 9/25/2018.
//

#ifndef BACKTESTER_CONSTANTS_HPP
#define BACKTESTER_CONSTANTS_HPP

// Include the different integer types
#include <cstdint>

// Header file containing the declarations of system-wide constants for the backtester.
// Different types of constants are denoted by different namespaces
namespace bloomberg_session {
    extern const char* HOST;
    extern const uint16_t PORT;
}

// Services available
namespace bloomberg_services {
    extern const char* REFDATA;
    extern const char* MKTDATA;
}

// Data retrieval names for accessing different elements
namespace element_names {
    extern const char* SECURITY_DATA;
    extern const char* SECURITY_NAME;
    extern const char* DATE;
    extern const char* FIELD_ID;
    extern const char* FIELD_DATA;
    extern const char* FIELD_DESC;
    extern const char* FIELD_INFO;
    extern const char* FIELD_ERROR;
    extern const char* FIELD_MSG;
    extern const char* SECURITY_ERROR;
    extern const char* ERROR_MESSAGE;
    extern const char* FIELD_EXCEPTIONS;
    extern const char* ERROR_INFO;
    extern const char* RESPONSE_ERROR;
    extern const char* CATEGORY;
    extern const char* SUBCATEGORY;
    extern const char* MESSAGE;
    extern const char* CODE;
}

// Names for data in the portfolio
namespace portfolio_fields {
    extern const char* HELD_CASH;
    extern const char* COMMISSION;
    extern const char* SLIPPAGE;
    extern const char* TOTAL_HOLDINGS;
    extern const char* RETURNS;
    extern const char* EQUITY_CURVE;
    extern const char* RESERVE_CASH;
}


// Enums for the date rules specifying types
namespace date_time_enums {
    extern const unsigned int EVERY_DAY;
    extern const unsigned int WEEK_START;
    extern const unsigned int WEEK_END;
    extern const unsigned int MONTH_START;
    extern const unsigned int MONTH_END;

    extern const unsigned int T_MARKET_OPEN;
    extern const unsigned int T_MARKET_CLOSE;

    extern const unsigned int US_MARKET_OPEN_HOUR;
    extern const unsigned int US_MARKET_OPEN_MINUTE;
    extern const unsigned int US_MARKET_CLOSE_HOUR;
    extern const unsigned int US_MARKET_CLOSE_MINUTE;

    extern const unsigned int US_MARKET_EARLY_CLOSE_HOUR;
    extern const unsigned int US_MARKET_EARLY_CLOSE_MINUTE;
}

// Simulation-related variables
namespace simulation {
    extern const double SLIPPAGE_LN_MEAN;
    extern const double SLIPPAGE_LN_SD;
}

#endif //BACKTESTER_CONSTANTS_HPP
