cmake_minimum_required(VERSION 3.10)

project(Benchmark)

set(Benchmark_SRCS
    src/main.cpp
    src/Timer.cpp
    src/Timer.h
    src/Sorting.h
)

add_executable(Benchmark
    ${Benchmark_SRCS}
)

# Tests
set(BenchmarkUT_SRCS
    tests/testMain.cpp
    src/Timer.cpp
    src/Sorting.h
)

add_executable(BenchmarkUTRunner
    ${BenchmarkUT_SRCS}
)