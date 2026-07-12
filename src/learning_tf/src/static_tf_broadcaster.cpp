#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/QuadWord.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2_ros/static_transform_broadcaster.hpp"
#include <memory>

class StaticTFBroadcasterNode: public rclcpp::Node
{
private:
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> broadcaster_;
public:
    StaticTFBroadcasterNode() : Node("static_tf_broadcaster_cpp")
    {
        broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);
        this->publish_tf();
    }

    void publish_tf()
    {
        geometry_msgs::msg::TransformStamped trans;
        trans.header.stamp = this->get_clock()->now();
        trans.header.frame_id = "map";
        trans.child_frame_id = "target_point";
        trans.transform.translation.x = 5.0;
        trans.transform.translation.y = 3.0;
        trans.transform.translation.z = 0.0;
        tf2::Quaternion q;
        q.setRPY(0.0, 0.0, 60 * M_PI / 180.0);
        trans.transform.rotation = tf2::toMsg(q);
        this->broadcaster_->sendTransform(trans);
    }
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<StaticTFBroadcasterNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}


