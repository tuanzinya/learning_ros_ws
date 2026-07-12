cmake_minimum_required(VERSION 3.8)
project(learning_tf)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclpy REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

add_executable(static_tf_broadcaster_cpp src/static_tf_broadcaster.cpp)
ament_target_dependencies(static_tf_broadcaster_cpp rclcpp geometry_msgs tf2_ros tf2_geometry_msgs)

add_executable(dynamic_tf_broadcaster_cpp src/dynamic_tf_broadcaster.cpp)
ament_target_dependencies(dynamic_tf_broadcaster_cpp rclcpp geometry_msgs tf2_ros tf2_geometry_msgs)

add_executable(tf_listener_cpp src/tf_listener.cpp)
ament_target_dependencies(tf_listener_cpp rclcpp geometry_msgs tf2_ros tf2_geometry_msgs)


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

install(PROGRAMS
  scripts/static_tf_broadcaster.py
  scripts/dynamic_tf_broadcaster.py
  scripts/tf_listener.py
  DESTINATION lib/${PROJECT_NAME}  
)

install(TARGETS
  static_tf_broadcaster_cpp
  dynamic_tf_broadcaster_cpp
  tf_listener_cpp
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
