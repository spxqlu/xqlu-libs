
#include "libinclude.h"

#ifndef AE_WINDOWS



_PCSerial* CSerial_Init(_PCSerial *pSerial)
{
	_PCSerial *p = pSerial;

	pSerial->fd = -1;
	p->cBase->mode  = CMODE_SYN;
	p->cBase->state = CSTATE_OFF;

	return p;
}

int  CSerial_Init_Port(_PCSerial *pSerial)
{
	int   i;  
    int   status;  
    int   speeds[] = {
		B115200, B57600, B38400, B19200, B9600, B4800,
		  B2400,  B1800,  B1200,   B600,  B300,  B200}; 
    int   names[]  = {
		 115200,  57600,  38400,  19200,  9600,  4800,
		   2400,   1800,   1200,    600,   300,   200};  
           
    struct termios options;  
       
    if( tcgetattr( fd, &options ) != 0 ){  
         perror("get com opthons failed. \n");      
         return CRET_ERR;   
    }  
    
    //设置串口输入波特率和输出波特率  
    for ( i= 0; i < sizeof(speeds) / sizeof(int); i++){  
         if (pSerial->cBase->baudRate == names[i]){               
               cfsetispeed(&options, speeds[i]);   
               cfsetospeed(&options, speeds[i]);    
         }  
    }       
     
    //修改控制模式，保证程序不会占用串口  
    options.c_cflag |= CLOCAL;  
    //修改控制模式，使得能够从串口中读取输入数据  
    options.c_cflag |= CREAD;  
    
    //设置数据流控制
    switch(pSerial->cBase->fCtrl) 
    {  
       case 0 ://不使用流控制  
              options.c_cflag &= ~CRTSCTS;  
              break;     
        
       case 1 ://使用硬件流控制  
              options.c_cflag |= CRTSCTS;  
              break;  
       case 2 ://使用软件流控制  
              options.c_cflag |= IXON | IXOFF | IXANY;  
              break;  
    }  
    //设置数据位  
    //屏蔽其他标志位  
    options.c_cflag &= ~CSIZE;  
    switch (pSerial->cBase->dataBit)  
    {    
       case 5:  
              options.c_cflag |= CS5;  
              break;  
       case 6:  
              options.c_cflag |= CS6;  
              break;  
       case 7:      
              options.c_cflag |= CS7;  
              break;  
       case 8:      
              options.c_cflag |= CS8;  
              break;    
       default:     
              fprintf(stderr,"unsupported data size.\n");  
              return CRET_ERR;   
    }  
    //设置校验位  
    switch (pSerial->cBase->parity)  
    {    
       case 'n':  
       case 'N': //无奇偶校验位。  
                 options.c_cflag &= ~PARENB;   
                 options.c_iflag &= ~INPCK;      
                 break;   
       case 'o':    
       case 'O'://设置为奇校验      
                 options.c_cflag |= (PARODD | PARENB);   
                 options.c_iflag |= INPCK;               
                 break;   
       case 'e':   
       case 'E'://设置为偶校验    
                 options.c_cflag |= PARENB;         
                 options.c_cflag &= ~PARODD;         
                 options.c_iflag |= INPCK;        
                 break;  
       case 's':  
       case 'S': //设置为空格   
                 options.c_cflag &= ~PARENB;  
                 options.c_cflag &= ~CSTOPB;  
                 break;   
        default:    
                 fprintf(stderr,"unsupported parity. \n");      
                 return CRET_ERR;   
    }   
    // 设置停止位   
    switch (pSerial->cBase->stopBit)  
    {    
       case 1 :     
                options.c_cflag &= ~CSTOPB; break;   
       case 2 :     
                options.c_cflag |= CSTOPB; break;  
       default:     
                fprintf(stderr,"unsupported stop bits. \n");   
                return CRET_ERR;  
    }  
     
    //修改输出模式，原始数据输出  
    options.c_oflag &= ~OPOST;  
    
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//我加的  
    //options.c_lflag &= ~(ISIG | ICANON);  
     
    //设置等待时间和最小接收字符  
    options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */    
    options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */  
     
    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据，但是不读  
    tcflush(fd,TCIFLUSH);  
     
    //Active configuration. (set termios data configuration which be changed.)  
    if (tcsetattr(fd,TCSANOW,&options) != 0){  
        perror("com set options error! \n");    
        return CRET_ERR;   
    }  

    return CRET_OK;   
}


