#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from learning_srv.srv import FaceDetector
import face_recognition
import cv2
from ament_index_python.packages import get_package_share_directory
import os 
from cv_bridge import CvBridge
import time # 统计任务处理耗时
from rcl_interfaces.msg import SetParametersResult # 动态参数设置相关


class FaceDetectNode(Node):
    def __init__(self):
        super().__init__('face_detect_node')
        self.service_ = self.create_service(FaceDetector, 'face_detect', self.detect_face_callback)
        self.default_path_ = os.path.join(get_package_share_directory('learning_srv') + '/images/the_Suns.jpg')
        self.bridge_ = CvBridge()
        # 声明参数
        self.declare_parameter('number_of_times_to_upsample', 1) # 声明参数number_of_times_to_upsample
        self.declare_parameter('model', 'hog') # 声明参数model
        self.number_of_times_to_upsample_ = self.get_parameter('number_of_times_to_upsample').value # 获取参数值
        self.model_ = self.get_parameter('model').value # 获取参数值
        self.add_on_set_parameters_callback(self.parameter_callback) # 绑定参数更新回调函数
        # 设置自身节点参数
        # self.set_parameters([rclpy.Parameter('model', rclpy.Parameter.Type.STRING, 'cnn')])
        self.get_logger().info(f"人脸识别服务端已启动！")
        
    def parameter_callback(self, parameters):
        """
        parameter_callback 的 Docstring
        参数更新回调函数
        :param self: self
        :param parameters: 
        """
        for parameter in parameters:
            self.get_logger().info(f"{parameter.name}->{parameter.value}")
            if parameter.name == 'number_of_times_to_upsample':
                self.number_of_times_to_upsample_ = parameter.value
            elif parameter.name == 'model':
                self.model_ = parameter.value
        return SetParametersResult(successful = True) 
                 
    def detect_face_callback(self, request, response):
        """
        detect_face_callback 的 Docstring
        图像识别回调函数
        :param self: self
        :param request: 请求消息
        :param response: 处理后消息
        """
        if request.image.data:
            cv_image = self.bridge_.imgmsg_to_cv2(request.image)
        else:
            cv_image = cv2.imread(self.default_path_)
            self.get_logger().info(f"传入图像为空，使用默认图像！")
            
        start_time = time.time()
        self.get_logger().info(f"加载图像完成，开始识别！")
        # 检测人脸
        face_location = face_recognition.face_locations(cv_image, 
            number_of_times_to_upsample=self.number_of_times_to_upsample_, 
            model=self.model_)
        response.use_time = time.time() - start_time
        response.number = len(face_location)
        for top, right, bottom, left in face_location:
            response.top.append(top)
            response.right.append(right)
            response.bottom.append(bottom)
            response.left.append(left)
        self.get_logger().info(f"识别完成！")
        
        return response

def main():
    rclpy.init()
    node = FaceDetectNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()
    

