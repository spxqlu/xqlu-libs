
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
         perror("SetupSerial 1");      
         return CRET_ERR;   
    }  
    
    //���ô������벨���ʺ����������  
    for ( i= 0; i < sizeof(speeds) / sizeof(int); i++){  
         if (pSerial->cBase->baudRate == names[i]){               
               cfsetispeed(&options, speeds[i]);   
               cfsetospeed(&options, speeds[i]);    
         }  
    }       
     
    //�޸Ŀ���ģʽ����֤���򲻻�ռ�ô���  
    options.c_cflag |= CLOCAL;  
    //�޸Ŀ���ģʽ��ʹ���ܹ��Ӵ����ж�ȡ��������  
    options.c_cflag |= CREAD;  
    
    //��������������
    switch(pSerial->cBase->fCtrl) 
    {  
       case 0 ://��ʹ��������  
              options.c_cflag &= ~CRTSCTS;  
              break;     
        
       case 1 ://ʹ��Ӳ��������  
              options.c_cflag |= CRTSCTS;  
              break;  
       case 2 ://ʹ�����������  
              options.c_cflag |= IXON | IXOFF | IXANY;  
              break;  
    }  
    //��������λ  
    //����������־λ  
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
              fprintf(stderr,"Unsupported data size\n");  
              return CRET_ERR;   
    }  
    //����У��λ  
    switch (pSerial->cBase->parity)  
    {    
       case 'n':  
       case 'N': //����żУ��λ��  
                 options.c_cflag &= ~PARENB;   
                 options.c_iflag &= ~INPCK;      
                 break;   
       case 'o':    
       case 'O'://����Ϊ��У��      
                 options.c_cflag |= (PARODD | PARENB);   
                 options.c_iflag |= INPCK;               
                 break;   
       case 'e':   
       case 'E'://����ΪżУ��    
                 options.c_cflag |= PARENB;         
                 options.c_cflag &= ~PARODD;         
                 options.c_iflag |= INPCK;        
                 break;  
       case 's':  
       case 'S': //����Ϊ�ո�   
                 options.c_cflag &= ~PARENB;  
                 options.c_cflag &= ~CSTOPB;  
                 break;   
        default:    
                 fprintf(stderr,"Unsupported parity\n");      
                 return CRET_ERR;   
    }   
    // ����ֹͣλ   
    switch (pSerial->cBase->stopBit)  
    {    
       case 1 :     
                options.c_cflag &= ~CSTOPB; break;   
       case 2 :     
                options.c_cflag |= CSTOPB; break;  
       default:     
                fprintf(stderr,"Unsupported stop bits\n");   
                return CRET_ERR;  
    }  
     
    //�޸����ģʽ��ԭʼ�������  
    options.c_oflag &= ~OPOST;  
    
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//�Ҽӵ�  
    //options.c_lflag &= ~(ISIG | ICANON);  
     
    //���õȴ�ʱ�����С�����ַ�  
    options.c_cc[VTIME] = 1; /* ��ȡһ���ַ��ȴ�1*(1/10)s */    
    options.c_cc[VMIN] = 1; /* ��ȡ�ַ������ٸ���Ϊ1 */  
     
    //�����������������������ݣ����ǲ��ٶ�ȡ ˢ���յ������ݵ��ǲ���  
    tcflush(fd,TCIFLUSH);  
     
    //�������� (���޸ĺ��termios�������õ������У�  
    if (tcsetattr(fd,TCSANOW,&options) != 0){  
        perror("com set error!\n");    
        return CRET_ERR;   
    }  

    return CRET_OK;   
}


int  CSerial_Open_Port(_PCSerial *pSerial)
{
	pSerial->fd = open( pSerial->cBase->port, O_RDWR|O_NOCTTY|O_NDELAY);  
	if ( 0 == pSerial->fd )  {  
		perror("Can't Open Serial Port");  
		return CRET_ERR;  
	}  
	//�ָ�����Ϊ����״̬                                 
	if( fcntl(pSerial->fd, F_SETFL, 0) < 0 )  {  
		printf("fcntl failed!\n");  
		return CRET_ERR;  
	}       
	else  {  
		printf("fcntl=%d\n",fcntl(pSerial->fd, F_SETFL,0));  
	}  
	//�����Ƿ�Ϊ�ն��豸      
	if( 0 == isatty(STDIN_FILENO) ) {  
		printf("standard input is not a terminal device\n");  
		return CRET_ERR;  
	}  
	else  {  
		printf("isatty success!\n");  
	}                
  
	return CRET_OK;  
}


void CSerial_Destroy_Port(_PCSerial *pSerial)
{
	//nothing to do
}


int CSerial_Write_Char(_PCSerial* pSerial, char* buf, int size)
{
	int len = 0;  
	
    len = write(pSerial->fd, buf, size);  
    if (len == size )  
	{  
		return len;  
	}       
    else     
	{  
		tcflush(fd,TCOFLUSH);  
		return FALSE; 
	}  
}

int CSerial_Read_Char(_PCSerial* pSerial, char* buf, int size)
{
	int len,fs_sel;  
    fd_set rFs;  
	
    struct timeval ts;  
	
    FD_ZERO(&rFs);  
    FD_SET(fd,&rFs);  
	
    ts.tv_sec = 10;  
    ts.tv_usec = 0;  
	
    //ʹ��selectʵ�ִ��ڵĶ�·ͨ��  
    fs_sel = select(pSerial->fd+1,&rFs,NULL,NULL,&ts);  
    if(fs_sel)  
	{  
		len = read(pSerial->fd,buf,size);  
		 
		return len;  
	}  
    else  
	{  
		
		return FALSE;  
    } 


}

void* CSerial_Loop(void* pData)
{
	_PCSerial *pSerial = (_PCSerial*)pData;

	if( !pSerial || pSerial->fd < 1 ){
		return 0;
	}

	pSerial->cBase->bThreadRun = 1;

	while ( pSerial->cBase->bThreadRun ) //ѭ����ȡ����  
	{    
		

		 
    }
	
	return 0;
}


#endif // NO AE_WINDOWS