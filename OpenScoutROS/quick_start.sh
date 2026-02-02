#!/bin/bash

# Quick Start Script for ROS 2 Arduino Bridge

echo "======================================"
echo "Arduino ROS 2 Bridge - Quick Start"
echo "======================================"
echo ""

# Check if in correct directory
if [ ! -f "docker-compose.yml" ]; then
    echo "ERROR: docker-compose.yml not found!"
    echo "Please run this script from the ros2_bridge directory"
    exit 1
fi

# Check if Docker is running
if ! docker info > /dev/null 2>&1; then
    echo "ERROR: Docker is not running!"
    echo "Please start Docker Desktop first"
    exit 1
fi

# Build if needed
if [ "$1" == "build" ] || [ ! "$(docker images -q arduino-ros-bridge 2> /dev/null)" ]; then
    echo "Building Docker image..."
    docker-compose build
    echo ""
fi

# Get Arduino IP
echo "Please enter the Arduino IP address"
echo "(Check Arduino Serial Monitor for IP)"
read -p "Arduino IP: " ARDUINO_IP

if [ -z "$ARDUINO_IP" ]; then
    echo "No IP provided. Exiting."
    exit 1
fi

echo ""
echo "Starting ROS bridge..."
echo "Arduino IP: $ARDUINO_IP"
echo ""
echo "After bridge starts, open a NEW terminal and run:"
echo "  ./start_teleop.sh"
echo ""

# Run the bridge with the IP
docker-compose run ros python3 /app/ros_bridge.py $ARDUINO_IP
