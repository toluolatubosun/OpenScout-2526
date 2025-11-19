FROM ros:noetic

RUN apt-get update && apt-get install -y \
    ros-noetic-rosserial-python \
    ros-noetic-rosserial-server \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY keyboard_control.py /app/

RUN chmod +x keyboard_control.py

CMD ["bash"]