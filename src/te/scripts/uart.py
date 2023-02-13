#！ /usr/bin/env python3
#coding=utf-8

import serial

def open_ser():
	port = '/dev/tty'  # 串口号
	baudrate = 9600  # 波特率
	try:
		global ser
		ser = serial.Serial(port, baudrate, timeout=2)
		if (ser.isOpen() == True):
			print("串口打开成功")
	except Exception as exc:
		print("串口打开异常", exc)


# 数据发送
def send_msg(temp):
	try:
		# send_datas = input("请输入要发送的数据\n")
		# ser.write(str(send_datas).encode("gbk"))
		# print("已发送数据:",send_datas)
		#ser.write(str(send_datas1,send_datas2,send_datas3).encode("gbk"))
		ser.write(str(temp))
		#print("已发送数据:", x,y,dist)
		#print("已发送数据:", x,y)
	except Exception as exc:
		print("发送异常", exc)


# 接收数据
def read_msg():
	try:
		print("等待接收数据")
		while True:
			data = ser.read(ser.in_waiting).decode('gbk')
			if data != '':
				break
		print("已接受到数据:", data)
	except Exception as exc:
		print("读取异常", exc)


# 关闭串口
def close_ser():
	try:
		ser.close()
		if ser.isOpen():
			print("串口未关闭")
		else:
			print("串口已关闭")
	except Exception as exc:
		print("串口关闭异常", exc)

open_ser()  # 打开串口
#send_msg(x,y,dist)  # 写数据
#send_msg(temp)  # 写数据
# read_msg()  # 读数据
ser.close()
