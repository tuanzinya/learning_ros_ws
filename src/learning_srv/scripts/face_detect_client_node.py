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
from rcl_interfaces.srv import SetParameters
from rcl_interfaces.msg import Parameter, ParameterValue, ParameterType

Image_Name = 'the_Suns.jpg'

class FaceDetectClientNode(Node):
    def __init__(self):
        super().__init__('face_detect_client_node')
        self.client_ = self.create_client(FaceDetector, 'face_detect')
        self.bridge_ = CvBridge()
        self.default_path_ = os.path.join(get_package_share_directory('learning_srv') + '/images/' + Image_Name)
        self.get_logger().info(f"人脸识别服务端已启动！")
        self.image_ = cv2.imread(self.default_path_)
        
    def call_set_parameters(self, parameters):
        """
        call_set_parameters 的 Docstring
        调用服务，修改参数值
        :param self: self
        :param parameters: 传入的参数值
        """    
        # 1. 创建一个客户端，等待服务上线 
        update_param = self.create_client(SetParameters, '/face_detect_node/set_parameters')
        while update_param.wait_for_service(timeout_sec = 0.1) is False:
            self.get_logger().info(f"等待参数更新服务端上线！")
        # 2. 创建request
        req = SetParameters.Request()
        req.parameters = parameters
        # 3. 调用服务端更新参数
        fut = update_param.call_async(req)
        rclpy.spin_until_future_complete(self, fut)
        res = fut.result()
        return res
    
    def update_detect_model(self, model = 'hog'):
        """
        update_detect_model 的 Docstring
        根据传入的model，构造Parameters，调用call_set_parameters更新服务端的参数
        :param self: 说明
        :param model: 说明
        """
        # 1.创建参数对象
        param = Parameter()
        param.name = 'model'
        # 2.赋值
        param_value = ParameterValue()
        param_value.string_value = model
        param_value.type = ParameterType.PARAMETER_STRING
        param.value = param_value
        # 3.请求更新参数
        res = self.call_set_parameters([param])
        for result in res.results:
            self.get_logger().info(f"设置参数结果：{result.successful} {result.reason}")
        
    
    def send_request(self):
        # 1.判断服务端是否在线
        while self.client_.wait_for_service(timeout_sec = 0.1) is False:
            self.get_logger().info(f"等待服务端启动...")
        
        # 2.构造Request
        request = FaceDetector.Request()
        request.image = self.bridge_.cv2_to_imgmsg(self.image_)
        
        # 3.发送请求并异步获取结果
        fut = self.client_.call_async(request) # 异步，等待服务端处理完成后将结果存储在fut中
        
        # while not fut.done():
        #     time.sleep(1.0) # 休眠当前线程，导致当前线程无法再接受来自服务端的返回值
        rclpy.spin_until_future_complete(self, fut) # 等待服务端返回响应 
        response = fut.result()
        self.get_logger().info(f"接收到响应，共检测到有{response.number}张人脸, 耗时{response.use_time}s")
        # self.show_response(response)
    
    def show_response(self, response):
        for i in range(response.number):
            top = response.top[i]
            right = response.right[i]
            bottom = response.bottom[i]
            left = response.left[i]
            cv2.rectangle(self.image_, (left, top), (right, bottom), (255, 0, 0), 4)
        cv2.imshow("Face Detect Result", self.image_)
        cv2.waitKey(0)

def main():
    rclpy.init()
    node = FaceDetectClientNode()
    node.update_detect_model('hog')
    node.send_request()
    node.update_detect_model('cnn')
    node.send_request()
    
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()
    