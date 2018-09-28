# Require up-to-date CMake version
cmake_minimum_required(VERSION 3.10)
project(backtester)

# Set the initial CMake variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_INCLUDE_CURRENT_DIR YES)

# Bloomberg library includes
include_directories("C:\\blp\\C++\\include")
# Also include project level directories0000
include_directories(src include test)
add_subdirectory(include)
add_subdirectory(test)

# Initialize project sources here\
set(BACKTEST_SRCS
        src/data/dataretriever.cpp
        src/data/data.cpp
        src/constants.cpp
        src/infrastructure/events.cpp
        src/strategy/strategy.cpp)

# Build the backtester executable
add_executable(Backtester
        main.cpp
        ${BACKTEST_SRCS})

target_link_libraries(backtester_libs)

# Link the executable to the Bloomberg libraries
file(GLOB BLPAPI_LIBRARIES
        "C:/blp/C++/lib/*.lib")
message(STATUS "Bloomberg Libraries: ${BLPAPI_LIBRARIES}")
target_link_libraries(Backtester ${BLPAPI_LIBRARIES})