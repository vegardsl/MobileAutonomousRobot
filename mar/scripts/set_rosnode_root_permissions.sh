#!/bin/bash
cd ~/mar_catkin_ws/devel/lib/server_wifi # cd to the directory with your node
sudo chown root:root server_wifi # change ownship to root
sudo chmod a+rx server_wifi      # set as executable by all
sudo chmod u+s server_wifi       # set the setuid bit

cd ~/mar_catkin_ws/devel/lib/mobile_platform # cd to the directory with your node
sudo chown root:root motor_driver_interface # change ownship to root
sudo chmod a+rx motor_driver_interface      # set as executable by all
sudo chmod u+s motor_driver_interface       # set the setuid bit

