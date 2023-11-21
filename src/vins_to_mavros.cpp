#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>
#include <Eigen/Eigen>
 
 
Eigen::Vector3d p_mav;
Eigen::Quaterniond q_mav;
 
 
void vins_callback(const nav_msgs::Odometry::ConstPtr &msg)
{
    if(msg->header.frame_id == "world")
    {
        p_mav = Eigen::Vector3d(msg->pose.pose.position.y, -msg->pose.pose.position.x, msg->pose.pose.position.z);
 
        q_mav = Eigen::Quaterniond(msg->pose.pose.orientation.w, msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z);
        Eigen::AngleAxisd roll(M_PI/2,Eigen::Vector3d::UnitX()); // 绕 x 轴旋转 pi / 2
        Eigen::AngleAxisd pitch(0,Eigen::Vector3d::UnitY());
        Eigen::AngleAxisd yaw(0,Eigen::Vector3d::UnitZ());
 
        Eigen::Quaterniond _q_mav = roll * pitch * yaw;
        q_mav = q_mav * _q_mav;
    }
}
 
 
int main(int argc, char **argv)
{
    ros::init(argc, argv, "vins_to_mavros");
    ros::NodeHandle nh("~");
 
    ros::Subscriber slam_sub = nh.subscribe<nav_msgs::Odometry>("odom", 100, vins_callback);
 
    ros::Publisher vision_pub = nh.advertise<geometry_msgs::PoseStamped>("vision_pose", 10);
 
 
    // the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);
 
    ros::Time last_request = ros::Time::now();
 
    while(ros::ok()) {
        geometry_msgs::PoseStamped vision;
 
        vision.pose.position.x = p_mav[0];
        vision.pose.position.y = p_mav[1];
        vision.pose.position.z = p_mav[2];
 
        vision.pose.orientation.x = q_mav.x();
        vision.pose.orientation.y = q_mav.y();
        vision.pose.orientation.z = q_mav.z();
        vision.pose.orientation.w = q_mav.w();
 
        vision.header.stamp = ros::Time::now();
        vision_pub.publish(vision);
 
        ROS_INFO("\nposition:\n   x: %.18f\n   y: %.18f\n   z: %.18f\norientation:\n   x: %.18f\n   y: %.18f\n   z: %.18f\n   w: %.18f", \
        p_mav[0],p_mav[1],p_mav[2],q_mav.x(),q_mav.y(),q_mav.z(),q_mav.w());
 
        ros::spinOnce();
        rate.sleep();
    }
 
    return 0;
}


