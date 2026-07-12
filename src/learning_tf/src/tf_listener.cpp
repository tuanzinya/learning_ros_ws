#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/QuadWord.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2_ros/transform_listener.hpp"
#include "tf2_ros/buffer.hpp"
#include "tf2/utils.hpp" // 提供四元数转欧拉角
#include <memory>
#include <chrono>

using namespace std::chrono_literals;

class TFListenerNode: public rclcpp::Node
{
private:
    std::shared_ptr<tf2_ros::TransformListener> listener_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::shared_ptr<tf2_ros::Buffer> buf_;
public:
    TFListenerNode() : Node("tf_listener_cpp")
    {
        buf_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        listener_ = std::make_shared<tf2_ros::TransformListener>(*buf_, this);
        timer_ = this->create_wall_timer(100ms, std::bind(&TFListenerNode::getTransform, this));
    }

    void getTransform()
    {
        try{
            // 到buf_里查询坐标关系
            const auto transform = buf_->lookupTransform("base_link", "target_point", get_clock()->now(), 
            rclcpp::Duration::from_seconds(1.0f));
            // 获取查询结果
            auto translation = transform.transform.translation;
            auto rotation = transform.transform.rotation;
            double y, p, r;
            tf2::getEulerYPR(rotation, y, p, r);
            RCLCPP_INFO(get_logger(), "平移:%.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
            RCLCPP_INFO(get_logger(), "旋转:%.2f, %.2f, %.2f", y, p, r);
        }
        catch(const std::exception& e)
        {
            RCLCPP_WARN(get_logger(), "%s", e.what());
        }
    }
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TFListenerNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}


