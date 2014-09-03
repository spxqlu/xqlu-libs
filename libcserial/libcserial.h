

#ifndef _LIB_C_SERIAL_H_
#define _LIB_C_SERIAL_H_


#define  CRET_OK	1
#define  CRET_ERR	0

#define  CSTATE_ON  1
#define  CSTATE_OFF 0

#define  CMODE_SYN	0		//同步模式
#define  CMODE_ASY	1		//异步模式

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
		//如果没有调用此函数，设置的用户数据的内存不会被free，除非用户可以确保内存已经释放。
		int CSerial_Set_Destroy_CallBack(struct CSerialPort* pSerial,CSDestroyUserDataCb func);
		void* CSerial_Get_UserData(struct CSerialPort* pSerial);

		int CSerial_Open(struct CSerialPort* pSerial);
		int CSerial_IsOpen(struct CSerialPort* pSerial);
		int CSerial_Close(struct CSerialPort* pSerial);

		//线程里面的数据处理回调函数，每次调用传递一个字符处理
		int CSerial_Set_Proc_CallBack(struct CSerialPort* pSerial,CSProcCb func);

		int CSerial_Read(struct CSerialPort* pSerial, char* buf, int size);
		int CSerial_Write(struct CSerialPort* pSerial, char* buf, int size);

#ifdef __cplusplus
}
#endif



#endif //_LIB_C_SERIAL_H_