/**
* Code in this file is based on 
* Example 8-5. "keys_to_twist_with_ramps.py" 
* in the book
* "Programming Robots with ROS"
* By: Morgan Quigley; Brian Gerkey; William D. Smart
* Publisher: O'Reilly Media, Inc.
* Pub. Date: December 2, 2015
* Print ISBN-13: 978-1-4493-2389-9
*
* Translated to C++ and adapted to fit this project.
*/

#include "ros/ros.h"
#include "geometry_msgs/Twist.h"

#include <cmath>

geometry_msgs::Twist g_target_twist;
geometry_msgs::Twist g_prev_twist;
ros::Time g_last_send_time;
double g_vel_scale = 0.2;
double g_vel_ramp = 0.7;

ros::Publisher cmd_vel_ramped;

double ramped_vel(double v_prev, 
                  double v_target, 
                  ros::Time t_prev, 
                  ros::Time t_now, 
                  double ramp_rate)
{
  double step = ramp_rate * (t_now - t_prev).toSec();

  double sign;
  if(v_target > v_prev)
  {
    sign = 1.0;
  }
  else
  {
    sign = -1.0;
  }

  double error = fabs(v_target - v_prev);
  if(error < step)
  {
    return v_target; // If we can get there within this timestep, we're done.
  }

  return (v_prev + sign*step); // Take a step towards the target.
}

geometry_msgs::Twist ramped_twist(geometry_msgs::Twist prev, 
                     geometry_msgs::Twist target, 
                     ros::Time t_prev, 
                     ros::Time t_now, 
                     double ramp)
{
  geometry_msgs::Twist tw;
  tw.angular.z = (float)ramped_vel((double)prev.angular.z, 
                                   (double)target.angular.z, 
                                   t_prev,
                                   t_now, 
                                   ramp);

  tw.linear.x = (float)ramped_vel((double)prev.linear.x, 
                                  (double)target.linear.x, 
                                  t_prev,
                                  t_now, 
                                  ramp);
  return tw;
}

void send_twist()
{
  ros::Time t_now = ros::Time::now();
  g_prev_twist = ramped_twist(g_prev_twist, 
                              g_target_twist, 
                              g_last_send_time, 
                              t_now, 
                              g_vel_ramp);
  
  g_last_send_time = t_now;
  cmd_vel_ramped.publish(g_prev_twist);
}

void targetVelocityUpdate_cb(const geometry_msgs::Twist::ConstPtr& msg)
{
  g_target_twist.linear.x = msg->linear.x;
  g_target_twist.angular.z = msg->angular.z;
}

void initNode()
{
  g_last_send_time = ros::Time::now();

  // Set initial linear command state to zero
  g_target_twist.linear.x = 0;
  g_target_twist.linear.y = 0;
  g_target_twist.linear.z = 0;
  // Set initial angular command state to zero
  g_target_twist.angular.x = 0;
  g_target_twist.angular.y = 0;
  g_target_twist.angular.z = 0;

  // Set initial linear command state to zero
  g_prev_twist.linear.x = 0;
  g_prev_twist.linear.y = 0;
  g_prev_twist.linear.z = 0;
  // Set initial angular command state to zero
  g_prev_twist.angular.x = 0;
  g_prev_twist.angular.y = 0;
  g_prev_twist.angular.z = 0;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "velocity_ramp");
  ros::NodeHandle n;
  ros::Subscriber sub = n.subscribe("cmd_vel_switched", 1, targetVelocityUpdate_cb);
  cmd_vel_ramped = n.advertise<geometry_msgs::Twist>("cmd_vel_ramped", 1);

  initNode();

  ros::Rate loop_rate(10);
  
  while(ros::ok())
  {
    send_twist();
    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}

