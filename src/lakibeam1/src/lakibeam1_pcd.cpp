#include <ros/ros.h> 
#include <pcl/point_cloud.h> 
#include <pcl_conversions/pcl_conversions.h> 

#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud2_iterator.h>

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#include <sys/select.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <math.h>
#include "../include/data_type.h"
#include "../include/remote.h"

int main(int argc, char **argv)
{
	struct sockaddr_in ser_addr, clent_addr;
	std::string hostip, sensorip, port, frame_id, output_topic, scanfreq, filter, laser_enable, scan_range_start, scan_range_stop;
	std::vector <bm_response_scan_t> scan_vec;
	ros::Time scan_begin;
	unsigned int last_timestamp_;
	bool inverted;
	int i = 0, j = 12, points_cnt;
	int angle_offset = 0;
	int resolution = 25;
	int scan_vec_ready = 0;
	int sockfd;

	ros::init(argc, argv, "pointcloud2_publisher");
	ros::NodeHandle nh("~");
	ros::Rate rate(30);

	nh.getParam("frame_id", frame_id);
	nh.getParam("output_topic", output_topic);
	nh.getParam("inverted", inverted);
	nh.getParam("hostip", hostip);
	nh.getParam("sensorip", sensorip);
	nh.getParam("port", port);
	nh.getParam("angle_offset", angle_offset);

	nh.getParam("scanfreq", scanfreq);
	nh.getParam("filter", filter);
	nh.getParam("laser_enable", laser_enable);
	nh.getParam("scan_range_start", scan_range_start);
	nh.getParam("scan_range_stop", scan_range_stop);

	ros::Publisher pcl_pub = nh.advertise<sensor_msgs::PointCloud2> (output_topic, 10);

	ROS_INFO("frame_id:%s", frame_id.c_str());
	ROS_INFO("output_topic:%s", output_topic.c_str());
	ROS_INFO("inverted:%s", (inverted ? "True" : "False"));
	ROS_INFO("hostip:%s", hostip.c_str());
	ROS_INFO("sensorip:%s", sensorip.c_str());
	ROS_INFO("port:%s", port.c_str());
	ROS_INFO("scanfreq:%s", scanfreq.c_str());
	ROS_INFO("filter:%s", filter.c_str());
	ROS_INFO("laser_enable:%s", laser_enable.c_str());
	ROS_INFO("scan_range_start:%s", scan_range_start.c_str());
	ROS_INFO("scan_range_stop:%s", scan_range_stop.c_str());

	sensor_config(sensorip, "/api/v1/sensor/scanfreq", scanfreq);
	//sensor_config(sensorip, "/api/v1/sensor/filter", filter);
	sensor_config(sensorip, "/api/v1/sensor/laser_enable", laser_enable);
	sensor_config(sensorip, "/api/v1/sensor/scan_range/start", scan_range_start);
	sensor_config(sensorip, "/api/v1/sensor/scan_range/stop", scan_range_stop);
	
	ros::Duration(2.0).sleep();
	get_telemetry_data(sensorip);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
	{
		ROS_INFO("Failed to create socket");
		return -1;
    }

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = inet_addr(hostip.c_str());
	ser_addr.sin_port = htons(atoi(port.c_str()));

	if(bind(sockfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr)) < 0)
	{
		ROS_INFO("Socket bind error!");
		return -1;
	}

	while (ros::ok())
	{
		if(scan_vec_ready == 0)
		{
			while(1)
			{
				if(j == 12)
				{
					unsigned int len = sizeof(clent_addr);
					last_timestamp_ = MSOP_Data.Timestamp;
					recvfrom(sockfd, &MSOP_Data, sizeof(MSOP_Data), 0, (struct sockaddr*)&clent_addr, &len);
					if(MSOP_Data.BlockID[0].Azimuth == 0)
					{
						scan_begin = ros::Time::now();
					}
					if((MSOP_Data.BlockID[1].Azimuth - MSOP_Data.BlockID[0].Azimuth) > 0)
					{
						resolution = (MSOP_Data.BlockID[1].Azimuth - MSOP_Data.BlockID[0].Azimuth) / 16;
					}
					points_cnt = 0;
					j = 0;
				}

				for(;j < 12; j++)
				{
					for(i = 0; i < 16; i++)
					{
						bm_response_scan_t response_ptr;
						response_ptr.angle = (MSOP_Data.BlockID[j].Azimuth + (resolution * i));
						if(MSOP_Data.BlockID[j].DataFlag == 0xEEFF)
						{
							if(response_ptr.angle == 0)
							{
								if(!scan_vec.empty() & (scan_vec_ready == 0))
								{
									scan_vec_ready = 1;
									if(scan_vec.size() < 1200)
									{
										j = 12;
									}
									break;
								}
							}
							response_ptr.dist = MSOP_Data.BlockID[j].Result[i].Dist_1;
							response_ptr.rssi = MSOP_Data.BlockID[j].Result[i].RSSI_1;
							response_ptr.timestamp = MSOP_Data.Timestamp + (points_cnt * (MSOP_Data.Timestamp - last_timestamp_) / 192.0);
							points_cnt ++;
							scan_vec.push_back(response_ptr);
						}
					}
					if(scan_vec_ready == 1)
					{
						break;
					}
				}
				if(scan_vec_ready == 1)
				{
					break;
				}
			}
		}

		if(scan_vec_ready == 1)
		{
  			sensor_msgs::PointCloud2 cloud2_;
			ROS_INFO("New topic %s published, total data points: %ld", output_topic.c_str(), scan_vec.size());
        	cloud2_.header.frame_id = frame_id.c_str();
			cloud2_.header.stamp = scan_begin;
			cloud2_.width = scan_vec.size();
			cloud2_.height = 1;
			cloud2_.row_step = 0;
			cloud2_.is_dense = false;
    		cloud2_.is_dense = true;
			sensor_msgs::PointCloud2Modifier pc2_modifier(cloud2_);
    		pc2_modifier.setPointCloud2Fields(5, "x", 1, sensor_msgs::PointField::FLOAT32, "y", 1,
                                      sensor_msgs::PointField::FLOAT32, "z", 1, sensor_msgs::PointField::FLOAT32,
                                      "intensity", 1, sensor_msgs::PointField::FLOAT32,
									  "time", 1, sensor_msgs::PointField::FLOAT32);

      		cloud2_.data.resize((cloud2_.width) * cloud2_.point_step);

			sensor_msgs::PointCloud2Iterator<float> iter_x(cloud2_, "x");
			sensor_msgs::PointCloud2Iterator<float> iter_time(cloud2_, "time");

			for(int i =0 ;i < scan_vec.size();i++)
			{
				if (!inverted)
				{
					iter_x[0] = (sin(DEG2RAD(scan_vec[i].angle) / 100) * (float)scan_vec[i].dist) / 1000;//x
					iter_x[1] = (cos(DEG2RAD(scan_vec[i].angle) / 100) * (float)scan_vec[i].dist) / 1000;//y
				}
				else
				{
					iter_x[0] = (sin(DEG2RAD(scan_vec[scan_vec.size() - i - 1].angle) / 100) * (float)scan_vec[i].dist) / 1000;//x
					iter_x[1] = (cos(DEG2RAD(scan_vec[scan_vec.size() - i - 1].angle) / 100) * (float)scan_vec[i].dist) / 1000;//y
				}
				iter_x[2] = 0.0;
				iter_x[3] = (float)scan_vec[i].rssi;//intensity
				iter_time[0] = (scan_vec[i].timestamp - scan_vec[0].timestamp) * 0.000001;//timestamp in microseconds(us)
				++iter_x;
				++iter_time;
			}

			pcl_pub.publish(cloud2_);
			ros::spinOnce();
			scan_vec.clear();
			scan_vec_ready = 0;
		}
	}
	close(sockfd);

	return 0;
}