int  CSerial_Open_Port(_PCSerial *pSerial)
{
	pSerial->fd = open( pSerial->cBase->port, O_RDWR|O_NOCTTY|O_NDELAY);  
	if ( 0 == pSerial->fd )  {  
		perror("open serial port failed. \n");  
		return CRET_ERR;  
	}  
	//恢复串口为阻塞状态                                 
	if( fcntl(pSerial->fd, F_SETFL, 0) < 0 )  {  
		printf("fcntl failed! \n");  
		return CRET_ERR;  
	}
	//测试是否为终端设备      
	if( 0 == isatty(STDIN_FILENO) ) {  
		printf("standard input is not a terminal device\n");  
		return CRET_ERR;  
	}  
	else  {  
		printf("isatty success!\n");  
	}
	
	if( CMODE_ASY == pSerial->cBase->mode ){
		if( !Ae_Thread_Start(&pSerial->cBase->hThread, NULL,CSerial_Loop,pSerial) ){
			pSerial->cBase->state = CSTATE_OFF;
			perror("start asynchronous thread failed. \n");	
		}
	}
  
	return CRET_OK;  
}


void CSerial_Destroy_Port(_PCSerial *pSerial)
{
	//nothing to do
}


int CSerial_Write_Char(_PCSerial* pSerial, char* buf, int size)
{
	int len = write(pSerial->fd, buf, size);  
    if( len == size )  {  
		return len;  
	}       
    else{  
		tcflush(fd,TCOFLUSH);  
		return CRET_ERR; 
	}  
}

int CSerial_Read_Char(_PCSerial* pSerial, char* buf, int size)
{
	int len = 0, ret = 0;  
    fd_set rFs; 
    struct timeval ts;  
	
    FD_ZERO(&rFs);  
    FD_SET(fd,&rFs);  
	
    ts.tv_sec  = 3;  
    ts.tv_usec = 0;  
	
    ret = select(pSerial->fd+1,&rFs,NULL,NULL,&ts);  
    if( ret > 0 ){
		memset(pSerial->cBase->chReadBuf, 0, BUF_4K);
		len = read(pSerial->fd,pSerial->cBase->chReadBuf, BUF_4K);  
		if( CMODE_ASY == pSerial->cBase->mode ){
			int i = 0;
			if( pSerial->cBase->CSerial_Proc_Char ){
				for( i=0; i<len; i++ ){
					pSerial->cBase->CSerial_Proc_Char(pSerial, pSerial->cBase->chReadBuf[i]);
				}
			}
			else{
				perror("com asynchronous mode, but no process function! \n");
			}
		}
		else if( CMODE_SYN == pSerial->cBase->mode ){
			size = size > BUF_4K ? BUF_4K : size;
			memcpy(buf,pSerial->cBase->chReadBuf, size);	
		}
	}  
    else if( ret == 0 ){ 
		//nothing to do  
    } 
	else{
		perror("select error. \n");
	}

	return len;
}

void* CSerial_Loop(void* pData)
{
	_PCSerial *pSerial = (_PCSerial*)pData;

	if( !pSerial || pSerial->fd < 1 ){
		return 0;
	}

	pSerial->cBase->bThreadRun = 1;

	while ( pSerial->cBase->bThreadRun ){    
		int len = CSerial_Read_Char(pSerial, pSerial->cBase->chReadBuf, BUF_4K);
		if( len ){
			//log data
		}
    }
	
	return 0;
}


#endif // NO AE_WINDOWS