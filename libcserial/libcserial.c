

#include "libinclude.h"

void main()
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
	
	CSerial_Destroy_Port(pSerial);

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

void CSerial_Set_Parity(_PCSerial* pSerial, char data)
{
	pSerial->cBase->parity = data;	
}

char* CSerial_Get_Comm_Port(struct CSerialPort* pSerial)
{
	return pSerial->cBase->port;
}

int   CSerial_Get_Baudrate(struct CSerialPort* pSerial)
{
	return pSerial->cBase->baudRate;
}

short CSerial_Get_DataBit(struct CSerialPort* pSerial)
{
	return pSerial->cBase->dataBit;
}

short CSerial_Get_StopBit(struct CSerialPort* pSerial)
{
	return pSerial->cBase->stopBit;
}
		
char CSerial_Get_Parity(struct CSerialPort* pSerial)
{
	return pSerial->cBase->parity;
}

int CSerial_Set_UserData(_PCSerial* pSerial, void *pData)
{
	pSerial->cBase->userData = pData;
}

void* CSerial_Get_UserData(struct CSerialPort *pSerial)
{
	return pSerial->cBase->userData;
}

int CSerial_Set_Destroy_CallBack(_PCSerial* pSerial, CSDestroyUserDataCb func)
{
	pSerial->cBase->CSerial_Destroy_UserData = func;
}


int CSerial_Open(_PCSerial* pSerial)
{
	if( CSerial_Init_Port(pSerial) ){
		return CSerial_Open_Port(pSerial);
	}
	
	return (pSerial->cBase->state = CSTATE_OFF);
}

int CSerial_IsOpen(_PCSerial* pSerial)
{
	return pSerial->cBase->state;
}

int CSerial_Close(_PCSerial* pSerial)
{
#ifdef AE_WINDOWS
	if ( pSerial->cBase->bThreadRun ){
		do
		{
			SetEvent(pSerial->hShutdownEvent);
		} while (pSerial->cBase->bThreadRun);
	}
	CloseHandle(pSerial->hComm);

#else
	close(pSerial->fd);
#endif
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
#ifdef AE_WINDOWS
	if( CMODE_ASY == pSerial->cBase->mode ){
		return CSerial_Write_Asy(pSerial, buf, size);
	}
	else if( CMODE_SYN == pSerial->cBase->mode ){
		return CSerial_Write_Syn(pSerial, buf, size);
	}
#else
	return CSerial_Write_Char(pSerial, buf, size);
#endif

	return 0;
}


