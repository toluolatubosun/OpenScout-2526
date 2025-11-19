FROM ros:humble

RUN apt-get update && apt-get install -y \
    python3-pip \
    iputils-ping \
    net-tools \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY ros_bridge.py /app/

RUN chmod +x ros_bridge.py

# Set up ROS environment
ENV ROS_DOMAIN_ID=0
ENV RMW_IMPLEMENTATION=rmw_fastrtps_cpp

CMD ["bash"]