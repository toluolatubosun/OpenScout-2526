FROM ros:humble

# Install teleop_twist_keyboard
RUN apt-get update && apt-get install -y \
    ros-humble-teleop-twist-keyboard \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy bridge script
COPY ros_bridge.py /app/
COPY launch.sh /app/

RUN chmod +x /app/launch.sh

# Source ROS
RUN echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc

CMD ["/app/launch.sh"]
