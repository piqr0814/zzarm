cmake_minimum_required(VERSION 3.8)
project(dual_camera_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找 ROS 2 依赖
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

# ==================== 1. 配置海康 MVS SDK 路径 ====================
set(HIK_SDK_DIR "/opt/MVS")
include_directories(${HIK_SDK_DIR}/include)
link_directories(${HIK_SDK_DIR}/lib/64)

# 编译海康相机节点
add_executable(hik_camera_node src/hik_camera_node.cpp)
ament_target_dependencies(hik_camera_node rclcpp sensor_msgs)
target_link_libraries(hik_camera_node MvCameraControl)

# ==================== 2. 配置迈德威视 SDK 路径 ====================
# 注意：将下方路径替换为你实际解压 linuxSDK_V2.1.0.50 的路径！
set(MV_SDK_DIR "/home/zza/Downloads/mindvision_sdk")
include_directories(${MV_SDK_DIR}/include)
link_directories(${MV_SDK_DIR}/lib/x64)

# 编译迈德威视相机节点
add_executable(mv_camera_node src/mv_camera_node.cpp)
ament_target_dependencies(mv_camera_node rclcpp sensor_msgs)
target_link_libraries(mv_camera_node MVSDK)

# 安装可执行文件，供 ros2 run 调用
install(TARGETS
  hik_camera_node
  mv_camera_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()