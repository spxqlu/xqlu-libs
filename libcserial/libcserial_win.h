

#ifndef _LIB_C_WIN_SERIAL_H_
#define _LIB_C_WIN_SERIAL_H_

typedef struct CSerialPort _PCSerial;

struct CSerialPort
{
	struct CBaseSerial  *cBase;
	
	// synchronisation objects
	CRITICAL_SECTION	csCommunicationSync;
	
	DWORD				dwCommEvents;
	
	// handles
	HANDLE				hShutdownEvent;
	HANDLE				hComm;
	HANDLE				hWriteEvent;
	
	// Event array. 
	// One element is used for each event. There are two event handles for each port.
	// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
	// There is a general shutdown when the port is closed. 
	HANDLE				hEventArray[3];
	
	// structures
	OVERLAPPED			stOv;
	COMMTIMEOUTS		stCommTimeouts;
	DCB					stDcb;
};


#ifdef __cplusplus
extern "C"{
#endif
	
		 _PCSerial* CSerial_Init(_PCSerial *pSerial);
		 int  CSerial_Init_Port(_PCSerial *pSerial);
		 int  CSerial_Open_Port(_PCSerial *pSerial);
		 void CSerial_Destroy_Port(_PCSerial *pSerial);
		unsigned long __stdcall CSerial_Loop(void* pSerial);

		int CSerial_Write_Syn(_PCSerial* pSerial, char* buf, unsigned int size);
		int CSerial_Write_Asy(_PCSerial* pSerial, char* buf, unsigned int size);
		int CSerial_Write_Char(_PCSerial *pSerial);
		int CSerial_Read_Syn(_PCSerial *pSerial, char* buf, unsigned int size);
		int CSerial_Read_Char(_PCSerial *pSerial, COMSTAT comstat);
	
#ifdef __cplusplus
}
#endif


#endif // _LIB_C_WIN_SERIAL_H_