#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include "learning_srv/srv/patrol.hpp"
#include "rcl_interfaces/msg/set_parameters_result.hpp"
#include <chrono>
#include <cmath>
#include <string>

using namespace std::chrono_literals;

class TurtleControlNode: public rclcpp::Node
{
private:
    using Patrol = learning_srv::srv::Patrol;
    using SetParametersResult = rcl_interfaces::msg::SetParametersResult;

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_; 
    rclcpp::Service<Patrol>::SharedPtr server_;
    OnSetParametersCallbackHandle::SharedPtr parameter_callback_handler_;

    double target_x_{1.0};
    double target_y_{1.0};
    double alpha_{1.0}; //比例系数
    double max_speed_{3.0}; //最大速度
public:
    TurtleControlNode() : Node("patrol_tut_server") {}
    explicit TurtleControlNode(const std::string& name) : Node(name)
    {
        // 参数声明
        this->declare_parameter<double>("alpha", 1.0); // 公共参数alpha
        this->declare_parameter<double>("max_speed", 3.0); // 公共参数max_speed
        this->declare_parameter<bool>(std::string(this->get_name()) + "/private_param", false); //声明私有参数private_param
        // 获取参数
        this->get_parameter("alpha", alpha_);
        this->get_parameter("max_speed", max_speed_);
        // 更改参数
        this->set_parameter(rclcpp::Parameter("alpha", 2.0));
        

        parameter_callback_handler_ = this->add_on_set_parameters_callback( // 绑定参数更新回调函数
            std::bind(&TurtleControlNode::parameters_callback, this, std::placeholders::_1));
        server_ = this->create_service<Patrol>("patrol", 
            [&](const Patrol::Request::SharedPtr req, Patrol::Response::SharedPtr res) -> void
            {
                if (req->target_x > 0 && req->target_x < 12.0f &&
                    req->target_y > 0 && req->target_y < 12.0f)
                    {                        
                        this->target_x_ = req->target_x;
                        this->target_y_ = req->target_y;
                        res->result = Patrol::Response::SUCCESS;
                    }
                else
                    res->result = Patrol::Response::FAIL;
                // 添加参数更新回调，额外调用回调函数
            });
        pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, 
            std::bind(&TurtleControlNode::on_pose_received, this, std::placeholders::_1));
        RCLCPP_INFO(this->get_logger(), "\n服务端已启动\n服务端已启\n服务端已");
    }

    /// @brief 参数更新回调函数
    /// @param parameters 获取的参数值
    /// @return 返回参数更新结果，成功true，失败false
    SetParametersResult parameters_callback(const std::vector<rclcpp::Parameter> & parameters)
    {
        SetParametersResult result;
        result.successful = true;
        for (const auto & parameter : parameters) 
        {   
            RCLCPP_INFO(this->get_logger(), "更新参数的值%s = %.1f", parameter.get_name().c_str(), parameter.as_double());
            if (parameter.get_name() == "alpha")
                alpha_ = parameter.as_double();            
            if (parameter.get_name() == "max_speed")
                max_speed_ = parameter.as_double();
        }
        return result;
    }

    /// @brief 控制回调函数
    /// @param pose 小海龟当前位姿
    void on_pose_received(const turtlesim::msg::Pose::SharedPtr pose)
    {  
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
    auto node = std::make_shared<TurtleControlNode>("patrol_tut_server");
    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}
