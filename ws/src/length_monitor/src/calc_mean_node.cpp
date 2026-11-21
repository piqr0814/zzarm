#include <memory>

#include "example_interfaces/msg/float64.hpp"
#include "example_interfaces/msg/float64_multi_array.hpp"
#include "length_monitor/stats.hpp"
#include "rclcpp/rclcpp.hpp"

class CalcMeanNode : public rclcpp::Node {
public:
  CalcMeanNode() : Node("calc_mean_node") {
    pub_ = this->create_publisher<example_interfaces::msg::Float64>("/length_mean", 10);
    sub_ = this->create_subscription<example_interfaces::msg::Float64MultiArray>(
      "/input_lengths", 10,
      std::bind(&CalcMeanNode::on_msg, this, std::placeholders::_1));
  }

private:
  void on_msg(const example_interfaces::msg::Float64MultiArray::SharedPtr msg) {
    auto avg = meanLength(msg->data);
    if (!avg.has_value()) {
      RCLCPP_WARN(this->get_logger(), "Received empty array; skipping publish.");
      return;
    }
    example_interfaces::msg::Float64 out;
    out.data = *avg;
    RCLCPP_INFO(this->get_logger(), "Publishing mean: %.4f", out.data);
    pub_->publish(out);
  }

  rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr pub_;
  rclcpp::Subscription<example_interfaces::msg::Float64MultiArray>::SharedPtr sub_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CalcMeanNode>());
  rclcpp::shutdown();
  return 0;
}
