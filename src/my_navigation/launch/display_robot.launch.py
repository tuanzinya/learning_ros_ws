import launch
import launch_ros
import os
from ament_index_python.packages import get_package_share_directory
from launch_ros.parameter_descriptions import ParameterValue

def generate_launch_description():
    # 获取默认的urdf路径
    urdf_package_path = get_package_share_directory('mybot_description')
    urdf_path = os.path.join(urdf_package_path, 'urdf', 'fish_robot.xacro')
    config_path = os.path.join(urdf_package_path, 'config', 'display_robot_config.rviz')
    
    # 声明一个urdf目录的参数，方便修改
    action_declare_arg_mode_path = launch.actions.DeclareLaunchArgument(
        name='model', default_value=str(urdf_path), description='加载的模型文件路径'
    )
    
    # 通过文件路径，获取内容，并转换成参数值对象，以供传入 robot_state_publisher
    # cmd_result = launch.substitutions.Command(['cat ', launch.substitutions.LaunchConfiguration('model')]) # 使用urdf生成模型
    cmd_result = launch.substitutions.Command(['xacro ', launch.substitutions.LaunchConfiguration('model')]) # 使用xacro生成模型
    robot_description_value = ParameterValue(cmd_result,value_type = str)
    
    action_robot_state_pub = launch_ros.actions.Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description':robot_description_value}]
    )
    
    action_joint_state_pub = launch_ros.actions.Node(
        package='joint_state_publisher',
        executable='joint_state_publisher',
    )
    
    action_rviz_node = launch_ros.actions.Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d', config_path]
    )
    
    return launch.LaunchDescription(
        [
            action_declare_arg_mode_path,
            action_robot_state_pub,
            action_joint_state_pub,
            action_rviz_node
        ]
    )

