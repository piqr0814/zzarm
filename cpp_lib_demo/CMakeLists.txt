cmake_minimum_required(VERSION 3.16)
project(stats_lib_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(stats_lib src/stats.cpp)
target_include_directories(stats_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(run_tests src/main.cpp)
target_link_libraries(run_tests PRIVATE stats_lib)
