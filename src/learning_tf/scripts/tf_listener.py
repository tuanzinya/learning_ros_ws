#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from rclpy.time import Time
from rclpy.time import Duration
from tf2_ros import TransformListener, Buffer # 静态坐标发布器
from tf_transformations import euler_from_quaternion # 欧拉角转四元数


class StaticTFBroadcaster(Node):
    def __init__(self):
        super().__init__('tf_listener')
        self.buffer_ = Buffer()
        self.listener_ = TransformListener(self.buffer_, self)
        self.timer_ = self.create_timer(0.5, self.get_transform)
        
    def get_transform(self):
        """
        定时获取坐标关系
        """
        try:
            result = self.buffer_.lookup_transform('base_link', 'bottle_link',
                Time(seconds = 0), Duration(seconds = 1.0))
            transform = result.transform
            self.get_logger().info(f"平移:{transform.translation}")
            self.get_logger().info(f"旋转:{transform.rotation}")
            rotation_euler = euler_from_quaternion([
                transform.rotation.x,
                transform.rotation.y,
                transform.rotation.z,
                transform.rotation.w
            ])
            self.get_logger().info(f"旋转RPY:{rotation_euler}")
        except Exception as e:
            self.get_logger().warn(f"获取坐标变换失败,原因:{str(e)}")
            

def main():     
    rclpy.init()
    node = StaticTFBroadcaster()
    rclpy.spin(node)
    rclpy.shutdown()
    
if __name__ == '__main__':
    main()
