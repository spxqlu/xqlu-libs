

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

#define  CSTATE_ON  1
#define  CSTATE_OFF 0

typedef void (*CSProcCb)(struct CSerialPort* pSerial, char ch);
typedef void (*CSDestroyUserDataCb)(void *pUserData);

struct CBaseSerial
{
#ifdef AE_WINDOWS
	short	port;
#else
	char	*port;
#endif
	int     baudRate;
	int     dataBit;
	short	parity;
	short   stopBit;

	short   state;		//����״̬
	int		timeout;	//���ڳ�ʱʱ��
	double	ltime;		//������Ӧ���ʱ��

	char    chReadBuf[BUF_4K];
	int		readBufCount;
	char    chWriteBuf[BUF_1K];
	int     writeBufCount;
	char    chConfBuf[BUF_256B];
	int     confBufCount;

	short   bThreadRun;
	AE_THREAD_HANDLE   hThread;

	void    *userData;
	CSDestroyUserDataCb  CSerial_Destroy_UserData;
	CSProcCb  CSerial_Proc_Char;
};


#ifdef AE_WINDOWS
	
	struct CSerialPort
	{
		//base struct
		struct CBaseSerial  cBase;


	};

#else

	struct CSerialPort
	{
		//base struct
		struct CBaseSerial  cBase;


	};

#endif


#ifdef __cplusplus
extern "C"{
#endif
		struct CSerialPort* CSerial_Create();
		void CSerial_Destroy(struct CSerialPort* pSerial);

		int CSerial_Set_UserData(void *pData);
		//���û�е��ô˺��������õ��û����ݵ��ڴ治�ᱻfree�������û�����ȷ���ڴ��Ѿ��ͷš�
		int CSerial_Set_Destroy_CallBack(CSDestroyUserDataCb func);

		int CSerial_Open(struct CSerialPort* pSerial);
		int CSerial_IsOpen(struct CSerialPort* pSerial);
		int CSerial_Close(struct CSerialPort* pSerial);

		//����һ���߳�ѭ�������д����
		int CSerial_Start_Loop(struct CSerialPort* pSerial);
		//�߳���������ݴ���ص�������ÿ�ε��ô���һ���ַ�����
		int CSerial_Set_Proc_CallBack(CSProcCb func);

		int CSerial_Read(struct CSerialPort* pSerial, char* buf, int size);
		int CSerial_Write(struct CSerialPort* pSerial, char* buf, int size);

#ifdef __cplusplus
}
#endif



#endif //_LIB_C_SERIAL_H_