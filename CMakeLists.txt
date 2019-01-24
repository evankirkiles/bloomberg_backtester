# Require up-to-date CMake version
cmake_minimum_required(VERSION 3.10)
project(backtester)

# Set the initial CMake variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_INCLUDE_CURRENT_DIR YES)
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Find the pthreads
find_package(Threads REQUIRED)

# Bloomberg library includes
include_directories("C:\\blp\\C++\\include")

# Also include project level directories
include_directories(src include test)
add_subdirectory(include)
add_subdirectory(test)
add_subdirectory(src/strategy/custom)

# Initialize project sources here
set(BACKTEST_SRCS
        src/data/dataretriever.cpp
        src/data/data.cpp
        src/constants.cpp
        src/holidays.cpp
        src/infrastructure/events.cpp
        src/infrastructure/daterules.cpp
        src/infrastructure/portfolio.cpp
        src/infrastructure/execution.cpp
        src/strategy/strategy.cpp
        src/simulation/slippage.cpp
        src/simulation/transactioncosts.cpp)

# Initialize custom algorithms here
set(ALGOS
        src/strategy/custom/basic_algo.cpp)

# Build the backtester executable
add_executable(Backtester
        main.cpp
        ${BACKTEST_SRCS}
        ${ALGOS})

target_link_libraries(backtester_libs)
target_link_libraries(strategy_files)

# Link the executable to the Bloomberg libraries
file(GLOB BLPAPI_LIBRARIES
        "C:/blp/C++/lib/*.lib")
message(STATUS "Bloomberg Libraries: ${BLPAPI_LIBRARIES}")
target_link_libraries(Backtester ${BLPAPI_LIBRARIES})

# Link the executable to pthread
target_link_libraries(Backtester Threads::Threads)