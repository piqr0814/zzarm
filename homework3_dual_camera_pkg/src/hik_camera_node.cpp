#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include "MvCameraControl.h"
#include <cstring>
#include <vector>

class HikCameraNode : public rclcpp::Node {
public:
    HikCameraNode() : Node("hik_camera_node"), handle_(nullptr), is_grabbing_(false) {
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/camera/image_raw", 10);
        if (!initCamera()) {
            RCLCPP_ERROR(this->get_logger(), "海康相机初始化失败！");
            return;
        }
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(33),
            std::bind(&HikCameraNode::grabAndPublish, this));
        RCLCPP_INFO(this->get_logger(), "海康相机节点已启动，正在发布 /camera/image_raw ...");
    }
    ~HikCameraNode() { closeCamera(); }

private:
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    void* handle_;
    bool is_grabbing_;

    bool initCamera() {
        int nRet = MV_OK;
        MV_CC_DEVICE_INFO_LIST stDeviceList;
        memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
        nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
        if (MV_OK != nRet || stDeviceList.nDeviceNum == 0) {
            RCLCPP_ERROR(this->get_logger(), "未发现海康相机设备！错误码: 0x%x", nRet);
            return false;
        }
        if (MV_CC_CreateHandle(&handle_, stDeviceList.pDeviceInfo[0]) != MV_OK) return false;
        if (MV_CC_OpenDevice(handle_) != MV_OK) return false;
        MV_CC_SetEnumValue(handle_, "TriggerMode", 0);
        MV_CC_SetEnumValue(handle_, "PixelFormat", PixelType_Gvsp_BGR8_Packed);
        if (MV_CC_StartGrabbing(handle_) != MV_OK) return false;
        is_grabbing_ = true;
        return true;
    }

    void grabAndPublish() {
        if (!is_grabbing_) return;
        MV_FRAME_OUT stImageInfo;
        memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT));
        int nRet = MV_CC_GetImageBuffer(handle_, &stImageInfo, 1000);
        if (nRet == MV_OK) {
            auto msg = sensor_msgs::msg::Image();
            msg.header.stamp = this->get_clock()->now();
            msg.header.frame_id = "hik_camera_optical_frame";
            msg.height = stImageInfo.stFrameInfo.nHeight;
            msg.width = stImageInfo.stFrameInfo.nWidth;
            if (stImageInfo.stFrameInfo.enPixelType == PixelType_Gvsp_Mono8) {
                msg.encoding = "mono8";
                msg.step = msg.width * 1;
            } else {
                msg.encoding = "bgr8";
                msg.step = msg.width * 3;
            }
            msg.is_bigendian = false;
            size_t data_size = msg.step * msg.height;
            msg.data.resize(data_size);
            memcpy(msg.data.data(), stImageInfo.pBufAddr, data_size);
            publisher_->publish(msg);
            MV_CC_FreeImageBuffer(handle_, &stImageInfo);
        } else {
            RCLCPP_WARN(this->get_logger(), "获取图像超时，错误码: 0x%x", nRet);
        }
    }

    void closeCamera() {
        if (handle_ != nullptr) {
            if (is_grabbing_) { MV_CC_StopGrabbing(handle_); is_grabbing_ = false; }
            MV_CC_CloseDevice(handle_);
            MV_CC_DestroyHandle(handle_);
            handle_ = nullptr;
            RCLCPP_INFO(this->get_logger(), "海康相机已安全停止并释放资源。");
        }
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<HikCameraNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
