#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import socket
import threading
import time

class ArduinoBridge(Node):
    def __init__(self):
        super().__init__('arduino_bridge')
        self.publisher_ = self.create_publisher(String, 'cmd_sent', 10)
        
        self.conn = None
        self.server_running = False
        self.setup_server()
        
    def setup_server(self):
        def server_thread():
            while not self.server_running:
                try:
                    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                    server.bind(('0.0.0.0', 11411))
                    server.listen(1)
                    
                    self.get_logger().info('Waiting for Arduino on port 11411...')
                    self.conn, addr = server.accept()
                    self.get_logger().info(f'Arduino connected: {addr}')
                    self.server_running = True
                    
                    # Keep connection alive
                    while self.server_running:
                        try:
                            # Send keepalive
                            self.conn.sendall(b'')
                            time.sleep(5)
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
                        server.close()
                    except:
                        pass
        
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
            except:
                self.get_logger().warn('Failed to send command')
                self.conn = None
                self.server_running = False
                return False
        else:
            self.get_logger().warn('No Arduino connection')
            return False

def main():
    rclpy.init()
    bridge = ArduinoBridge()
    
    def spin_ros():
        rclpy.spin(bridge)
    
    ros_thread = threading.Thread(target=spin_ros, daemon=True)
    ros_thread.start()
    
    print("CMD (WASD/0-9/X/+/-): ")
    while rclpy.ok():
        try:
            key = input().strip().upper()
            if key:
                bridge.send_command(key)
        except KeyboardInterrupt:
            break
    
    bridge.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()