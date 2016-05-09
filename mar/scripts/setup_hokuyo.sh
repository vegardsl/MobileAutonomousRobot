#!/bin/bash
echo "Setting up Hokuyo node"
echo "sudo chmod a+rw /dev/ttyACM0"
sudo chmod a+rw /dev/ttyACM0
echo "ls -l /dev/ttyACM0"
ls -l /dev/ttyACM0
echo "rosparam set hokuyo_node/port /dev/ttyACM0 "
rosparam set hokuyo_node/port /dev/ttyACM0 
