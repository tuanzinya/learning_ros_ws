#!/usr/bin/env python3
import rclpy
import psutil
import platform
from topic_practice.msg import SystemStatus
from rclpy.node import Node

class SysStatusPub(Node):
    def __init__(self, node_name):
        super().__init__(node_name)
        self.status_pub_ = self.create_publisher(
            SystemStatus, 'sys_status', 10
        )
        self.timer = self.create_timer(1.0, self.timer_callback)
        
    def timer_callback(self):
        cpu_percent = psutil.cpu_percent()
        memory_info = psutil.virtual_memory()
        net_io_counters = psutil.net_io_counters()
        """
        builtin_interfaces/Time stamp # 记录时间戳
        string host_name # 主机名字
        float32 cpu_percent # CPU使用率
        float32 memory_percent # 内存使用量
        float32 memory_total # 内存总量
        float32 memory_avaiable # 内存余量
        float64 net_sent # 网络发送数据总量 
        float64 net_recv # 网络接收数据总量 
        """ 
        msg = SystemStatus()
        msg.stamp = self.get_clock().now().to_msg()
        msg.host_name = platform.node()
        msg.cpu_percent = cpu_percent
        msg.memory_percent = float(memory_info.percent)
        msg.memory_total= float(memory_info.total)
        msg.memory_avaiable = float(memory_info.available)
        msg.net_sent = float(net_io_counters.bytes_sent / 1024 / 1024)
        msg.net_recv = float(net_io_counters.bytes_recv / 1024 / 1024) 

        self.get_logger().info(f"发布:{str(msg)}")
        self.status_pub_.publish(msg)
        
def main():
    rclpy.init()
    node = SysStatusPub('sys_status_pub')
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()
