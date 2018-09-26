project(backtester_libs)

# Initialize project sources here
set(BACKTEST_HEADERS
        bloombergincludes.hpp
        constants.hpp
        dataretriever.hpp)
set(BACKTEST_SRCS
        ../src/data/dataretriever.cpp
        ../src/constants.cpp)

add_library(backtester_libs ${BACKTEST_HEADERS} ${BACKTEST_SRCS})