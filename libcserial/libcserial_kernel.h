
#ifndef _LIB_C_KERNEL_SERIAL_H_
#define _LIB_C_KERNEL_SERIAL_H_


#define  BUF_8B		8
#define  BUF_16B	16
#define  BUF_32B	32
#define  BUF_64B	64
#define  BUF_128B	128
#define  BUF_256B	256
#define  BUF_512B	512
#define  BUF_1K		1024 
#define  BUF_4K		4096
#define  BUF_16K	16384
#define  BUF_32K	32768
#define  BUF_64K	65536

struct CSerialPort;

typedef struct CBaseSerial _PCBase;
typedef void (*_CSProcCb)(struct CSerialPort* pSerial, char ch);
typedef void (*_CSDestroyUserDataCb)(void *pUserData);

struct CBaseSerial
{
	char	*port;
	int     baudRate;
	short   dataBit;
	//parity: "E"=2 (even), "O"=1(odd), "N"=0 (none), "M"=3 (mark) or "S"=4 (space)
	char	parity;		
	short   stopBit;
	short   fCtrl;		//流控制，只用于linux系统串口配置
	
	short	mode;		//异步模式和同步模式，默认为同步模式
	
	short   state;		//串口状态
	int		timeout;	//串口超时时间
	double	ltime;		//串口响应最后时间
	
	char    chReadBuf[BUF_4K];
	int		readBufCount;
	char    chWriteBuf[BUF_4K];
	int     writeBufCount;
	char    chConfBuf[BUF_256B];
	int     confBufCount;
	
	short   bThreadRun;
	AE_THREAD_HANDLE   hThread;
	
	void    *userData;
	_CSDestroyUserDataCb  CSerial_Destroy_UserData;
	_CSProcCb  CSerial_Proc_Char;
};




#endif // _LIB_C_KERNEL_SERIAL_H_