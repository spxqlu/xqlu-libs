

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


//����һ���߳�ѭ�������д����
int CSerial_Start_Loop(struct CSerialPort* pSerial)
{
	
}

//�߳���������ݴ���ص�������ÿ�ε��ô���һ���ַ�����
int CSerial_Set_Proc_CallBack(CSProcCb func)
{
	
}


int CSerial_Read(struct CSerialPort* pSerial, char* buf, int size)
{
	
}

int CSerial_Write(struct CSerialPort* pSerial, char* buf, int size)
{
	
}


