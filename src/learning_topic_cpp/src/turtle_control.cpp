#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include <iostream>
#include <chrono>
#include <cmath>

using namespace std::chrono_literals;

class TurtleControlNode: public rclcpp::Node
{
private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_;

    double target_x_{1.0};
    double target_y_{1.0};
    double alpha_{1.0}; //比例系数
    double max_speed_{3.0}; //最大速度
public:
    TurtleControlNode() : Node("turtle_control") {}
    explicit TurtleControlNode(const std::string& name) : Node(name)
    {
        pub_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);
        sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, 
            std::bind(&TurtleControlNode::on_pose_received, this, std::placeholders::_1));
        // timer_ = this->create_wall_timer(1000ms, std::bind(&TurtleControlNode::timer_callback, this));
    }

    void on_pose_received(const turtlesim::msg::Pose::SharedPtr pose)
    {  
        //TODO
        //1. 获取当前位置
        double cur_x{ pose->x };
        double cur_y{ pose->y };
        RCLCPP_INFO(this->get_logger(), "当前位置:x = %lf, y = %lf", cur_x, cur_y);
        
        //2. 计算当前海龟位置根目标位置之间的距离差和角度差
        double distance{ std::sqrt(
            (target_x_ - cur_x) * (target_x_ - cur_x) +
            (target_y_ - cur_y) * (target_y_ - cur_y) 
        ) };
        double angle{ std::atan2((target_y_ - cur_y), (target_x_ - cur_x)) - pose->theta };

        //3. 控制策略
        auto msg = geometry_msgs::msg::Twist();
        if(distance > 0.1)
        {
            if(fabs(angle) > 0.2)
                msg.angular.z = fabs(angle);
            else
                msg.linear.x = alpha_ * distance;
        }

        //4. 限制线速度最大值
        if (msg.linear.x > max_speed_)
            msg.linear.x = max_speed_;

        pub_->publish(msg);
    }
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleControlNode>("turtle_control");
    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}
