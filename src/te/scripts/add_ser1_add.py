#! /usr/bin/env python
#coding=utf-8
import rospy
from sensor_msgs.msg import LaserScan
import struct
import serial

def cb(msg):
    min_dist = 0
    te_angle = 112.5
    scan_filter=[]
    empty_list =[]
    all_dist_list = []
    all_angle_list = []
    for q in range(90,136):
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
        
        all_dist_list.append(min_dist)
    
    for w in range(len(all_dist_list)):
        min_dist += all_dist_list[w]
    min_dist = min_dist/len(all_dist_list)

    for j in range(90,136):
            try:
                if msg.ranges[j*4] > 0 :
                    if msg.ranges[j*4] == min_dist:
                        te_angle = j       
            except IndexError:
                continue
    
    angle = te_angle - start_angle
    dist = min_dist
    rospy.loginfo(f"dist = {dist},angle = {angle}")
    temp = struct.pack("<BBff",0x2C,0x12,dist,angle)
    ser.write(temp)

def main():
	rospy.init_node('te')
	rospy.Subscriber('scan',LaserScan,cb,queue_size=10)
	rospy.spin()

if __name__ == "__main__" :
    port = '/dev/ttyUSB0'  # 串口号   
    baudrate = 9600  # 波特率
    ser = serial.Serial(port, baudrate, timeout=9)
    start_angle = 112.5
    main()