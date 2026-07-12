#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/QuadWord.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2_ros/transform_broadcaster.hpp"
#include <memory>
#include <chrono>

using namespace std::chrono_literals;

class DynamicTFBroadcasterNode: public rclcpp::Node
{
private:
    std::shared_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
    rclcpp::TimerBase::SharedPtr timer_;
public:
    DynamicTFBroadcasterNode() : Node("dynamic_tf_broadcaster_cpp")
    {
        broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        timer_ = this->create_wall_timer(100ms, std::bind(&DynamicTFBroadcasterNode::time_callback, this));
    }

    void time_callback()
    {
        geometry_msgs::msg::TransformStamped trans;
        trans.header.stamp = this->get_clock()->now();
        trans.header.frame_id = "map";
        trans.child_frame_id = "base_link";
        trans.transform.translation.x = 2.0;
        trans.transform.translation.y = 3.0;
        trans.transform.translation.z = 0.0;
        tf2::Quaternion q;
        q.setRPY(0.0, 0.0, 30 * M_PI / 180.0);
        trans.transform.rotation = tf2::toMsg(q);
        broadcaster_->sendTransform(trans);
    }
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<DynamicTFBroadcasterNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}


