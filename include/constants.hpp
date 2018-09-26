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
}

#endif //BACKTESTER_CONSTANTS_HPP
