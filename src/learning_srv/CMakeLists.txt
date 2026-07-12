cmake_minimum_required(VERSION 3.8)
project(learning_srv)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclpy REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(turtlesim REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rcl_interfaces REQUIRED)
find_package(learning_srv REQUIRED)

# 生成服务消息
rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/FaceDetector.srv"
  "srv/Patrol.srv"
  DEPENDENCIES sensor_msgs std_msgs
)
ament_export_dependencies(rosidl_default_runtime)

# cpp节点依赖
add_executable(patrol_tut_server src/patrol_tut_server.cpp)
ament_target_dependencies(patrol_tut_server learning_srv rclcpp turtlesim geometry_msgs rcl_interfaces)

add_executable(patrol_tut_client src/patrol_tut_client.cpp)
ament_target_dependencies(patrol_tut_client learning_srv rclcpp turtlesim geometry_msgs)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# 安装图片资源文件夹
install(
  DIRECTORY images
  DESTINATION share/${PROJECT_NAME}
)

# 安装cpp节点
install(TARGETS
  patrol_tut_server
  patrol_tut_client
  DESTINATION lib/${PROJECT_NAME}  
)

# 安装python节点
install(PROGRAMS
  scripts/learn_face_detect.py
  scripts/face_detect_node.py
  scripts/face_detect_client_node.py
  DESTINATION lib/${PROJECT_NAME}  
)

# 安装launch文件
install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)


ament_package()
