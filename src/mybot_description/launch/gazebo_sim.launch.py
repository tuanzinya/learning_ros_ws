import launch
import launch_ros
import os
from ament_index_python.packages import get_package_share_directory
from launch_ros.parameter_descriptions import ParameterValue
from launch.launch_description_sources import PythonLaunchDescriptionSource

def generate_launch_description():
    # 获取功能包的share路径
    urdf_package_path = get_package_share_directory('mybot_description')
    xacro_path = os.path.join(urdf_package_path, 'urdf', 'fishbot/fishbot.urdf.xacro')
    gazebo_path = os.path.join(urdf_package_path, 'world', 'custom_room.world')
    
    # 声明一个urdf目录的参数，方便修改
    action_declare_arg_mode_path = launch.actions.DeclareLaunchArgument(
        name='model', default_value=str(xacro_path), description='加载的模型文件路径'
    )  
    
    # 通过文件路径，获取内容，并转换成参数值对象，以供传入 robot_state_publisher
    cmd_result = launch.substitutions.Command(['xacro ', launch.substitutions.LaunchConfiguration('model')]) # 使用xacro生成模型
    robot_description_value = ParameterValue(cmd_result,value_type = str)
    
    action_robot_state_pub = launch_ros.actions.Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description':robot_description_value}]
    )
    
    action_launch_gazebo = launch.actions.IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            [get_package_share_directory('gazebo_ros'), '/launch', '/gazebo.launch.py']
        ),
        launch_arguments=[('world', gazebo_path), ('verbose', 'true')]
    )
    
    # 将urdf文件转换成sdf以便在gazebo中仿真
    action_spawn_entity = launch_ros.actions.Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=['-topic', '/robot_description', '-entity', 'fishbot']
    )
    
    action_load_joint_state_controller = launch.actions.ExecuteProcess(
        cmd='ros2 control load_controller fishbot_joint_state_broadcaster --set-state active'.split(' '),
        output='screen'
    )
    
    # action_load_effort_controller = launch.actions.ExecuteProcess(
    #     cmd='ros2 control load_controller fishbot_effort_controller --set-state active'.split(' '),
    #     output='screen'
    # )
    
    action_load_diff_driver_controller = launch.actions.ExecuteProcess(
        cmd='ros2 control load_controller fishbot_diff_drive_controller --set-state active'.split(' '),
        output='screen'
    )
    
    return launch.LaunchDescription(
        [
            action_declare_arg_mode_path,
            action_robot_state_pub,
            action_spawn_entity,
            action_launch_gazebo,
            launch.actions.RegisterEventHandler(
                event_handler=launch.event_handlers.OnProcessExit(
                    target_action=action_spawn_entity,
                    on_exit=[action_load_joint_state_controller]
                )
            ),
            # launch.actions.RegisterEventHandler(
            #     event_handler=launch.event_handlers.OnProcessExit(
            #         target_action=action_load_joint_state_controller,
            #         on_exit=[action_load_effort_controller]
            #     )
            # )
            launch.actions.RegisterEventHandler(
                event_handler=launch.event_handlers.OnProcessExit(
                    target_action=action_load_joint_state_controller,
                    on_exit=[action_load_diff_driver_controller]
                )
            )
        ]
    )

