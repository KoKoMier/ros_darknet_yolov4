#! /usr/bin/env python
#coding=utf-8
import rospy
from sensor_msgs.msg import LaserScan
import struct
import serial
import math

def main():
	rospy.init_node('te')
	rospy.Subscriber('scan',LaserScan,cb,queue_size=10)
	rospy.spin()

def cb(msg):
    scan_filter1={}
    all_list_dict = {}
    min_dist_list_kem = []
    min_dist_list = []
    num1 = 10
    while num1:
        num2 =2
        while num2:
            for i in range(90,136):
                try:
                    if 0 < msg.ranges[i*4] <3.2:
                        scan_filter2 = {i:msg.ranges[i*4]}
                        scan_filter1.update(scan_filter2)
                        scan_filter2.clear()
                except IndexError:
                    continue
            te_angle = min(scan_filter1,key=scan_filter1.get)    

            min_dist = scan_filter1.get(te_angle)
            min_dist_list_kem.append(min_dist)
            num2 -= 1

        min_dist = min_dist_list_kem[0]*q + min_dist_list_kem[1]*(1-q)
        min_dist_list.append(min_dist)
        num1 -= 1
    
    min_dist_unit =set(min_dist_list)
    min_dist_list = list(min_dist_unit)
    min_dist = sum(min_dist_list)/len(min_dist_list)

    min_dist = int(1000*min_dist)
    angle = te_angle
    rospy.loginfo(f"dist = {min_dist},angle = {angle}")
    temp = struct.pack("<BBff",0x2C,0x12,min_dist,angle)
    # ser.write(temp)

    

if __name__ == "__main__" :
    port = '/dev/ttyUSB0'  # 串口号
    baudrate = 9600  # 波特率
    # ser = serial.Serial(port, baudrate, timeout=9)
    q = 0.7  #（0，0.9，0.7,0.5,0.4)
    main()