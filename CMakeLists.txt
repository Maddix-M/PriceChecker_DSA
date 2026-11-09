cmake_minimum_required(VERSION 3.14)
project(PriceFilter CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Timing numbers only mean something with optimizations on.
# In CLion: Settings > Build, Execution, Deployment > CMake > add a "Release" profile.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(price_filter_core STATIC
    src/Utils.cpp
    src/DataGenerator.cpp
    src/DataLoader.cpp
    src/MergeSort.cpp
    src/QuickSort.cpp
    src/Benchmark.cpp
    src/ProductFilter.cpp
    src/Menu.cpp
)
target_include_directories(price_filter_core PUBLIC src)

add_executable(price_filter src/main.cpp)
target_link_libraries(price_filter price_filter_core)

add_executable(run_tests tests/tests.cpp)
target_link_libraries(run_tests price_filter_core)
