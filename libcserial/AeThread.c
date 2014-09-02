

#include "libinclude.h"


AE_THREAD_ID  Ae_Thread_Get_ThreadID()
{
#ifdef AE_WINDOWS
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}


int Ae_Thread_Start(AE_THREAD_HANDLE* pHandle, AE_THREAD_ATTR_POINTER pAttr,
			AE_THREAD_ENTRY_POINTER(pThread_Entry), void* args)
{

#ifdef AE_WINDOWS
	unsigned long threadID = 0;
	*pHandle = CreateThread(pAttr, 0, pThread_Entry, args, 0, &threadID);
	if( NULL == *pHandle){
		return 0;
	}
#else
	*pHandle = (AE_THREAD_HANDLE)malloc(sizeof(pthread_t));
	if( !*pHandle )
		return 0;
	
	int ret = pthread_create(*pHandle, pAttr, pThread_Entry, args);
	if( 0 != ret ){
		return 0;
	}
#endif

	return 1;
}




int Ae_Thread_Wait( AE_THREAD_HANDLE pHandle )
{
#ifdef AE_WINDOWS
	unsigned long ret = WaitForSingleObject(pHandle, INFINITE);
	if( WAIT_OBJECT_0 != ret ){
		return 0;
	}
#else
	void* retVal = NULL;
	int ret = pthread_join(*pHandle, &retVal);
	if( 0 != ret ){
		return 0;
	}
#endif

	return 1;
}

int Ae_Thread_Destroy(AE_THREAD_HANDLE* pHandle)
{
	if( !pHandle || !*pHandle )
		return 0;
	
#ifdef AE_WINDOWS
	CloseHandle(*pHandle);
#else
	free(*pHandle);
#endif

	*pHandle = NULL;

	return 1;
}


int	Ae_Thread_Start_Pool(AE_THREAD_HANDLE* pHandle, int count, AE_THREAD_ATTR_POINTER pAttr,
			AE_THREAD_ENTRY_POINTER(pThread_Entry), void* args)
{
	int i = 0, ret = 0;
	
	if( !pHandle || count<=0 )
		return 0;

	for( ; i<count; i++ ){
		ret = Ae_Thread_Start(&pHandle[i], pAttr, pThread_Entry, args);
		if( 0 == ret ){
			pHandle[i] = NULL;
		}

		AE_SLEEP(3);
	}
	
	return 1;
}


int Ae_Thread_Stop_Pool(AE_THREAD_HANDLE* pHandle, int count)
{
	int i = 0, ret = 0;
	
	if( !pHandle || count<=0 )
		return 0;

	for( ; i<count; i++ )
	{
		if( pHandle[i] == NULL )
			continue;

		ret = Ae_Thread_Wait(pHandle[i]);

		if( 0 == ret ){
			//log;
		}

		ret = Ae_Thread_Destroy(&pHandle[i]);

		if( 0 == ret ){
			//log
		}
	}
	
	return 1;
}





