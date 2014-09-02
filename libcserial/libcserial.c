

#include "libinclude.h"

void main()
{
	
}

struct CBaseSerial
{
	char	*port;
	int     baudRate;
	short   dataBit;
	short	parity;
	short   stopBit;

	short	mode;		//异步模式和同步模式，默认为同步模式
	
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

typedef struct CBaseSerial _PCBase;
typedef struct CSerialPort _PCSerial;


static _PCSerial* CSerial_Init(_PCSerial *pSerial);
static void CSerial_Base_Destroy(_PCBase *pBase);
static void CSerial_Destroy_Win(_PCSerial *pSerial);
static void CSerial_Destroy_Linux(_PCSerial *pSerial);
static int  CSerial_Init_WPort(_PCSerial *pSerial);
static int  CSerial_Start_WPort(_PCSerial *pSerial);
static int  CSerial_Init_LPort(_PCSerial *pSerial);
static int  CSerial_Open_LPort(_PCSerial *pSerial);
static int  CSerial_Read_Char(_PCSerial *pSerial, COMSTAT comstat);
static int  CSerial_Write_Char(_PCSerial *pSerial);

#ifdef AE_WINDOWS
unsigned long __stdcall CSerial_Loop(void* pSerial);
#else
void* CSerial_Loop(void* pSerial);
#endif

static _PCSerial* CSerial_Init(_PCSerial *pSerial)
{
	_PCSerial *p = pSerial;

#ifdef AE_WINDOWS
	//create events
	p->stOv.hEvent    = CreateEvent(NULL, TRUE, FALSE, NULL);
	p->hWriteEvent    = CreateEvent(NULL, TRUE, FALSE, NULL);
	p->hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	//initialize the event array
	p->hEventArray[0] = p->hShutdownEvent;
	p->hEventArray[1] = p->stOv.hEvent;
	p->hEventArray[2] = p->hWriteEvent;

	//initialize critical section
	InitializeCriticalSection(&p->csCommunicationSync);

#else


#endif

	p->cBase->mode  = CMODE_SYN;
	p->cBase->state = CSTATE_OFF;

	return p;
}

static int  CSerial_Init_WPort(_PCSerial *pSerial)
{
	int ret = CRET_OK;
	char szBaud[BUF_1K] = {0};

	pSerial->dwCommEvents = EV_RXCHAR | EV_CTS;
	
	// now it critical!
	EnterCriticalSection(&pSerial->csCommunicationSync);
	
	/* 
		The parity can be set to "E"=2 (even), "O"=1
		(odd), "N"=0 (none), "M"=3 (mark) or "S"=4 (space).
	*/
	AE_SNPRINTF(szBaud,BUF_1K,"baud=%d parity=%c data=%d stop=%d", pSerial->cBase->baudRate, 
		pSerial->cBase->parity, pSerial->cBase->dataBit, pSerial->cBase->stopBit);
	
	// get a handle to the port
	pSerial->hComm = CreateFileA(pSerial->cBase->port,	// communication port string (COMX)
		       GENERIC_READ | GENERIC_WRITE,	// read/write types
		       0,								// comm devices must be opened with exclusive access
		       NULL,							// no security attributes
		       OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
		       FILE_FLAG_OVERLAPPED,			// Async I/O
		       0);							// template must be 0 for comm devices
	
	if ( INVALID_HANDLE_VALUE == pSerial->hComm ){
		return CSTATE_OFF;
	}
	
	// set the timeout values
	pSerial->stCommTimeouts.ReadIntervalTimeout         = 1000;
	pSerial->stCommTimeouts.ReadTotalTimeoutMultiplier  = 1000;
	pSerial->stCommTimeouts.ReadTotalTimeoutConstant    = 1000;
	pSerial->stCommTimeouts.WriteTotalTimeoutMultiplier = 1000;
	pSerial->stCommTimeouts.WriteTotalTimeoutConstant   = 1000;
	
	// configure
	if ( (CRET_OK==ret) && !SetCommTimeouts(pSerial->hComm, &pSerial->stCommTimeouts) ){
		//log ProcessErrorMessage("SetCommTimeouts()");
		ret = CRET_ERR;
	}

	if ( (CRET_OK==ret) && !SetCommMask(pSerial->hComm, pSerial->dwCommEvents) ){
		//log ProcessErrorMessage("SetCommMask()");
		ret = CRET_ERR;
	}

	if ( (CRET_OK==ret) && !GetCommState(pSerial->hComm, &pSerial->stDcb) ){
		//log ProcessErrorMessage("GetCommState()");
		ret = CRET_ERR;
	}
	
	pSerial->stDcb.fRtsControl = RTS_CONTROL_ENABLE;		// set RTS bit high!
	if ( (CRET_OK==ret) && !BuildCommDCBA(szBaud, &pSerial->stDcb) ){
		//log ProcessErrorMessage("BuildCommDCB()");
		ret = CRET_ERR;
	}
	
	if ( (CRET_OK==ret) && SetCommState(pSerial->hComm, &pSerial->stDcb) ){
		// flush the port
		PurgeComm(pSerial->hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	}
	else{
		//log
		ret = CRET_ERR;
	}
	
	// release critical section
	LeaveCriticalSection(&pSerial->csCommunicationSync);

	return ret;
}

static int  CSerial_Start_WPort(_PCSerial *pSerial)
{
	pSerial->cBase->state = CSTATE_ON;
	if( CMODE_ASY == pSerial->cBase->mode ){
		if( !Ae_Thread_Start(&pSerial->cBase->hThread, NULL,CSerial_Loop,pSerial) ){
			pSerial->cBase->state = CSTATE_OFF;
			//log	
		}
	}
	
	return pSerial->cBase->state;
}

static int  CSerial_Init_LPort(_PCSerial *pSerial)
{
	
}

static int  CSerial_Open_LPort(_PCSerial *pSerial)
{
	
}


static void CSerial_Base_Destroy(_PCBase *pBase)
{
	if( pBase->port ){
		free(pBase->port);
	}
	pBase->port = NULL;

	if( pBase->CSerial_Destroy_UserData ){
		pBase->CSerial_Destroy_UserData(pBase->userData);
	}
	pBase->userData = NULL;

	free(pBase);
}

static void CSerial_Destroy_Win(_PCSerial *pSerial)
{
	if( pSerial->stOv.hEvent ){
		CloseHandle(pSerial->stOv.hEvent);
	}
	pSerial->stOv.hEvent = NULL;

	if( pSerial->hWriteEvent ){
		CloseHandle(pSerial->hWriteEvent);
	}
	pSerial->hWriteEvent = NULL;

	if( pSerial->hShutdownEvent ){
		CloseHandle(pSerial->hShutdownEvent);
	}
	pSerial->hShutdownEvent = NULL;
	
	pSerial->hEventArray[0] = NULL;
	pSerial->hEventArray[1] = NULL;
	pSerial->hEventArray[2] = NULL;

	DeleteCriticalSection(&pSerial->csCommunicationSync);
}

static void CSerial_Destroy_Linux(_PCSerial *pSerial)
{
	
}

static int  CSerial_Read_Char(_PCSerial *pSerial, COMSTAT comstat)
{

}

static int  CSerial_Write_Char(_PCSerial *pSerial)
{

}


_PCSerial* CSerial_Create()
{
	_PCSerial *p = (_PCSerial*)malloc(sizeof(_PCSerial));
	if( !p ){
		return NULL;
	}
	memset(p, 0, sizeof(_PCSerial));
	p->cBase = (_PCBase*)malloc(sizeof(_PCBase));
	if( !p->cBase ){
		free(p);
		return NULL;
	}
	memset(p->cBase, 0, sizeof(_PCBase));

	return CSerial_Init(p);
}

void CSerial_Destroy(_PCSerial* pSerial)
{
	CSerial_Close(pSerial);
	
#ifdef AE_WINDOWS
	CSerial_Destroy_Win(pSerial);
#else
	CSerial_Destroy_Linux(pSerial);
#endif

	CSerial_Base_Destroy(pSerial->cBase);
	pSerial->cBase = NULL;

	free(pSerial);
}

void CSerial_Set_Mode(struct CSerialPort* pSerial, short data)
{
	pSerial->cBase->mode = data;
}

short CSerial_Get_Mode(struct CSerialPort* pSerial)
{
	return pSerial->cBase->mode;
}

void CSerial_Set_Comm_Port(_PCSerial* pSerial, char* port)
{
	if( port ){
		pSerial->cBase->port = AE_STRDUP(port);
	}
	else{
		pSerial->cBase->port = AE_STRDUP("NULL");
	}
}

void CSerial_Set_Baudrate(_PCSerial* pSerial, int data)
{
	pSerial->cBase->baudRate = data;
}

void CSerial_Set_DataBit(_PCSerial* pSerial, short data)
{
	pSerial->cBase->dataBit = data;
}

void CSerial_Set_StopBit(_PCSerial* pSerial, short data)
{
	pSerial->cBase->stopBit = data;
}

void CSerial_Set_Parity(_PCSerial* pSerial, short data)
{
	pSerial->cBase->parity = data;	
}


int CSerial_Set_UserData(_PCSerial* pSerial, void *pData)
{
	pSerial->cBase->userData = pData;
}

int CSerial_Set_Destroy_CallBack(_PCSerial* pSerial, CSDestroyUserDataCb func)
{
	pSerial->cBase->CSerial_Destroy_UserData = func;
}


int CSerial_Open(_PCSerial* pSerial)
{
#ifdef AE_WINDOWS
	if( CSerial_Init_WPort(pSerial) ){
		return CSerial_Start_WPort(pSerial);
	}
#else
	if( CSerial_Init_LPort(pSerial) ){
		return CSerial_Open_LPort(pSerial);
	}
#endif
	return (pSerial->cBase->state = CSTATE_OFF);
}

int CSerial_IsOpen(_PCSerial* pSerial)
{
	return pSerial->cBase->state;
}

int CSerial_Close(_PCSerial* pSerial)
{
	
}


//启动一个线程循环处理读写请求
#ifdef AE_WINDOWS
unsigned long __stdcall CSerial_Loop(void* pData)
#else
void* CSerial_Loop(void* pData)
#endif
{
	_PCSerial *port = (_PCSerial*)pData;
		
	// Misc. variables
	DWORD BytesTransfered = 0; 
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
	COMSTAT comstat;
	BOOL  bResult = TRUE;
	
	port->cBase->bThreadRun = 1;
		
	// Clear comm buffers at startup
	if (port->hComm){
		PurgeComm(port->hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	}

	// begin forever loop.  This loop will run as long as the thread is alive.
	while( port->cBase->bThreadRun )
	{ 

		// Make a call to WaitCommEvent().  This call will return immediatly
		// because our port was created as an async port (FILE_FLAG_OVERLAPPED
		// and an m_OverlappedStructerlapped structure specified).  This call will cause the 
		// m_OverlappedStructerlapped element m_OverlappedStruct.hEvent, which is part of the m_hEventArray to 
		// be placed in a non-signeled state if there are no bytes available to be read,
		// or to a signeled state if there are bytes available.  If this event handle 
		// is set to the non-signeled state, it will be set to signeled when a 
		// character arrives at the port.

		// we do this for each port!

		bResult = WaitCommEvent(port->hComm, &Event, &port->stOv);

		if (!bResult)  
		{ 
			// If WaitCommEvent() returns FALSE, process the last error to determin
			// the reason..
			switch (dwError = GetLastError()) 
			{ 
			case ERROR_IO_PENDING: 	
				{ 
					// This is a normal return value if there are no bytes
					// to read at the port.
					// Do nothing and continue
			//		printf("WaitCommEvent IO_PENDING\n");
					break;
				}
			case 87:
				{
					// Under Windows NT, this value is returned for some reason.
					// I have not investigated why, but it is also a valid reply
					// Also do nothing and continue.
					break;
				}
			default:
				{
					// All other error codes indicate a serious error has
					// occured.  Process this error.
					//port->ProcessErrorMessage("WaitCommEvent()");
					break;
				}
			}
		}
		else
		{
			bResult = ClearCommError(port->hComm, &dwError, &comstat);

			if (comstat.cbInQue == 0)
				continue;
		}	// end if bResult

		// Main wait function.  This function will normally block the thread
		// until one of nine events occur that require action.
		Event = WaitForMultipleObjects(3, port->hEventArray, FALSE, INFINITE);

		switch (Event)
		{
		case 0:
			{
				// Shutdown event.  This is event zero so it will be
				// the higest priority and be serviced first.
				// Kill this thread.
			 	port->cBase->bThreadRun = 0;
		
				break;
			}
		case 1:	// read event
			{
				GetCommMask(port->hComm, &CommEvent);

				if (CommEvent & EV_RXCHAR)
					// Receive character event from port.
					CSerial_Read_Char(port, comstat);
					
				break;
			}  
		case 2: // write event
			{
				// Write character event from port
				CSerial_Write_Char(port);
				break;
			}

		} // end switch
	} // close forever loop


	return 0;
}

//线程里面的数据处理回调函数，每次调用传递一个字符处理
int CSerial_Set_Proc_CallBack(_PCSerial* pSerial, CSProcCb func)
{
	pSerial->cBase->CSerial_Proc_Char = func;
}


int CSerial_Read(_PCSerial* pSerial, char* buf, int size)
{
	
}

int CSerial_Write(_PCSerial* pSerial, char* buf, int size)
{
	
}


