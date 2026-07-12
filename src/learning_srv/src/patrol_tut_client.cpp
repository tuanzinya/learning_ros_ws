#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include "learning_srv/srv/patrol.hpp"
#include <chrono>
#include <ctime>
#include "rcl_interfaces/msg/parameter.hpp"
#include "rcl_interfaces/msg/parameter_value.hpp"
#include "rcl_interfaces/msg/parameter_type.hpp"
#include "rcl_interfaces/srv/set_parameters.hpp"

using namespace std::chrono_literals;

class PatrolClientNode: public rclcpp::Node
{
private:
    using Patrol = learning_srv::srv::Patrol;
    using SetParam = rcl_interfaces::srv::SetParameters;

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Client<Patrol>::SharedPtr cli_;

    void time_callback()
    {
        // 等待服务上线
        while(!cli_->wait_for_service(1s))
        {
            if(!rclcpp::ok())
            {
                RCLCPP_ERROR(this->get_logger(), "等待上线失败！");
                return;
            }
            RCLCPP_INFO(this->get_logger(), "等待服务上线中...");
        }
        
        // 构造请求的对象
        auto res = std::make_shared<Patrol::Request>();
        res->target_x = std::rand() % 15;
        res->target_y = std::rand() % 15;
        RCLCPP_INFO(this->get_logger(), "预设目标点:(%.lf, %.lf)", res->target_x, res->target_y);
        
        // 发送请求
        cli_->async_send_request(res, 
            [&](rclcpp::Client<Patrol>::SharedFuture res_fut)->void
            {
                auto response = res_fut.get();
                if (response->result == Patrol::Response::SUCCESS)
                    RCLCPP_INFO(this->get_logger(), "请求巡逻目标点成功！");
                else
                    RCLCPP_INFO(this->get_logger(), "请求巡逻目标点失败！");

            });
    }

public:
    PatrolClientNode() : Node("patrol_tut_client") 
    {
        std::srand(std::time(NULL));
        cli_ = this->create_client<Patrol>("patrol");
        timer_ = this->create_wall_timer(10s, std::bind(&PatrolClientNode::time_callback, this));
    }

    /// @brief 创建客户端发送请求
    /// @param param 更新后的参数值
    /// @return 返回更新参数服务端的结果的指针
    SetParam::Response::SharedPtr call_set_parameter(rcl_interfaces::msg::Parameter& param)
    {
        auto param_cli = this->create_client<SetParam>("/patrol_tut_server/set_parameters");
        while(!param_cli->wait_for_service(1s))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(this->get_logger(), "等待上线失败！");
                return nullptr;
            }
            RCLCPP_INFO(this->get_logger(), "等待服务上线中...");
        }

        auto req = std::make_shared<SetParam::Request>();
        req->parameters.push_back(param);

        auto fut = param_cli->async_send_request(req);
        rclcpp::spin_until_future_complete(this->get_node_base_interface(), fut);
        auto res = fut.get();
        return res;
    }

    /// @brief 更新参数
    /// @param alpha 更新后的值
    void update_server_param_alpha(double alpha)
    {
        // 1.创建一个参数对象
        auto param = rcl_interfaces::msg::Parameter();
        param.name = "alpha";
        // 2.创建参数值
        auto val = rcl_interfaces::msg::ParameterValue();
        val.type =rcl_interfaces::msg::ParameterType::PARAMETER_DOUBLE;
        val.double_value = alpha;
        param.value = val;
        // 3.请求参数并处理
        auto res = this->call_set_parameter(param); //call_set_parameter发送参数更改请求
        if (res == nullptr)
        {
            RCLCPP_INFO(this->get_logger(), "参数更新失败！");
            return;
        }
        for (auto result : res->results)
        {
            if(result.successful == false)
                RCLCPP_INFO(this->get_logger(), "参数更新失败：%s", result.reason.c_str());
            else
                RCLCPP_INFO(this->get_logger(), "参数更新成功！");
        }
    }

};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PatrolClientNode>();
    node->update_server_param_alpha(4.0);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
