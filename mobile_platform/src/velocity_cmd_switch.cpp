#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/Char.h"

#define BT 0
#define WIFI 1
#define LOCAL 2
#define NAV 3

geometry_msgs::Twist g_bt_twist;
geometry_msgs::Twist g_wifi_twist;
geometry_msgs::Twist g_loc_twist;
geometry_msgs::Twist g_nav_twist;
geometry_msgs::Twist g_out_twist;
std_msgs::Char msg;

ros::Publisher cmd_vel;

int switch_state;

void initVelSettings()
{
/* ----- g_out_twist ----- */
  // Set initial linear command state to zero
  g_out_twist.linear.x = 0;
  g_out_twist.linear.y = 0;
  g_out_twist.linear.z = 0;

  // Set initial angular command state to zero
  g_out_twist.angular.x = 0;
  g_out_twist.angular.y = 0;
  g_out_twist.angular.z = 0;

/* ----- g_wifi_twist ----- */
  g_wifi_twist.linear.x = 0;
  g_wifi_twist.linear.y = 0;
  g_wifi_twist.linear.z = 0;

  g_wifi_twist.angular.x = 0;
  g_wifi_twist.angular.y = 0;
  g_wifi_twist.angular.z = 0;

/* ----- g_loc_twist ----- */
  g_loc_twist.linear.x = 0;
  g_loc_twist.linear.y = 0;
  g_loc_twist.linear.z = 0;

  g_loc_twist.angular.x = 0;
  g_loc_twist.angular.y = 0;
  g_loc_twist.angular.z = 0;

/* ----- g_nav_twist ----- */
  g_nav_twist.linear.x = 0;
  g_nav_twist.linear.y = 0;
  g_nav_twist.linear.z = 0;

  g_nav_twist.angular.x = 0;
  g_nav_twist.angular.y = 0;
  g_nav_twist.angular.z = 0;

/* ----- g_bt_twist ----- */
  g_bt_twist.linear.x = 0;
  g_bt_twist.linear.y = 0;
  g_bt_twist.linear.z = 0;

  g_bt_twist.angular.x = 0;
  g_bt_twist.angular.y = 0;
  g_bt_twist.angular.z = 0;
}

void btVelUpdate_cb(const geometry_msgs::Twist::ConstPtr& msg)
{
  g_bt_twist.linear.x = msg->linear.x;
  g_bt_twist.angular.z = msg->angular.z;
}

void locVelUpdate_cb(const geometry_msgs::Twist::ConstPtr& msg)
{
  g_loc_twist.linear.x = msg->linear.x;
  g_loc_twist.angular.z = msg->angular.z;
}

void wifiVelUpdate_cb(const geometry_msgs::Twist::ConstPtr& msg)
{
  g_wifi_twist.linear.x = msg->linear.x;
  g_wifi_twist.angular.z = msg->angular.z;
}

void navVelUpdate_cb(const geometry_msgs::Twist::ConstPtr& msg)
{
  g_nav_twist.linear.x = msg->linear.x;
  g_nav_twist.angular.z = msg->angular.z;
}

void switchSetting_cb(const std_msgs::Char::ConstPtr& msg)
{
  if(msg->data == 'n' || msg->data == 'N')
  {
    switch_state = NAV;
  }
  else if(msg->data == 'b' || msg->data == 'B')
  {
    switch_state = BT;
  }
  else if(msg->data == 'v' || msg->data == 'V')
  {
    switch_state = WIFI;
  }
  else if(msg->data == 'l' || msg->data == 'L')
  {
   switch_state = LOCAL;
  }
}

void send_twist()
{
  std::cout << "Switch state: " << switch_state << std::endl;
  switch(switch_state){
    case BT:
      g_out_twist = g_bt_twist; 
      break;
    case WIFI:
      g_out_twist =  g_wifi_twist;
      break;
    case LOCAL:
      g_out_twist = g_loc_twist;
      break;
    case NAV:
      g_out_twist = g_nav_twist;
    break;
  }
  cmd_vel.publish(g_out_twist);
}


int main(int argc, char **argv)
{

  initVelSettings();  

  ros::init(argc, argv, "velocity_cmd_switch");
  ros::NodeHandle n;
  ros::Subscriber sub_1 = n.subscribe("cmd_vel_bt", 1, btVelUpdate_cb);
  ros::Subscriber sub_2 = n.subscribe("cmd_vel_loc", 1, locVelUpdate_cb);
  ros::Subscriber sub_3 = n.subscribe("cmd_vel_wifi", 1, wifiVelUpdate_cb);
  ros::Subscriber sub_4 = n.subscribe("cmd_vel", 1, navVelUpdate_cb);
  ros::Subscriber sub_5 = n.subscribe("switch_setting", 1, switchSetting_cb);

  cmd_vel = n.advertise<geometry_msgs::Twist>("cmd_vel_switched", 1);

  switch_state = BT;

  ros::Rate loop_rate(10);
  
  while(ros::ok())
  {
    send_twist();
    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}
