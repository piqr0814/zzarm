# 【第三次作业】工业相机 ROS 2 图像采集功能包（dual_camera_pkg）

# dual_camera_pkg — 工业相机 ROS 2 图像采集功能包

本功能包基于 ROS 2（C++ rclcpp 与 sensor_msgs）开发，同时兼容海康机器人（Hikrobot）与迈德威视（MindVision）两款工业相机，实现连续取流、Bayer 原始像素转码（bgr8）、标准图像话题发布以及节点退出时的安全硬件资源释放。

## 1. 功能包目录结构

    dual_camera_pkg/
    ├── CMakeLists.txt              # CMake 构建规则（链接海康与迈德威视动态库）
    ├── package.xml                 # ROS 2 功能包依赖声明
    ├── README.md                   # 项目说明、接口规格与实机采集留样记录
    ├── sample_raw.png              # 实机采集无损原图留样（PNG 无损格式）
    └── src/
        ├── hik_camera_node.cpp     # 海康工业相机采集与转码发布节点
        └── mv_camera_node.cpp      # 迈德威视工业相机采集与发布节点

## 2. 硬件型号、采集接口与环境依赖

* **兼容相机型号**：
  1. 海康机器人（Hikrobot）MV-CS016-10UC（USB 3.0 彩色工业相机，默认 BayerRG8 转码输出 bgr8）
  2. 迈德威视（MindVision）系列工业相机（通过 MVSDK 回调/主动抓帧接口采集）
* **采集接口与依赖**：
  * 底层驱动 SDK：海康 MVS SDK（libMvCameraControl.so，路径 /opt/MVS）与迈德威视 Linux SDK（libMVSDK.so）
  * ROS 2 依赖：rclcpp、sensor_msgs、rqt_image_view

## 3. 构建与启动方式

### 3.1 编译功能包
    cd ~/ros2_work
    colcon build --packages-select dual_camera_pkg
    source ~/ros2_work/install/setup.bash

### 3.2 启动采集节点与可视化
    # 1. 启动海康相机节点（或运行 mv_camera_node 启动迈德威视相机）
    source ~/ros2_work/install/setup.bash
    ros2 run dual_camera_pkg hik_camera_node

    # 2. 新开终端查看实时画面
    ros2 run rqt_image_view rqt_image_view --ros-args -r image:=/camera/image_raw

### 3.3 安全退出与重启恢复
在运行节点的终端按下 Ctrl + C，节点析构函数自动调用 MV_CC_StopGrabbing、MV_CC_CloseDevice 与 MV_CC_DestroyHandle 停止取流并释放相机句柄；释放后可直接重新执行 ros2 run 命令恢复取流。

## 4. 发布话题规格、QoS 与时间戳来源

* **发布话题名称**：/camera/image_raw
* **消息类型**：sensor_msgs/msg/Image（未标注原始图像）
* **QoS 配置**：ROS 2 默认传感器/图像发布配置（Reliability: RELIABLE，Durability: VOLATILE，History: KEEP_LAST，队列深度 Depth = 10）
* **时间戳来源**：每次成功取帧并完成像素转码后，调用 ROS 2 节点时钟接口 this->get_clock()->now() 实时生成（采用系统 WALL_TIME 墙钟时间写入 header.stamp）

## 5. 实机同帧留样参数

* **留样原图文件**：sample_raw.png
* **图像保存格式**：PNG（无损压缩整数图，未经过拉伸或有损 JPEG 压缩）
* **同帧元数据记录**（通过 ros2 topic echo /camera/image_raw --once --no-arr 获取）：

| 参数字段 | 实机采集数值 | 说明 |
| :--- | :--- | :--- |
| **Topic Name** | /camera/image_raw | 发布话题名 |
| **width × height** | 1440 × 1080 | 图像分辨率（宽 × 高，像素） |
| **encoding** | bgr8 | 发布编码格式 |
| **保存格式** | PNG | 无损原图保存格式（sample_raw.png） |
| **header.stamp.sec** | 1795862868 | 时间戳（秒，来自 node 时钟） |
| **header.stamp.nanosec** | 460845480 | 时间戳（纳秒，来自 node 时钟） |
| **header.frame_id** | hik_camera_optical_frame | 相机光学坐标系名称 |
| **step** | 4320 | 单行字节数（1440 × 3） |
