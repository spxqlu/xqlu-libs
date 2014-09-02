

#ifndef _LIB_C_SERIAL_H_
#define _LIB_C_SERIAL_H_


#define  BUF_8B		8
#define  BUF_16B	16
#define  BUF_32B	32
#define  BUF_64B	64
#define  BUF_128B	128
#define  BUF_256B	256
#define  BUF_512B	512
#define  BUF_1K		1024 
#define  BUF_4K		4096
#define  BUF_16k	16384
#define  BUF_32K	32768
#define  BUF_64k	65536

#define  CRET_OK	1
#define  CRET_ERR	0

#define  CSTATE_ON  1
#define  CSTATE_OFF 0

#define  CMODE_SYN	0		//ͬ��ģʽ
#define  CMODE_ASY	1		//�첽ģʽ

typedef void (*CSProcCb)(struct CSerialPort* pSerial, char ch);
typedef void (*CSDestroyUserDataCb)(void *pUserData);


struct CBaseSerial;

#ifdef AE_WINDOWS
	
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

#else

	struct CSerialPort
	{
		struct CBaseSerial  *cBase;


	};

#endif


#ifdef __cplusplus
extern "C"{
#endif
		struct CSerialPort* CSerial_Create();
		void CSerial_Destroy(struct CSerialPort* pSerial);

		void CSerial_Set_Comm_Port(struct CSerialPort* pSerial, char* port);
		void CSerial_Set_Baudrate(struct CSerialPort* pSerial, int data);
		void CSerial_Set_DataBit(struct CSerialPort* pSerial, short data);
		void CSerial_Set_StopBit(struct CSerialPort* pSerial, short data);
		void CSerial_Set_Parity(struct CSerialPort* pSerial, short data);

		void CSerial_Set_Mode(struct CSerialPort* pSerial, short data);
		short CSerial_Get_Mode(struct CSerialPort* pSerial);

		int CSerial_Set_UserData(struct CSerialPort *pSerial, void *pData);
		//���û�е��ô˺��������õ��û����ݵ��ڴ治�ᱻfree�������û�����ȷ���ڴ��Ѿ��ͷš�
		int CSerial_Set_Destroy_CallBack(struct CSerialPort* pSerial,CSDestroyUserDataCb func);
		void* CSerial_Get_UserData(struct CSerialPort* pSerial);

		int CSerial_Open(struct CSerialPort* pSerial);
		int CSerial_IsOpen(struct CSerialPort* pSerial);
		int CSerial_Close(struct CSerialPort* pSerial);

		//�߳���������ݴ���ص�������ÿ�ε��ô���һ���ַ�����
		int CSerial_Set_Proc_CallBack(struct CSerialPort* pSerial,CSProcCb func);

		int CSerial_Read(struct CSerialPort* pSerial, char* buf, int size);
		int CSerial_Write(struct CSerialPort* pSerial, char* buf, int size);

#ifdef __cplusplus
}
#endif



#endif //_LIB_C_SERIAL_H_