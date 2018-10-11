project(strategy_files)

# Initialize project sources here
set(STRAT_HEADERS
        src/basic_algo.hpp)
set(STRAT_SRCS
        basic_algo.cpp)

add_library(strategy_files ${STRAT_HEADERS} ${STRAT_SRCS})