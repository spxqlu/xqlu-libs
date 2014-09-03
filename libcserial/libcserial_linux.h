
#ifndef _LIB_C_LINUX_SERIAL_H_
#define _LIB_C_LINUX_SERIAL_H_

typedef struct CSerialPort _PCSerial;

struct CSerialPort
{
	struct CBaseSerial  *cBase;
	
	int                 fd;
};


#ifdef __cplusplus
extern "C"{
#endif
	
			_PCSerial* CSerial_Init(_PCSerial *pSerial);
			int  CSerial_Init_Port(_PCSerial *pSerial);
			int  CSerial_Open_Port(_PCSerial *pSerial);
			void CSerial_Destroy_Port(_PCSerial *pSerial);
			void* CSerial_Loop(void* pData);

			int CSerial_Write_Char(_PCSerial* pSerial, char* buf, int size);
			int CSerial_Read_Char(_PCSerial* pSerial, char* buf, int size);
	
#ifdef __cplusplus
}
#endif


#endif // _LIB_C_LINUX_SERIAL_H_