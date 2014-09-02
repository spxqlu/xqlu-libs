

#include "libinclude.h"


void main()
{
	
}


struct CSerialPort* CSerial_Create()
{

}

void CSerial_Destroy(struct CSerialPort* pSerial)
{
	
}


int CSerial_Set_UserData(void *pData)
{
	
}

int CSerial_Set_Destroy_CallBack(CSDestroyUserDataCb func)
{
	
}


int CSerial_Open(struct CSerialPort* pSerial)
{
	
}

int CSerial_IsOpen(struct CSerialPort* pSerial)
{
	
}

int CSerial_Close(struct CSerialPort* pSerial)
{
	
}


//启动一个线程循环处理读写请求
int CSerial_Start_Loop(struct CSerialPort* pSerial)
{
	
}

//线程里面的数据处理回调函数，每次调用传递一个字符处理
int CSerial_Set_Proc_CallBack(CSProcCb func)
{
	
}


int CSerial_Read(struct CSerialPort* pSerial, char* buf, int size)
{
	
}

int CSerial_Write(struct CSerialPort* pSerial, char* buf, int size)
{
	
}


