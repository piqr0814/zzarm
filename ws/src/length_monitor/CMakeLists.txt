cmake_minimum_required(VERSION 3.8)
project(length_monitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(example_interfaces REQUIRED)

add_library(stats_core src/stats.cpp)
target_include_directories(stats_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

add_executable(pub_lengths_node src/pub_lengths_node.cpp)
target_link_libraries(pub_lengths_node stats_core)
ament_target_dependencies(pub_lengths_node rclcpp example_interfaces)

add_executable(calc_mean_node src/calc_mean_node.cpp)
target_link_libraries(calc_mean_node stats_core)
ament_target_dependencies(calc_mean_node rclcpp example_interfaces)

install(TARGETS
  stats_core
  pub_lengths_node
  calc_mean_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()
