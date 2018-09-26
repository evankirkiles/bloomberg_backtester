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
}