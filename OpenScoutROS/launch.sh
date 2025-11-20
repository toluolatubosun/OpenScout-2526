#!/bin/bash

source /opt/ros/humble/setup.bash

echo "======================================"
echo "ROS 2 Arduino Bridge Launcher"
echo "======================================"
echo ""
echo "Starting bridge..."
echo ""

# Run the bridge (it will ask for Arduino IP)
python3 /app/ros_bridge.py

echo ""
echo "======================================"
echo "Bridge stopped."
echo "======================================"
