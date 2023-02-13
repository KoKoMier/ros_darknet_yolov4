#include"ros/ros.h"
#include"sensor_msgs/LaserScan.h"
#include "std_msgs/String.h"
#include <serial/serial.h>
#include<iostream>
#include<string>
#include<list>
#include <algorithm>
#include <string.h>
#define start_angle 112.5

using namespace std;
using namespace ros;

class serial_writer
{
public:
	void set_serial(CString port_name, int baud_rate = 9600);
	void send_data(char input_data_1_byte);
	void send_data(std::vector<char> input_data);
private:
	LPCWSTR COM_port = _T("COM3");
	HANDLE hCom;
	int baud_rate = 9600;
	int serial_open(LPCWSTR COMx, int BaudRate);
	int serial_write(char lpOutBuffer[]);
	void Serial_close(void);
};

void cb(const sensor_msgs::LaserScan::ConstPtr& msg)
{
    int min_dist = 0;
    float te_angle = 112.5;
    list<int> scan_filter;
    list<int> empty_list;
    list<int> all_dist_list;
    list<int> all_angle_list;
    for(int q = 90 ; q < 136 ;q++)
    {
        for (int i = 90 ; i<136 ;i++)
        {
            try
            {
                if (0 < msg->ranges[i*4] <3.2)
                    scan_filter.push_back(msg->ranges[i*4]);
            }
            catch(const std::exception& e)
            {
                continue;
            }
        }
        if (scan_filter != empty_list)
        {
            auto min_it = min_element(scan_filter.begin(), scan_filter.end());
        }      
        scan_filter.clear();
        
        all_dist_list.push_back(min_dist);
    }
    int length1 = sizeof(all_dist_list)/4;
    for (int w = 0 ; length1 ; w++)
    {
        min_dist += all_dist_list.front();
        all_dist_list.pop_front();
    }
    min_dist = min_dist/length1;

    for (int j = 90 ; j <136 ; j++)
    {
        try
        {
            if (msg->ranges[j*4] > 0)
            {
                if (msg->ranges[j*4] == min_dist)
                    te_angle = j;       
            }
        }
        catch(const std::exception& e)
        {
            continue;
        }
    }

    float angle = te_angle - start_angle;
    int dist = min_dist;
    ROS_INFO("dist = %d,angle = %f",dist,angle);
    char a[4]; 
    a[0] = '0x2C';
    a[1] = 0x12;
    a[2] = dist;
    a[3] = angle;
    serial_talk()
    ser.write(a,4);
}

void serial_talk(char &port,int baudrate,char a[3],serial::Serial &ser)
{
    ser.write(a[3],4);
}

int main(int argc, char const *argv[])
{
    char port = '/dev/tty';
    int baudrate = 9600;
    serial::Serial ser; //声明串口对象
    
    ser.setPort("/dev/tty");//串口设备
    ser.setBaudrate(9600);//设置波特率
    
    serial::Timeout to = serial::Timeout::simpleTimeout(9);
    ser.setTimeout(to);
    
    ser.open();//打开串口
    
    ros::init(argc,argv,"te");
    // 3.创建 ROS 句柄
    ros::NodeHandle nh;
    // 4.创建订阅者对象
    ros::Subscriber sub = nh.subscribe<sensor_msgs::LaserScan>("/scan",1000,cb);
    // 5.回调函数处理订阅的数据
    // 6.spin
    ros::spin();
    return 0;}
