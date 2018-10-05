# Require up-to-date CMake version
cmake_minimum_required(VERSION 3.10)
project(backtestertests)

# Set the initial CMake variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_INCLUDE_CURRENT_DIR YES)

# Bloomberg library includes
include_directories("C:\\blp\\C++\\include")
# Also include project level directories0000
include_directories(../src ../include)
# Also, because using Google Test, include its local includes
include_directories(lib/googletest/include lib/googletest)
add_subdirectory(lib/googletest)

# Initialize project sources here
set(BACKTEST_TESTS
        dataretriever_test.cpp
        data_test.cpp
        daterules_test.cpp
        strategy_test.cpp)

# Build the backtester executable
add_executable(BacktesterTests
        ${BACKTEST_TESTS})

# Link the executable to the Bloomberg libraries
target_link_libraries(BacktesterTests backtester_libs)
file(GLOB BLPAPI_LIBRARIEST
        "C:/blp/C++/lib/*.lib")
target_link_libraries(BacktesterTests ${BLPAPI_LIBRARIEST})
# Link the executable to Google Test
target_link_libraries(BacktesterTests gtest gtest_main)
