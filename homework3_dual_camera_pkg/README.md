# 【第三次作业】工业相机 ROS 2 图像采集功能包（dual_camera_pkg）

## 1. 相机型号
* **海康机器人（Hikrobot）**：MV-CS016-10UC（USB 3.0 彩色工业相机，默认 BayerRG8 转码 bgr8）
* **迈德威视（MindVision）**：兼容支持工业相机系列

## 2. 采集接口与依赖
* **底层 SDK 接口**：海康 MVS SDK（libMvCameraControl.so，路径 /opt/MVS）、迈德威视 Linux SDK（libMVSDK.so）
* **ROS 2 依赖**：rclcpp、sensor_msgs、rqt_image_view

## 3. 构建及启动方式
    # 1. 编译与加载环境
    cd ~/ros2_work && colcon build --packages-select dual_camera_pkg
    source ~/ros2_work/install/setup.bash

    # 2. 启动节点（海康 / 迈德威视二选一）
    ros2 run dual_camera_pkg hik_camera_node
    # ros2 run dual_camera_pkg mv_camera_node

    # 3. 查看图像（新开终端）
    ros2 run rqt_image_view rqt_image_view --ros-args -r image:=/camera/image_raw

## 4. 发布话题与 QoS
* **发布话题**：/camera/image_raw（消息类型：sensor_msgs/msg/Image）
* **QoS 策略**：ROS 2 默认传感器/图像配置（Reliability: RELIABLE，Durability: VOLATILE，History: KEEP_LAST，队列深度 Depth = 10）

## 5. 时间戳来源
* 每次取帧并完成像素转码后，调用 ROS 2 节点时钟接口 this->get_clock()->now() 实时获取系统墙钟时间（WALL_TIME）写入 header.stamp。

## 6. 同帧留样记录（sample_raw.png）
* **保存格式**：PNG（无损原图）
* **分辨率与编码**：1440 × 1080 | bgr8 | step: 4320
* **header.frame_id**：hik_camera_optical_frame
* **header.stamp**：sec: 1795862868 | nanosec: 460845480
