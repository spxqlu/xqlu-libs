

#ifndef _LIB_INCLUDE_H_
#define _LIB_INCLUDE_H_


#ifdef AE_WINDOWS
#	ifdef ITASK_EXPORT
#		define ITASK_API __declspec(dllexport)
#	else
#		define ITASK_API __declspec(dllimport)
#	endif
#else
#	define ITASK_API
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <signal.h>


#ifdef AE_WINDOWS
#include <conio.h>
#include <windows.h>
#include <sys/timeb.h>
#include "IO.h"
#else
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include <netdb.h>
#include <iconv.h>
#include <dlfcn.h>
#include <termios.h>
#endif

#ifdef AE_WINDOWS
#define AE_STRDUP           _strdup
#define AE_SNPRINTF     	_snprintf
#define AE_SLEEP(sec)		Sleep(((DWORD)(sec))*((DWORD)1000))
#else
#define AE_STRDUP           strdup
#define AE_SNPRINTF         snprintf
#define AE_SLEEP(sec)		sleep((sec))
#endif


#include "AeThread.h"
#include "libcserial_kernel.h"

#ifdef AE_WINDOWS
#include "libcserial_win.h"
#else
#include "libcserial_linux.h"
#endif

#include "libcserial.h"


#endif //_LIB_INCLUDE_H_