#include <QApplication>
#include <QLabel>
#include <QString>
#include <sstream>
#include <thread>
#include "rclcpp/rclcpp.hpp"
#include "topic_practice/msg/system_status.hpp"

using SysStatus = topic_practice::msg::SystemStatus;

class SysStatusDisplay:
 public rclcpp::Node
{
private:
    rclcpp::Subscription<SysStatus>::SharedPtr sub_;
    QLabel* label_;
public: 
    ~SysStatusDisplay()
    {
        delete label_;
    }

    SysStatusDisplay() : Node("sys_status_display")
    {
        label_ = new QLabel();
        sub_ = create_subscription<SysStatus>("sys_status", 10, 
            [&](const SysStatus::SharedPtr msg)->void
            {
                label_->setText(get_qstr_from_msg(msg));
            });
        label_->setText(get_qstr_from_msg(std::make_shared<SysStatus>()));
        label_->show();
    }


    /*
        msg = SystemStatus()
        msg.stamp = self.get_clock().now().to_msg()
        msg.host_name = platform.node()
        msg.cpu_percent = cpu_percent
        msg.memory_percent = float(memory_info.percent)
        msg.memory_total= float(memory_info.total)
        msg.memory_avaiable = float(memory_info.available)
        msg.net_sent = float(net_io_counters.bytes_sent / 1024 / 1024)
        msg.net_recv = float(net_io_counters.bytes_recv / 1024 / 1024) */

    QString get_qstr_from_msg(const SysStatus::SharedPtr msg)
    {
        std::stringstream show_str;
        show_str << "===========================系统提供状态可视化工具===========================\n"
                << "数 据 时 间:\t " << msg->stamp.sec << "\ts\n"
                << "主 机 名 字:\t " << msg->host_name << "\t\n" 
                << "CPU 使 用 率:\t " << msg->cpu_percent << "\t%\n" 
                << "内 存 使 用 率:\t " << msg->memory_percent << "\t%\n" 
                << "内 存 总 大 小:\t " << msg->memory_total << "\tMB\n" 
                << "内 存 剩 余 量:\t " << msg->memory_avaiable << "\tMB\n" 
                << "网 络 发 送 量:\t " << msg->net_sent << "\tMB\n"
                << "网 络 接 收 量:\t " << msg->net_recv << "\tMB\n"
                << "=========================================================================\n";
        return QString::fromStdString(show_str.str());
    }
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    QApplication app(argc, argv);
    auto node = std::make_shared<SysStatusDisplay>();
    std::thread spin_th([&]()
    {
        rclcpp::spin(node);
    });
    spin_th.detach();
    app.exec();
    if(spin_th.joinable())
        spin_th.join(); 

    return 0;
}

