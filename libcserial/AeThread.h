

#ifndef _AE_THREAD_H_
#define _AE_THREAD_H_

#define	 THREAD_LOG_CATEGORY	"THREAD"

#ifdef  AE_WINDOWS
	#define AE_THREAD_ID		unsigned long
	#define AE_THREAD_HANDLE	HANDLE

	#define AE_THREAD_ATTR_POINTER		LPSECURITY_ATTRIBUTES
	
	#define AE_THREAD_ENTRY_POINTER(pThread_Entry)	\
		unsigned long (__stdcall *pThread_Entry)(void* )
		
	#define AE_THREAD_ENTRY(thread_entry, args)		\
		unsigned long __stdcall thread_entry ( void* args )
#else
	#define AE_THREAD_ID		pthread_t
	#define AE_THREAD_HANDLE	pthread_t*

	#define AE_THREAD_ATTR_POINTER		pthread_attr_t*

	#define AE_THREAD_ENTRY_POINTER(pThread_entry)	\
		void* ( *pThread_entry )(void* )
		
	#define AE_THREAD_ENTRY(thread_entry, args)		\
		void* thread_entry ( void* args )
#endif


#ifdef __cplusplus
extern "C"{
#endif

		int Ae_Thread_Start(AE_THREAD_HANDLE* pHandle, AE_THREAD_ATTR_POINTER pAttr,
				AE_THREAD_ENTRY_POINTER(pThread_Entry), void* args);
		int Ae_Thread_Wait( AE_THREAD_HANDLE pHandle );
		int Ae_Thread_Destroy(AE_THREAD_HANDLE* pHandle);
		AE_THREAD_ID  Ae_Thread_Get_ThreadID();

		int	Ae_Thread_Start_Pool(AE_THREAD_HANDLE* pHandle, int count, AE_THREAD_ATTR_POINTER pAttr,
			AE_THREAD_ENTRY_POINTER(pThread_Entry), void* args);
		int Ae_Thread_Stop_Pool(AE_THREAD_HANDLE* pHandle, int count);

#ifdef __cplusplus
}
#endif


#endif // _AE_THREAD_H_


