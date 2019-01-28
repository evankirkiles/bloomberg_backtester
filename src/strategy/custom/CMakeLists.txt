project(strategy_files)

# Initialize project sources here
set(STRAT_HEADERS
        src/basic_algo.hpp
        src/momentum1.hpp)
set(STRAT_SRCS
        basic_algo.cpp
        momentum1.cpp)

add_library(strategy_files ${STRAT_HEADERS} ${STRAT_SRCS})