

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

	short   state;		//串口状态
	int		timeout;	//串口超时时间
	double	ltime;		//串口响应最后时间

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
		//如果没有调用此函数，设置的用户数据的内存不会被free，除非用户可以确保内存已经释放。
		int CSerial_Set_Destroy_CallBack(CSDestroyUserDataCb func);

		int CSerial_Open(struct CSerialPort* pSerial);
		int CSerial_IsOpen(struct CSerialPort* pSerial);
		int CSerial_Close(struct CSerialPort* pSerial);

		//启动一个线程循环处理读写请求
		int CSerial_Start_Loop(struct CSerialPort* pSerial);
		//线程里面的数据处理回调函数，每次调用传递一个字符处理
		int CSerial_Set_Proc_CallBack(CSProcCb func);

		int CSerial_Read(struct CSerialPort* pSerial, char* buf, int size);
		int CSerial_Write(struct CSerialPort* pSerial, char* buf, int size);

#ifdef __cplusplus
}
#endif



#endif //_LIB_C_SERIAL_H_