#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import socket
import json
import sys
import threading
import time

class ArduinoROSBridge(Node):
    def __init__(self, arduino_ip, arduino_port=8888):
        super().__init__('arduino_ros_bridge')
        
        self.arduino_ip = arduino_ip
        self.arduino_port = arduino_port
        self.socket = None
        self.connected = False
        
        # Status printing configuration
        self.STATUS_PRINT_INTERVAL = 10  # Print every Nth status message
        self.status_counter = 0
        
        # Connect to Arduino
        self.connect_to_arduino()
        
        # Subscribe to cmd_vel
        self.subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.cmd_vel_callback,
            10)
        
        # Start status receiver thread
        self.status_thread = threading.Thread(target=self.receive_status, daemon=True)
        self.status_thread.start()
        
        self.get_logger().info('Arduino ROS Bridge started')
        self.get_logger().info(f'Subscribed to /cmd_vel')
    
    def connect_to_arduino(self):
        """Connect to Arduino TCP server"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(5.0)
            self.get_logger().info(f'Connecting to Arduino at {self.arduino_ip}:{self.arduino_port}...')
            self.socket.connect((self.arduino_ip, self.arduino_port))
            self.connected = True
            self.get_logger().info('Connected to Arduino!')
        except Exception as e:
            self.get_logger().error(f'Failed to connect to Arduino: {e}')
            self.connected = False
    
    def cmd_vel_callback(self, msg):
        """Convert Twist message to Arduino command"""
        if not self.connected:
            return
        
        # Extract velocities
        linear_x = msg.linear.x
        angular_z = msg.angular.z
        
        # Determine command based on Twist values
        cmd = 'X'  # Default stop
        speed = 150  # Default speed in PWM (0-255)
        duration = 300  # Default duration in ms (100-900)
        
        # Thresholds
        LINEAR_THRESHOLD = 0.1
        ANGULAR_THRESHOLD = 0.1
        
        # Map to W/A/S/D commands
        if abs(linear_x) > LINEAR_THRESHOLD:
            if linear_x > 0:
                cmd = 'W'  # Forward
                speed = int(abs(linear_x) * 255)
            else:
                cmd = 'S'  # Backward
                speed = int(abs(linear_x) * 255)
        elif abs(angular_z) > ANGULAR_THRESHOLD:
            if angular_z > 0:
                cmd = 'A'  # Turn left
                speed = int(abs(angular_z) * 255)
            else:
                cmd = 'D'  # Turn right
                speed = int(abs(angular_z) * 255)
        
        # Clamp speed
        speed = max(50, min(255, speed))
        
        # Build JSON command
        command = {
            "cmd": cmd,
            "speed": speed,
            "duration": duration
        }
        
        # Send to Arduino (no spaces in JSON for Arduino parser)
        try:
            json_str = json.dumps(command, separators=(',', ':')) + '\n'
            self.socket.sendall(json_str.encode())
            self.get_logger().info(f'Sent: {command}', throttle_duration_sec=0.5)
        except Exception as e:
            self.get_logger().error(f'Send error: {e}')
            self.connected = False
    
    def receive_status(self):
        """Receive status JSON from Arduino"""
        buffer = ""
        while rclpy.ok():
            if not self.connected:
                time.sleep(1)
                continue
            
            try:
                data = self.socket.recv(1024).decode()
                if not data:
                    self.get_logger().warn('Arduino disconnected')
                    self.connected = False
                    break
                
                buffer += data
                
                # Process complete JSON messages (lines)
                while '\n' in buffer:
                    line, buffer = buffer.split('\n', 1)
                    if line.strip():
                        try:
                            status = json.loads(line)
                            self.status_counter += 1
                            
                            # Only print every STATUS_PRINT_INTERVAL messages
                            if self.status_counter % self.STATUS_PRINT_INTERVAL == 0:
                                self.get_logger().info(
                                    f'Status: cmd={status["command"]}, '
                                    f'speed={status["speed"]}, '
                                    f'duration={status["duration"]}, '
                                    f'rssi={status["rssi"]} dBm',
                                    throttle_duration_sec=1.0
                                )
                        except json.JSONDecodeError:
                            pass
            except socket.timeout:
                continue
            except Exception as e:
                self.get_logger().error(f'Receive error: {e}')
                self.connected = False
                break

def main(args=None):
    rclpy.init(args=args)
    
    # Get Arduino IP
    if len(sys.argv) > 1:
        arduino_ip = sys.argv[1]
    else:
        arduino_ip = input("Enter Arduino IP address: ").strip()
    
    if not arduino_ip:
        print("No IP provided. Exiting.")
        return
    
    print(f"\n{'='*50}")
    print(f"Arduino ROS Bridge - ROS 2 Humble")
    print(f"{'='*50}")
    print(f"Arduino IP: {arduino_ip}:8888")
    print(f"Listening to: /cmd_vel")
    print(f"\nUse teleop_twist_keyboard to control:")
    print(f"  ros2 run teleop_twist_keyboard teleop_twist_keyboard")
    print(f"{'='*50}\n")
    
    node = ArduinoROSBridge(arduino_ip)
    
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
