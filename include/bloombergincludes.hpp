//
// Created by Evan Kirkiles on 9/25/2018.
//

#ifndef BACKTESTER_BLOOMBERGINCLUDES_HPP
#define BACKTESTER_BLOOMBERGINCLUDES_HPP

// Header containing all the header files needed for includes in files using the Bloomberg API.
// This reduces redundancy in any Bloomberg API-using files.
#include <blpapi_defs.h>
#include <blpapi_correlationid.h>
#include <blpapi_element.h>
#include <blpapi_event.h>
#include <blpapi_exception.h>
#include <blpapi_message.h>
#include <blpapi_session.h>
#include <blpapi_subscriptionlist.h>
#include <cstdlib>

// Also provide an include to common libraries
#include <iostream>
#include <string>
#include <memory>

#endif //BACKTESTER_BLOOMBERGINCLUDES_HPP
