#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from tf2_ros import StaticTransformBroadcaster # 静态坐标发布器
from geometry_msgs.msg import TransformStamped # 消息接口
from tf_transformations import quaternion_from_euler # 欧拉角转四元数
import math

class DynamicTFBroadcaster(Node):
    def __init__(self):
        super().__init__('dynamic_tf_broadcaster')
        self.static_broadcaster_ = StaticTransformBroadcaster(self)
        self.timer_ = self.create_timer(1.0, self.publish_dynamic_tf)
        
    def publish_dynamic_tf(self):
        """
        发布TF camera_link 到 bottle_link的关系
        """
        transform = TransformStamped()
        transform.header.frame_id = 'camera_link'
        transform.child_frame_id = 'bottle_link'
        transform.header.stamp = self.get_clock().now().to_msg()
        
        transform.transform.translation.x = 0.2
        transform.transform.translation.y = 0.3
        transform.transform.translation.z = 0.5

        # 欧拉角转四元数 q = x, y, z, w
        q = quaternion_from_euler(0, 0, 0)
        transform.transform.rotation.x = q[0] 
        transform.transform.rotation.y = q[1] 
        transform.transform.rotation.z = q[2] 
        transform.transform.rotation.w = q[3] 
        
        self.static_broadcaster_.sendTransform(transform)
        self.get_logger().info(f'发布TF:{transform}')

def main():     
    rclpy.init()
    node = DynamicTFBroadcaster()
    rclpy.spin(node)
    rclpy.shutdown()
    
if __name__ == '__main__':
    main()
