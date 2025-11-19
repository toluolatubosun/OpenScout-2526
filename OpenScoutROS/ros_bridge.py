#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
from geometry_msgs.msg import Twist
import socket
import threading
import time

class ArduinoBridge(Node):
    def __init__(self):
        super().__init__('arduino_bridge')
        self.publisher_ = self.create_publisher(String, 'cmd_sent', 10)
        self.status_publisher = self.create_publisher(String, 'arduino_status', 10)
        
        # Subscribe to cmd_vel for robot movement
        self.cmd_subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.cmd_vel_callback,
            10)
        
        # Subscribe to direct commands
        self.command_subscription = self.create_subscription(
            String,
            'arduino_command',
            self.command_callback,
            10)
        
        self.conn = None
        self.server_running = False
        self.setup_server()
        
    def cmd_vel_callback(self, msg):
        # Convert Twist message to Arduino commands
        linear_x = msg.linear.x
        angular_z = msg.angular.z
        
        # Simple mapping: forward/backward and turn
        if linear_x > 0.1:
            self.send_command('W')
        elif linear_x < -0.1:
            self.send_command('S')
        elif angular_z > 0.1:
            self.send_command('A')  # Turn left
        elif angular_z < -0.1:
            self.send_command('D')  # Turn right
        else:
            self.send_command('X')  # Stop
            
    def command_callback(self, msg):
        self.send_command(msg.data)
        
    def setup_server(self):
        def server_thread():
            while True:
                try:
                    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                    server.bind(('0.0.0.0', 11411))
                    server.listen(1)
                    
                    self.get_logger().info('Waiting for Arduino on port 11411...')
                    self.conn, addr = server.accept()
                    self.get_logger().info(f'Arduino connected: {addr}')
                    self.server_running = True
                    
                    # Publish connection status
                    status_msg = String()
                    status_msg.data = f"Arduino connected from {addr}"
                    self.status_publisher.publish(status_msg)
                    
                    # Keep connection alive and handle incoming data
                    while self.server_running:
                        try:
                            # Check for any incoming data from Arduino
                            if self.conn:
                                self.conn.settimeout(1.0)
                                try:
                                    data = self.conn.recv(1024)
                                    if data:
                                        self.get_logger().info(f'Arduino says: {data.decode().strip()}')
                                except socket.timeout:
                                    pass
                                except:
                                    break
                            time.sleep(0.1)
                        except:
                            self.get_logger().warn('Arduino disconnected')
                            self.conn = None
                            self.server_running = False
                            break
                            
                except Exception as e:
                    self.get_logger().error(f'Server error: {e}')
                    time.sleep(2)
                finally:
                    try:
                        if server:
                            server.close()
                    except:
                        pass
                    self.conn = None
                    self.server_running = False
        
        threading.Thread(target=server_thread, daemon=True).start()
        
    def send_command(self, cmd):
        if self.conn:
            try:
                self.conn.sendall(cmd.encode() + b'\n')
                msg = String()
                msg.data = cmd
                self.publisher_.publish(msg)
                self.get_logger().info(f'Sent: {cmd}')
                return True
            except Exception as e:
                self.get_logger().warn(f'Failed to send command: {e}')
                self.conn = None
                self.server_running = False
                return False
        else:
            self.get_logger().warn('No Arduino connection')
            return False

def main():
    rclpy.init()
    bridge = ArduinoBridge()
    
    print("ROS2 Arduino Bridge started!")
    print("Send commands via:")
    print("  ros2 topic pub /arduino_command std_msgs/String \"data: 'W'\"")
    print("  ros2 topic pub /cmd_vel geometry_msgs/Twist ...")
    print("Or use keyboard input:")
    
    def spin_ros():
        rclpy.spin(bridge)
    
    ros_thread = threading.Thread(target=spin_ros, daemon=True)
    ros_thread.start()
    
    try:
        while rclpy.ok():
            key = input("CMD (WASD/0-9/X/+/-): ").strip()
            if key:
                bridge.send_command(key.upper())
    except KeyboardInterrupt:
        pass
    
    bridge.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()