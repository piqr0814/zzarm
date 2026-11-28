#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include "CameraApi.h"
#include <cstring>

class MindVisionCameraNode : public rclcpp::Node {
public:
    MindVisionCameraNode() : Node("mv_camera_node"), hCamera_(-1), pRgbBuffer_(nullptr) {
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/camera/image_raw", 10);
        if (!initCamera()) {
            RCLCPP_ERROR(this->get_logger(), "迈德威视相机初始化失败！");
            return;
        }
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(33),
            std::bind(&MindVisionCameraNode::grabAndPublish, this));
        RCLCPP_INFO(this->get_logger(), "迈德威视相机节点已启动，正在发布 /camera/image_raw ...");
    }
    ~MindVisionCameraNode() { closeCamera(); }

private:
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    int hCamera_;
    unsigned char* pRgbBuffer_;
    tSdkCameraCapbility tCapability_;

    bool initCamera() {
        int iCameraCounts = 1;
        tSdkCameraDevInfo tCameraEnumList;
        CameraSdkInit(1);
        if (CameraEnumerateDevice(&tCameraEnumList, &iCameraCounts) != CAMERA_STATUS_SUCCESS || iCameraCounts == 0) {
            RCLCPP_ERROR(this->get_logger(), "未找到迈德威视相机！");
            return false;
        }
        if (CameraInit(&tCameraEnumList, -1, -1, &hCamera_) != CAMERA_STATUS_SUCCESS) {
            RCLCPP_ERROR(this->get_logger(), "迈德威视初始化失败！");
            return false;
        }
        CameraGetCapability(hCamera_, &tCapability_);
        int max_size = tCapability_.sResolutionRange.iWidthMax * tCapability_.sResolutionRange.iHeightMax * 3;
        pRgbBuffer_ = (unsigned char*)malloc(max_size);
        CameraSetTriggerMode(hCamera_, 0);
        CameraSetIspOutFormat(hCamera_, CAMERA_MEDIA_TYPE_BGR8);
        CameraPlay(hCamera_);
        return true;
    }

    void grabAndPublish() {
        if (hCamera_ < 0) return;
        tSdkFrameHead sFrameInfo;
        BYTE* pbyBuffer = nullptr;
        if (CameraGetImageBuffer(hCamera_, &sFrameInfo, &pbyBuffer, 1000) == CAMERA_STATUS_SUCCESS) {
            CameraImageProcess(hCamera_, pbyBuffer, pRgbBuffer_, &sFrameInfo);
            auto msg = sensor_msgs::msg::Image();
            msg.header.stamp = this->get_clock()->now();
            msg.header.frame_id = "mv_camera_optical_frame";
            msg.height = sFrameInfo.iHeight;
            msg.width = sFrameInfo.iWidth;
            msg.encoding = "bgr8";
            msg.is_bigendian = false;
            msg.step = msg.width * 3;
            size_t data_size = msg.step * msg.height;
            msg.data.resize(data_size);
            memcpy(msg.data.data(), pRgbBuffer_, data_size);
            publisher_->publish(msg);
            CameraReleaseImageBuffer(hCamera_, pbyBuffer);
        } else {
            RCLCPP_WARN(this->get_logger(), "迈德威视取帧超时！");
        }
    }

    void closeCamera() {
        if (hCamera_ >= 0) {
            CameraUnInit(hCamera_);
            hCamera_ = -1;
            RCLCPP_INFO(this->get_logger(), "迈德威视相机已安全关闭。");
        }
        if (pRgbBuffer_ != nullptr) { free(pRgbBuffer_); pRgbBuffer_ = nullptr; }
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MindVisionCameraNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
