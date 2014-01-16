//Author: Eranda Lakshantha

#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include "/opt/ros/electric/stacks/nxt/nxt_msgs/msg_gen/cpp/include/nxt_msgs/JointCommand.h"

ros::Publisher publisher;
ros::Publisher publisher2;
ros::Timer timer;
nxt_msgs::JointCommand message;
geometry_msgs::Twist base_msg;
bool start_time_flag = false;
bool start_time_flag_2 = false;
bool overAllDone = false;
bool navigationDone = false;
bool velocity_flag = false;
ros::Time now1_2;
ros::Time now1_2_2;
ros::Time now1_1;
std::string command = "";

void armCallback(const std_msgs::String::ConstPtr& msg)
{
  command = msg->data.c_str();
}

void publish()
{
	if(command == "do"){	
	message.name = "m_wheel_joint";	
	message.effort = 0.6;

	base_msg.linear.x = 0.03;

	if (!start_time_flag) 
	{
		now1_2 = ros::Time::now();
		start_time_flag = 1;
	}

	ros::Time now2_2 = ros::Time::now();
	ros::Duration d2 = now2_2 - now1_2;

	if(d2.toSec()>3.0)
	{
		//navigate the robot for 2 seconds
		ROS_INFO("entering navigation phase");
		if(!velocity_flag)
		{
			now1_1 = ros::Time::now();
			velocity_flag = true;
		}
		ros::Time now2_1 = ros::Time::now();
		ros::Duration d3 = now2_1 - now1_1;

		if(d3.toSec() > 1.0 )
		{
			base_msg.linear.x = 0;
			navigationDone = true;
		}
		
		publisher2.publish(base_msg);
	}
	else
	{
		publisher.publish(message);
	}

	if(navigationDone)
	{
		message.effort = -0.6;
				
		if (!start_time_flag_2) 
		{
			now1_2_2 = ros::Time::now();
			start_time_flag_2 = 1;
		}

		ros::Time now2_2_2 = ros::Time::now();
		ros::Duration d4 = now2_2_2 - now1_2_2;

		if(d4.toSec()>3.0)
		{
			overAllDone = true;
		}
		
		publisher.publish(message);
	}
}
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "arm_controller");
	ros::NodeHandle nh;	
	ros::Subscriber sub = nh.subscribe("armtalker",1000,armCallback);
	publisher = nh.advertise<nxt_msgs::JointCommand>("joint_command", 1);
	publisher2 = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1);
	timer = nh.createTimer(ros::Duration(0.1),boost::bind(publish));

	while(nh.ok() && !overAllDone)
	{
		ros::spinOnce();
		//r.sleep();
	}
	
	
	return 0;
}
