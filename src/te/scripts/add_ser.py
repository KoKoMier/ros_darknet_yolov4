#! /usr/bin/env python
#coding=utf-8
import rospy
from sensor_msgs.msg import LaserScan
import struct
import serial
import math

# import time

def main():
	rospy.init_node('te')
	rospy.Subscriber('scan',LaserScan,cb,queue_size=10)
	rospy.spin()

def cb(msg):

    # start_time = time.time()    # 程序开始时间

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
    temp = struct.pack("<BBff",0x2C,0x12,x,y)
    ser.write(temp)

    # end_time = time.time()    # 程序结束时间
    # run_time = end_time - start_time    # 程序的运行时间，单位为秒
    # rospy.loginfo(f"时间 = {run_time}")

if __name__ == "__main__" :
    port = '/dev/ttyUSB0'  # 串口号
    baudrate = 9600  # 波特率
    ser = serial.Serial(port, baudrate, timeout=9)
    min_dist = 1
    te_angle = 0
    main()