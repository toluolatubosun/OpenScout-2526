#!/usr/bin/env python3
import rospy
from std_msgs.msg import String
import sys, tty, termios

def getKey():
    fd = sys.stdin.fileno()
    old = termios.tcgetattr(fd)
    try:
        tty.setraw(fd)
        return sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old)

def main():
    rospy.init_node('keyboard_control')
    pub = rospy.Publisher('cmd_char', String, queue_size=10)
    
    print("WASD to move, 0-9 for speed, X to stop, Q to quit")
    
    while not rospy.is_shutdown():
        key = getKey()
        if key == 'q':
            break
        if key in 'wasdxWASDX0123456789':
            pub.publish(key.upper())
            print(f"Sent: {key.upper()}")

if __name__ == '__main__':
    main()