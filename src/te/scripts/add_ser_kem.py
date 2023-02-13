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
    scan_filter=[]
    empty_list =[]
    for i in range(90,136):
        try:
            if 0 < msg.ranges[i*4] <3.2:
                scan_filter.append(msg.ranges[i*4]) 
        except IndexError:
            continue
    if scan_filter != empty_list:
        min_dist1 = min(scan_filter)


    scan_filter.clear()

    for i2 in range(90,136):
        try:
            if 0 < msg.ranges[i2*4] <3.2:
                scan_filter.append(msg.ranges[i2*4]) 
        except IndexError:
            continue
    if scan_filter != empty_list:
        min_dist2 = min(scan_filter)

    min_dist = min_dist1*0.5 + min_dist2*0.5

    # rospy.loginfo('distance:' + min)
    for j in range(90,136):
        try:
            if msg.ranges[j*4] > 0 :
                if msg.ranges[j*4] == min_dist:
                    te_angle = j       
        except IndexError:
            continue    

    
    angle = te_angle
    dist = min_dist
    rospy.loginfo(f"dist = {dist},te_angle = {angle}")
    temp = struct.pack("<BBff",0x2C,0x12,dist,angle)
    ser.write(temp)

if __name__ == "__main__" :
    port = '/dev/tty'  # 串口号
    baudrate = 9600  # 波特率
    ser = serial.Serial(port, baudrate, timeout=9)
    main()