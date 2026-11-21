#include <chrono>
#include <memory>
#include <vector>

#include "example_interfaces/msg/float64_multi_array.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class PubLengthsNode : public rclcpp::Node {
public:
  PubLengthsNode() : Node("pub_lengths_node"), idx_(0) {
    pub_ = this->create_publisher<example_interfaces::msg::Float64MultiArray>("/input_lengths", 10);
    batches_ = {
      {1.0, 2.0, 3.0},
      {},
      {4.0, 6.0},
      {10.0, 10.0, 10.0}
    };
    timer_ = this->create_wall_timer(1000ms, std::bind(&PubLengthsNode::on_timer, this));
  }

private:
  void on_timer() {
    example_interfaces::msg::Float64MultiArray msg;
    msg.data = batches_[idx_ % batches_.size()];
    RCLCPP_INFO(this->get_logger(), "Publishing batch #%zu (size=%zu)", idx_ % batches_.size(), msg.data.size());
    pub_->publish(msg);
    ++idx_;
  }

  rclcpp::Publisher<example_interfaces::msg::Float64MultiArray>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::vector<std::vector<double>> batches_;
  std::size_t idx_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PubLengthsNode>());
  rclcpp::shutdown();
  return 0;
}
