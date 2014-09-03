

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


#ifdef __cplusplus
extern "C"{
#endif
		struct CSerialPort* CSerial_Create();
		void CSerial_Destroy(struct CSerialPort* pSerial);
		void CSerial_Set_Comm_Port(struct CSerialPort* pSerial, char* port);
		void CSerial_Set_Baudrate(struct CSerialPort* pSerial, int data);
		void CSerial_Set_DataBit(struct CSerialPort* pSerial, short data);
		void CSerial_Set_StopBit(struct CSerialPort* pSerial, short data);
		void CSerial_Set_Parity(struct CSerialPort* pSerial, char data);
		
		char* CSerial_Get_Comm_Port(struct CSerialPort* pSerial);
		int   CSerial_Get_Baudrate(struct CSerialPort* pSerial);
		short CSerial_Get_DataBit(struct CSerialPort* pSerial);
		short CSerial_Get_StopBit(struct CSerialPort* pSerial);
		char  CSerial_Get_Parity(struct CSerialPort* pSerial);

		void CSerial_Set_Mode(struct CSerialPort* pSerial, short data);
		short CSerial_Get_Mode(struct CSerialPort* pSerial);

		int CSerial_Set_UserData(struct CSerialPort *pSerial, void *pData);
		void* CSerial_Get_UserData(struct CSerialPort *pSerial);
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