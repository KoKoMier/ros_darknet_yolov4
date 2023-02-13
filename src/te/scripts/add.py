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
    global te_angle
    global min_dist
    scan_filter=[]
    empty_list =[]
    for i in range(90,136):
        try:
            if 0 < msg.ranges[i*4] <3.2:
                scan_filter.append(msg.ranges[i*4]) 
        except IndexError:
            continue
    if scan_filter != empty_list:
        min_dist = min(scan_filter)

    # rospy.loginfo('distance:' + min)
    for j in range(90,136):
        try:
            if msg.ranges[j*4] > 0 :
                if msg.ranges[j*4] == min_dist:
                    te_angle = j       
        except IndexError:
            continue     
    x = min_dist*math.cos(te_angle)
    y = min_dist*math.sin(te_angle)
    rospy.loginfo(f"dist = {min_dist},te_angle = {te_angle},x = {x},y = {y}")
    

if __name__ == "__main__" :
    min_dist = 1
    te_angle = 2
    main()