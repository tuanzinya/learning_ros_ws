import launch
import launch_ros

def generate_launch_description():
    # 1.声明一个launch参数
    action_declare_arg_background_g = launch.actions.DeclareLaunchArgument(
        'launch_arg_bg', default_value = '150'
    )
    action_declare_arg_background_r = launch.actions.DeclareLaunchArgument(
        'launch_arg_br', default_value = '150'
    )
    
    """产生launch描述"""
    action_node_turtlesim_node = launch_ros.actions.Node(
        package='turtlesim',
        executable='turtlesim_node',
        parameters=[{'background_g': launch.substitutions.LaunchConfiguration('launch_arg_bg', default = '150')} # 2.把launch的参数手动传递给某个节点
                    ,{'background_r': launch.substitutions.LaunchConfiguration('launch_arg_br', default = '150')}],     
        output='screen'
    )
    action_node_patrol_tut_server = launch_ros.actions.Node(
        package='learning_srv',
        executable='patrol_tut_server',
        output='screen'
    )
    action_node_patrol_tut_client = launch_ros.actions.Node(
        package='learning_srv',
        executable='patrol_tut_client',
        output='screen'
    )

    return launch.LaunchDescription([
        # actions动作
        action_node_turtlesim_node,
        action_node_patrol_tut_server,
        action_node_patrol_tut_client,
        action_declare_arg_background_g,
        action_declare_arg_background_r
    ])
