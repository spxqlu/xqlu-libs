
#include "libinclude.h"

#ifdef AE_WINDOWS


 _PCSerial* CSerial_Init(_PCSerial *pSerial)
{
	_PCSerial *p = pSerial;

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

	p->cBase->mode  = CMODE_SYN;
	p->cBase->state = CSTATE_OFF;

	return p;
}

int  CSerial_Init_Port(_PCSerial *pSerial)
{
	int ret = CRET_OK;
	char szBaud[BUF_1K] = {0};
	char comBuf[BUF_256B] = {0};

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
	AE_SNPRINTF(comBuf, BUF_256B, "\\\\.\\%s", pSerial->cBase->port);
	pSerial->hComm = CreateFileA(comBuf,	// communication port string (COMX)
		       GENERIC_READ | GENERIC_WRITE,	// read/write types
		       0,								// comm devices must be opened with exclusive access
		       NULL,							// no security attributes
		       OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
		       FILE_FLAG_OVERLAPPED,			// Async I/O
		       0);							// template must be 0 for comm devices
	
	if ( INVALID_HANDLE_VALUE == pSerial->hComm ){
		// release critical section
		LeaveCriticalSection(&pSerial->csCommunicationSync);
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

int  CSerial_Open_Port(_PCSerial *pSerial)
{
	pSerial->cBase->state = CSTATE_ON;
	if( CMODE_ASY == pSerial->cBase->mode ){
		if( !Ae_Thread_Start(&pSerial->cBase->hThread, NULL,CSerial_Loop,pSerial) ){
			pSerial->cBase->state = CSTATE_OFF;
			perror("start asynchronous thread failed. \n");	
		}
	}
	
	return pSerial->cBase->state;
}


void CSerial_Destroy_Port(_PCSerial *pSerial)
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

int CSerial_Read_Syn(_PCSerial *pSerial, char* buf, unsigned int size)
{
	BOOL  bRead     = TRUE; 
	BOOL  bResult   = TRUE;
	DWORD dwError   = 0;
	DWORD BytesRead = 0;
	
	EnterCriticalSection(&pSerial->csCommunicationSync);
	
	if (bRead)
	{
		bResult = ReadFile(pSerial->hComm,	// Handle to COMM port 
			              buf,				// RX Buffer Pointer
			              size,				// Read bytes
			              &BytesRead,		// Stores number of bytes read
			              &pSerial->stOv);	// pointer to the m_ov structure
		// deal with the error code 
		if (!bResult)  
		{ 
			switch (dwError = GetLastError()) 
			{ 
			case ERROR_IO_PENDING: 	
				{ 
					// asynchronous i/o is still in progress 
					// Proceed on to GetOverlappedResults();
					bRead = FALSE;
					break;
				}
			default:
				{
					// Another error has occured.  Process this error.
					//pSerial->ProcessErrorMessage("ReadFile()");
					break;
				} 
			}
		}
		else
		{
			// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
			bRead = TRUE;
		}
	}  // close if (bRead)
	
	if (!bRead)
	{
		bRead = TRUE;
		bResult = GetOverlappedResult(pSerial->hComm,	// Handle to COMM port 
			                          &pSerial->stOv,	// Overlapped structure
			                          &BytesRead,		// Stores number of bytes read
			                          TRUE); 			// Wait flag
		
		// deal with the error code 
		if (!bResult)  {
			//pSerial->ProcessErrorMessage("GetOverlappedResults() in ReadFile()");
		}	
	}  // close if (!bRead)
	
	LeaveCriticalSection(&pSerial->csCommunicationSync);

	return BytesRead;
}

int  CSerial_Read_Char(_PCSerial *pSerial, COMSTAT comstat)
{
	BOOL  bRead = TRUE; 
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	char  RXBuff[2] = {0};

	for (;;) 
	{ 
		// Gain ownership of the comm port critical section.
		// This process guarantees no other part of this program 
		// is using the port object. 
		
		EnterCriticalSection(&pSerial->csCommunicationSync);
		
		// ClearCommError() will update the COMSTAT structure and
		// clear any other errors.
		
		bResult = ClearCommError(pSerial->hComm, &dwError, &comstat);
		
		LeaveCriticalSection(&pSerial->csCommunicationSync);
		
		// start forever loop.  I use this type of loop because I
		// do not know at runtime how many loops this will have to
		// run. My solution is to start a forever loop and to
		// break out of it when I have processed all of the
		// data available.  Be careful with this approach and
		// be sure your loop will exit.
		// My reasons for this are not as clear in this sample 
		// as it is in my production code, but I have found this 
		// solutiion to be the most efficient way to do this.
		
		if (comstat.cbInQue == 0){
			// break out when all bytes have been read
			break;
		}
		
		EnterCriticalSection(&pSerial->csCommunicationSync);
		
		if (bRead)
		{
			bResult = ReadFile(pSerial->hComm,		// Handle to COMM port 
				              RXBuff,				// RX Buffer Pointer
				              1,					// Read one byte
				              &BytesRead,			// Stores number of bytes read
				              &pSerial->stOv);		// pointer to the m_ov structure
			// deal with the error code 
			if (!bResult)  
			{ 
				switch (dwError = GetLastError()) 
				{ 
				case ERROR_IO_PENDING: 	
					{ 
						// asynchronous i/o is still in progress 
						// Proceed on to GetOverlappedResults();
						bRead = FALSE;
						break;
					}
				default:
					{
						// Another error has occured.  Process this error.
						//pSerial->ProcessErrorMessage("ReadFile()");
						break;
					} 
				}
			}
			else
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				bRead = TRUE;
			}
		}  // close if (bRead)
		
		if (!bRead)
		{
			bRead = TRUE;
			bResult = GetOverlappedResult(pSerial->hComm,	// Handle to COMM port 
				                         &pSerial->stOv,	// Overlapped structure
				                         &BytesRead,		// Stores number of bytes read
				                         TRUE); 			// Wait flag
			
			// deal with the error code 
			if (!bResult)  {
				//pSerial->ProcessErrorMessage("GetOverlappedResults() in ReadFile()");
			}	
		}  // close if (!bRead)
		
		if( pSerial->cBase->CSerial_Proc_Char ){
			pSerial->cBase->CSerial_Proc_Char(pSerial, RXBuff[0]);
		}
		
		LeaveCriticalSection(&pSerial->csCommunicationSync);
		
		
	} // end forever loop

	return 1;
}

int  CSerial_Write_Char(_PCSerial *pSerial)
{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;
	DWORD BytesSent = 0;
	
	ResetEvent(pSerial->hWriteEvent);
	
	// Gain ownership of the critical section
	EnterCriticalSection(&pSerial->csCommunicationSync);
	
	if (bWrite)
	{
		// Initailize variables
		pSerial->stOv.Offset = 0;
		pSerial->stOv.OffsetHigh = 0;
		
		// Clear buffer
		PurgeComm(pSerial->hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
		
		bResult = WriteFile(pSerial->hComm,			// Handle to COMM Port
			         pSerial->cBase->chWriteBuf,	// Pointer to message buffer in calling finction
					 pSerial->cBase->writeBufCount,	// Length of message to send
			         &BytesSent,					// Where to store the number of bytes sent
			         &pSerial->stOv);				// Overlapped structure
		
		// deal with any error codes
		if (!bResult)  
		{
			DWORD dwError = GetLastError();
			switch (dwError)
			{
			case ERROR_IO_PENDING:
				{
					// continue to GetOverlappedResults()
					BytesSent = 0;
					bWrite = FALSE;
					break;
				}
			default:
				{
					// all other error codes
					//port->ProcessErrorMessage("WriteFile()");
					LeaveCriticalSection(&pSerial->csCommunicationSync);
				}
			}
		} 
		
		memset(pSerial->cBase->chWriteBuf, 0, BUF_4K);	
		pSerial->cBase->writeBufCount = 0;	
		
	} // end if(bWrite)
	
	if (!bWrite)
	{
		bWrite = TRUE;
		
		bResult = GetOverlappedResult(pSerial->hComm,	// Handle to COMM port 
			                         &pSerial->stOv,	// Overlapped structure
			                         &BytesSent,		// Stores number of bytes sent
			                         TRUE); 			// Wait flag
		
		LeaveCriticalSection(&pSerial->csCommunicationSync);
		
		// deal with the error code 
		if (!bResult)  
		{
			//pSerial->ProcessErrorMessage("GetOverlappedResults() in WriteFile()");
		}	
	} // end if (!bWrite)

	return bResult;
}

int CSerial_Write_Asy(_PCSerial* pSerial, char* buf, unsigned int size)
{
	int ret = 1;

	if( size > BUF_4K - BUF_32B ){
		//log write data is too long
		ret = 0;
	}
	else if( pSerial->cBase->writeBufCount < BUF_4K - size ){
		memcpy(pSerial->cBase->chWriteBuf+pSerial->cBase->writeBufCount, buf, size);
		pSerial->cBase->writeBufCount += size;
		
		// set event for write
		SetEvent(pSerial->hWriteEvent);	
	}
	else{
		ret = 0;
		//log write buf is full.
	}

	return ret;
}

int CSerial_Write_Syn(_PCSerial* pSerial, char* buf, unsigned int size)
{
	int ret = 0;
	if( CSerial_Write_Asy(pSerial, buf, size) ){
		ret = CSerial_Write_Char(pSerial);
	}
	return ret;
}


unsigned long __stdcall CSerial_Loop(void* pData)
{
	_PCSerial *pSerial = (_PCSerial*)pData;
		
	// Misc. variables
	DWORD BytesTransfered = 0; 
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
	COMSTAT comstat;
	BOOL  bResult = TRUE;
		
	// Clear comm buffers at startup
	if (pSerial->hComm){
		PurgeComm(pSerial->hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	}
	else{
		//log
		pSerial->cBase->state = CSTATE_OFF;
		return 0;
	}

	pSerial->cBase->bThreadRun = 1;
	// begin forever loop.  This loop will run as long as the thread is alive.
	while( pSerial->cBase->bThreadRun )
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
		bResult = WaitCommEvent(pSerial->hComm, &Event, &pSerial->stOv);

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
			bResult = ClearCommError(pSerial->hComm, &dwError, &comstat);

			if (comstat.cbInQue == 0)
				continue;
		}	// end if bResult

		// Main wait function.  This function will normally block the thread
		// until one of nine events occur that require action.
		Event = WaitForMultipleObjects(3, pSerial->hEventArray, FALSE, INFINITE);

		switch (Event)
		{
		case 0:
			{
				// Shutdown event.  This is event zero so it will be
				// the higest priority and be serviced first.
				// Kill this thread.
			 	pSerial->cBase->bThreadRun = 0;
				ResetEvent(pSerial->hShutdownEvent);
		
				break;
			}
		case 1:	// read event
			{
				GetCommMask(pSerial->hComm, &CommEvent);

				if (CommEvent & EV_RXCHAR)
					// Receive character event from port.
					CSerial_Read_Char(pSerial, comstat);
					
				break;
			}  
		case 2: // write event
			{
				// Write character event from port
				CSerial_Write_Char(pSerial);
				break;
			}

		} // end switch
	} // close forever loop

	return 0;
}


#endif //AE_WINDOWS