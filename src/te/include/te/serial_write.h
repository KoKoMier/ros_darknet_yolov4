#pragma once
#include"windows.h"
#include <string.h>
#include <iostream>
#include <vector>
class serial_writer
{
public:
	/** @brief 设置串口参数。
	@param port_name 串口名称，请打开<设备管理器>查找电脑连接单片机的串口，如令port_name = "COM5";
	@param baud_rate 波特率。
	*/
	void set_serial(CString port_name, int baud_rate = 9600);
	/** @brief 发送单字节数据，调用该函数立即发送
	@param input_data_1_byte 字符型(8位)数据。
	*/
	void send_data(char input_data_1_byte);
	/** @brief 发送一个数据包，调用该函数立即发送，从input_data[0]开始发送。
	@param input_data 待发送的字符容器。
	*/
	void send_data(std::vector<char> input_data);
private:
	LPCWSTR COM_port = _T("COM3");
	HANDLE hCom;
	int baud_rate = 9600;
	int serial_open(LPCWSTR COMx, int BaudRate);
	int serial_write(char lpOutBuffer[]);
	void Serial_close(void);
};