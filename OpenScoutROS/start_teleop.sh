#!/bin/bash

# Start teleop_twist_keyboard in running container

echo "======================================"
echo "Starting teleop_twist_keyboard"
echo "======================================"
echo ""

# Find the running container
CONTAINER_ID=$(docker ps -q --filter ancestor=arduino-ros-bridge)

if [ -z "$CONTAINER_ID" ]; then
    echo "ERROR: No running bridge container found!"
    echo "Please start the bridge first: ./quick_start.sh"
    exit 1
fi

echo "Found container: $CONTAINER_ID"
echo ""
echo "Keyboard Controls:"
echo "  i = Forward"
echo "  , = Backward"
echo "  j = Turn Left"
echo "  l = Turn Right"
echo "  k = Stop"
echo "  q/z = Increase/Decrease speed"
echo "  CTRL+C = Exit"
echo ""
echo "Starting teleop..."
echo ""

# Start teleop in the container
docker exec -it $CONTAINER_ID bash -c "source /opt/ros/humble/setup.bash && ros2 run teleop_twist_keyboard teleop_twist_keyboard"
