#include <ros/ros.h> 
#include <sensor_msgs/LaserScan.h>
#include<vector>

void laserCallback(const sensor_msgs::LaserScan::ConstPtr& scan_msg) 
{ 
    float laser_dist = 0.0; // 获取激光雷达的距离measurements 
    std::vector<float> ranges = scan_msg->ranges; // 遍历激光雷达的距离measurements 
    for (int i=0; i<ranges.size(); i++) 
    { 
    // 计算激光雷达的距离 
    laser_dist += ranges[i]; 
    } // 输出激光雷达的距离 
    ROS_INFO("Laser Distance: %f", laser_dist); 
}

int main(int argc, char** argv)
{ 
    ros::init(argc, argv, "laser_dist"); 
    ros::NodeHandle nh; 
    ros::Subscriber sub = nh.subscribe("/scan", 1000, laserCallback);
    ros::spin();
    return 0;
}