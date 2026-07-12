cmake_minimum_required(VERSION 3.8)
project(topic_practice)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(rclcpp REQUIRED)
find_package(rclpy REQUIRED)
find_package(std_msgs REQUIRED)
find_package(topic_practice)


rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/SystemStatus.msg"
  DEPENDENCIES builtin_interfaces
)
ament_export_dependencies(rosidl_default_runtime)


add_executable(hello_qt src/hello_qt.cpp)
target_link_libraries(hello_qt Qt5::Widgets)

add_executable(sys_status_display src/sys_status_display.cpp)
target_link_libraries(sys_status_display Qt5::Widgets)
ament_target_dependencies(sys_status_display rclcpp topic_practice)


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
    scripts/sys_status_pub.py
    DESTINATION lib/${PROJECT_NAME}  
)

install(TARGETS
    hello_qt
    sys_status_display
    DESTINATION lib/${PROJECT_NAME}
)

ament_package()
