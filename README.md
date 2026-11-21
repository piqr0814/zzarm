# C++ & ROS 2 作业说明

## 1. 工程结构
- cpp_lib_demo/：8.1 C++17 与 CMake 统计库及测试。
- ws/src/length_monitor/：8.2 ROS 2 功能包（发布节点与均值节点）。

## 2. 构建与运行方法
### 8.1 C++ / CMake
cd ~/ros2_work/cpp_lib_demo
cmake -B build
cmake --build build
./build/run_tests

### 8.2 ROS 2 节点（每个终端先执行 export ROS_LOCALHOST_ONLY=1）
source /opt/ros/humble/setup.bash
cd ~/ros2_work/ws
colcon build

终端 1：
export ROS_LOCALHOST_ONLY=1
source /opt/ros/humble/setup.bash
source ~/ros2_work/ws/install/setup.bash
ros2 run length_monitor pub_lengths_node

终端 2：
export ROS_LOCALHOST_ONLY=1
source /opt/ros/humble/setup.bash
source ~/ros2_work/ws/install/setup.bash
ros2 run length_monitor calc_mean_node

终端 3：
export ROS_LOCALHOST_ONLY=1
source /opt/ros/humble/setup.bash
ros2 topic echo /length_mean

## 3. 节点、话题与输入输出
- 发布节点 pub_lengths_node 发布 /input_lengths（Float64MultiArray），循环发送多组长度数据及空数组。
- 均值节点 calc_mean_node 订阅 /input_lengths，发布 /length_mean（Float64）。非空计算均值并发布；空数组跳过发布并打印提示。

## 4. 为什么空数组不能返回 0
空数组代表“没有数据”，不是“测出来的长度就是 0”。如果返回 0 或发布 0，会把空数据当成真实测量值 0 算进结果里，导致统计出错。

## 5. 遇到的问题与解决方法
- 拉取 ROS 2 公钥时遇到网络和目录权限问题：安装 dirmngr 并用临时目录导入公钥后解决。
- 编译 length_monitor 时 stats.hpp 找不到：把头文件路径改成 length_monitor/stats.hpp 后重新 colcon build 通过。
