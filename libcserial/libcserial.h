

#ifndef _LIB_C_SERIAL_H_
#define _LIB_C_SERIAL_H_


#define  CRET_OK	1
#define  CRET_ERR	0

#define  CSTATE_ON  1
#define  CSTATE_OFF 0

#define  CMODE_SYN	0		//ͬ��ģʽ
#define  CMODE_ASY	1		//�첽ģʽ

struct CBaseSerial;
struct CSerialPort;

typedef void (*CSProcCb)(struct CSerialPort* pSerial, char ch);
typedef void (*CSDestroyUserDataCb)(void *pUserData);

#ifdef __cplusplus
extern "C"{
#endif
		ITASK_API struct CSerialPort* CSerial_Create();
		ITASK_API void CSerial_Destroy(struct CSerialPort* pSerial);
		ITASK_API void CSerial_Set_Comm_Port(struct CSerialPort* pSerial, char* port);
		ITASK_API void CSerial_Set_Baudrate(struct CSerialPort* pSerial, int data);
		ITASK_API void CSerial_Set_DataBit(struct CSerialPort* pSerial, short data);
		ITASK_API void CSerial_Set_StopBit(struct CSerialPort* pSerial, short data);
		ITASK_API void CSerial_Set_Parity(struct CSerialPort* pSerial, char data);
		
		ITASK_API char* CSerial_Get_Comm_Port(struct CSerialPort* pSerial);
		ITASK_API int   CSerial_Get_Baudrate(struct CSerialPort* pSerial);
		ITASK_API short CSerial_Get_DataBit(struct CSerialPort* pSerial);
		ITASK_API short CSerial_Get_StopBit(struct CSerialPort* pSerial);
		ITASK_API char  CSerial_Get_Parity(struct CSerialPort* pSerial);

		ITASK_API void  CSerial_Set_Mode(struct CSerialPort* pSerial, short data);
		ITASK_API short CSerial_Get_Mode(struct CSerialPort* pSerial);

		ITASK_API int   CSerial_Set_UserData(struct CSerialPort *pSerial, void *pData);
		ITASK_API void* CSerial_Get_UserData(struct CSerialPort *pSerial);
		//���û�е��ô˺��������õ��û����ݵ��ڴ治�ᱻfree�������û�����ȷ���ڴ��Ѿ��ͷš�
		ITASK_API int   CSerial_Set_Destroy_CallBack(struct CSerialPort* pSerial,CSDestroyUserDataCb func);

		ITASK_API int CSerial_Open(struct CSerialPort* pSerial);
		ITASK_API int CSerial_IsOpen(struct CSerialPort* pSerial);
		ITASK_API int CSerial_Close(struct CSerialPort* pSerial);

		//�߳���������ݴ���ص�������ÿ�ε��ô���һ���ַ������첽ģʽ�������á�
		ITASK_API int CSerial_Set_Proc_CallBack(struct CSerialPort* pSerial,CSProcCb func);

		ITASK_API int CSerial_Read(struct CSerialPort* pSerial, char* buf, int size);
		ITASK_API int CSerial_Write(struct CSerialPort* pSerial, char* buf, int size);

#ifdef __cplusplus
}
#endif



#endif //_LIB_C_SERIAL_H_