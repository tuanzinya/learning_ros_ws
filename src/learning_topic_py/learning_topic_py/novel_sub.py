# import subprocess
import espeakng
import rclpy
import threading
import time
from rclpy.node import Node
from example_interfaces.msg import String
from queue import Queue


class NovelSubNode(Node):
    def __init__(self, node_name):
        super().__init__(node_name)
        self.get_logger().info(f'{node_name}, 启动！')
        self.novels_queue_ = Queue()
        self.novel_sub_ = self.create_subscription(String, 'novel', self.novel_callback, 10)
        self.sp_th_ = threading.Thread(target = self.speech_thread)
        self.sp_th_.start()
        
        
    def novel_callback(self, msg):
        self.novels_queue_.put(msg.data)
        pass

    def speech_thread(self):
        speaker = espeakng.Speaker()
        speaker.voice = 'zh'
        
        while rclpy.ok():
            if self.novels_queue_.qsize() > 0:
                text = self.novels_queue_.get()
                self.get_logger().info(f'朗读：{text}')
                speaker.say(text)
                speaker.wait()
            else:
                # 让当前线程休眠
                time.sleep(1)
        

def main():
    rclpy.init()
    node = NovelSubNode('novel_sub')
    rclpy.spin(node)
    rclpy.shutdown()

