project(backtester_libs)

# Initialize project sources here
set(BACKTEST_HEADERS
        bloombergincludes.hpp
        constants.hpp
        holidays.hpp
        dataretriever.hpp
        data.hpp
        daterules.hpp
        events.hpp
        strategy.hpp
        portfolio.hpp
        execution.hpp)
set(BACKTEST_SRCS
        ../src/data/dataretriever.cpp
        ../src/constants.cpp
        ../src/holidays.cpp
        ../src/data/data.cpp
        ../src/infrastructure/daterules.cpp
        ../src/strategy/strategy.cpp
        ../src/infrastructure/events.cpp
        ../src/infrastructure/portfolio.cpp
        ../src/infrastructure/execution.cpp)

add_library(backtester_libs ${BACKTEST_HEADERS} ${BACKTEST_SRCS})