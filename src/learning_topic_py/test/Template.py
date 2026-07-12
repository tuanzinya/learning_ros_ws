import rclpy
from rclpy.node import Node

class NovelPubNode(Node):
    def __init__(self, node_name):
        super().__init__(node_name)
        self.get_logger().info(f'{node_name}, 启动！')

def main():
    rclpy.init()
    node = NovelPubNode('novel_pub')
    rclpy.spin(node)
    rclpy.shutdown()

