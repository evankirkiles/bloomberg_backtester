//
// Created by bloomberg on 9/25/2018.
//

// Include the declarations
#include "constants.hpp"

// Definition of global constants, each in their respective namespaces.
namespace bloomberg_session {
    const char* HOST("localhost");
    const uint16_t PORT(8194);
}

// Services available through the Bloomberg APi
namespace bloomberg_services {
    const char* REFDATA("//blp/refdata");
    const char* MKTDATA("//blp/mktdata");
}

// Definition of element-related constants for accessing Bloomberg data
namespace element_names {
    const char* SECURITY_DATA("securityData");
    const char* SECURITY_NAME("security");
    const char* DATE("date");

    const char* FIELD_ID("fieldId");
    const char* FIELD_DATA("fieldData");
    const char* FIELD_DESC("description");
    const char* FIELD_INFO("fieldInfo");
    const char* FIELD_ERROR("fieldError");
    const char* FIELD_MSG("message");
    const char* SECURITY_ERROR("securityError");
    const char* ERROR_MESSAGE("message");
    const char* FIELD_EXCEPTIONS("fieldExceptions");
    const char* ERROR_INFO("errorInfo");
    const char* RESPONSE_ERROR("responseError");
    const char* CATEGORY("category");
    const char* SUBCATEGORY("subcategory");
    const char* MESSAGE("message");
    const char* CODE("code");
}

// Enums for the date rules specifying types
namespace date_time_enums {
    const unsigned int EVERY_DAY = 0;
    const unsigned int WEEK_START = 1;
    const unsigned int WEEK_END = 2;
    const unsigned int MONTH_START = 3;
    const unsigned int MONTH_END = 4;

    const unsigned int T_MARKET_OPEN = 0;
    const unsigned int T_MARKET_CLOSE = 1;

    const unsigned int US_MARKET_OPEN_HOUR = 9;
    const unsigned int US_MARKET_OPEN_MINUTE = 0;
    const unsigned int US_MARKET_CLOSE_HOUR = 1;
    const unsigned int US_MARKET_CLOSE_MINUTE = 0;

    const unsigned int US_MARKET_EARLY_CLOSE_HOUR = 13;
    const unsigned int US_MARKET_EARLY_CLOSE_MINUTE = 0;
}