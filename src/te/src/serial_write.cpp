#include "serial_writer.h"
 
void serial_writer::set_serial(CString port_name, int baud_rate) {
	 this->COM_port = port_name.AllocSysString();
	 this->baud_rate = baud_rate;
}
 
void serial_writer::send_data(char input_data_1_byte) {
	bool serial_open = FALSE;
	serial_open = this->serial_open(this->COM_port, this->baud_rate);
	if (serial_open) {
		this->serial_write(&input_data_1_byte);
		this->Serial_close();
	}
}
 
void serial_writer::send_data(std::vector<char> input_data) {
	bool serial_open = FALSE;
	int i;
	serial_open = this->serial_open(this->COM_port, this->baud_rate);
	if (serial_open) {
		for (i = 0; i < input_data.size(); i++) {
			this->serial_write(&input_data[i]);
		}
		this->Serial_close();
	}
}
 
int serial_writer::serial_open(LPCWSTR COMx, int BaudRate) {
	this->hCom = CreateFile(COMx, //COM1口    
		GENERIC_READ | GENERIC_WRITE, //允许读和写    
		0, //独占方式    
		NULL,
		OPEN_EXISTING, //打开而不是创建     
		0, //重叠方式FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED  (同步方式设置为0)
		NULL);
	if (this->hCom == INVALID_HANDLE_VALUE)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
		std::cout << "-------------------------  无法打开串口！  -------------------------" << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY |
			FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		std::cout << std::endl;
		return FALSE;
	}
	SetupComm(this->hCom, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024 
 
								 //设定读写超时 
								 /*COMMTIMEOUTS TimeOuts;
								 TimeOuts.ReadIntervalTimeout=1000;
								 TimeOuts.ReadTotalTimeoutMultiplier=500;
								 TimeOuts.ReadTotalTimeoutConstant=5000; //设定写超时
								 TimeOuts.WriteTotalTimeoutMultiplier=500;
								 TimeOuts.WriteTotalTimeoutConstant = 2000;
								 SetCommTimeouts(hCom, &TimeOuts); //设置超时
								 */
	DCB dcb;
	GetCommState(this->hCom, &dcb);
	dcb.BaudRate = BaudRate;		//设置波特率为BaudRate
	dcb.ByteSize = 8;					//每个字节有8位 
	dcb.Parity = NOPARITY;			//无奇偶校验位 
	dcb.StopBits = ONESTOPBIT;		//一个停止位
	SetCommState(this->hCom, &dcb);		//设置参数到hCom
	PurgeComm(this->hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//清空缓存区		//PURGE_TXABORT 中断所有写操作并立即返回，即使写操作还没有完成。
												   //PURGE_RXABORT 中断所有读操作并立即返回，即使读操作还没有完成。
												   //PURGE_TXCLEAR 清除输出缓冲区 
												   //PURGE_RXCLEAR 清除输入缓冲区  
	return TRUE;
}
 
int serial_writer::serial_write(char lpOutBuffer[])	{//同步写串口
	DWORD dwBytesWrite = sizeof(lpOutBuffer);
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	ClearCommError(this->hCom, &dwErrorFlags, &ComStat);
	bWriteStat = WriteFile(this->hCom, lpOutBuffer, dwBytesWrite, &dwBytesWrite, NULL);
	if (!bWriteStat)
	{
		//printf("写串口失败!\n");
		return FALSE;
	}
	PurgeComm(this->hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	return TRUE;
}
 
void serial_writer::Serial_close(void){//关闭串口
	CloseHandle(this->hCom);
}