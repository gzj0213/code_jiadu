/*
 * =====================================================================================
 *
 *       Filename:  libserial.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年03月10日 09时23分25秒 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *        Company:  
 *
 * =====================================================================================
 */

#include "libserial.h"

const int speed_arr[] = {B230400, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300,B0};
const int name_arr[] = {230400, 115200, 57600, 38400,  19200,  9600, 4800, 2400, 1200,  300, 0};

int libserial_save_set(int fd, struct termios *save_tios)
{
	if(tcgetattr(fd, save_tios) < 0)
		return -1;
	return 0;
}

int libserial_re_set(int fd, struct termios *save_tios)
{
	if(tcsetattr(fd, TCSANOW, save_tios) < 0)
		return -1;
	return 0;
}


int libserial_init(char *dev, int speed, int parity, int databits, int stopbits, int hwf, int swf)
{

	int fd = -1,i,ret; 
	struct termios tty; 

	fd = open(dev, O_RDWR | O_NOCTTY); 

	if(fd == -1){
		return -1; 
	}

	//    if (flock(fd, LOCK_EX) == -1){
	//        printf("cannot lock com %s\n",dev);
	//        close(fd); 
	//        return(-1);
	//    }

	ret = tcgetattr(fd, &tty);

	if(ret<0){
		close(fd); 
		return -1; 
	} 
	//设置波特率 
	for(i=0; i<sizeof(speed_arr)/sizeof(int); i++){
		if(speed == name_arr[i]){
			cfsetispeed(&tty, speed_arr[i]);
			cfsetospeed(&tty, speed_arr[i]);
			break; 
		}
		if(name_arr[i] == 0){ 
			printf("speed %d is not support,set to 9600\n",speed);
			cfsetispeed(&tty, B9600);
			cfsetospeed(&tty, B9600);
		}
	}

	//设置数据位 
	switch (databits){
		case 5: 
			tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5;
			break; 
		case 6: 
			tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6;
			break; 
		case 7: 
			tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7;
			break; 
		case 8: 
		default: 
			tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
			break; 
	}

	//设置停止位 
	if(stopbits == 2) 
		tty.c_cflag |= CSTOPB; 
	else 
		tty.c_cflag &= ~CSTOPB;

	//设置奇偶校验 
	switch (parity){ 
		//无奇偶校验 
		case 'n': 
		case 'N': 
			tty.c_cflag &= ~PARENB;   /* Clear parity enable */ 
			tty.c_iflag &= ~INPCK;    /* Enable parity checking */ 
			break; 
			//奇校验 
		case 'o': 
		case 'O': 
			tty.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/ 
			tty.c_iflag |= INPCK;             /* Disable parity checking */ 
			break; 
			//偶校验 
		case 'e': 
		case 'E': 
			tty.c_cflag |= PARENB;    /* Enable parity */ 
			tty.c_cflag &= ~PARODD;   /* 转换为偶效验*/ 
			tty.c_iflag |= INPCK;     /* Disable parity checking */ 
			break; 
			//等效于“无奇偶校验” 
		case 'S': 
		case 's':  /*as no parity*/ 
			tty.c_cflag &= ~PARENB; 
			tty.c_cflag &= ~CSTOPB; 
			break; 
		default: 
			tty.c_cflag &= ~PARENB;   /* Clear parity enable */ 
			tty.c_iflag &= ~INPCK;    /* Enable parity checking */ 
			break; 
	}

	//设置硬件流控制 
	if (hwf) 
		tty.c_cflag |= CRTSCTS; 
	else 
		tty.c_cflag &= ~CRTSCTS; 

	/*if (hwf) */
	/*tty.c_cflag |= CIREN;*/

	//设置软件流控制 
	if (swf)
		tty.c_iflag |= IXON | IXOFF;
	else 
		tty.c_iflag &= ~(IXON|IXOFF|IXANY); 

	//设置为RAW模式 
	tty.c_iflag &= ~(IGNBRK | IGNCR | INLCR | ICRNL | IUCLC | 
			IXANY | IXON | IXOFF | INPCK | ISTRIP); 
	tty.c_iflag |= (BRKINT | IGNPAR); 
	tty.c_oflag &= ~OPOST; 
	tty.c_lflag &= ~(XCASE|ECHONL|NOFLSH); 
	tty.c_lflag &= ~(ICANON | ISIG | ECHO); 
	tty.c_cflag |= (CLOCAL | CREAD); 

	//tty.c_cc[VTIME] = 1;   /*（非阻塞模式）操作超时，单位为0.1秒*/ 
	//tty.c_cc[VMIN] = 1;   /* 1为阻塞模式，0为非阻塞模式*/ 
	tty.c_cc[VTIME] = 2;   /*（非阻塞模式）操作超时，单位为0.1秒*/ 
	tty.c_cc[VMIN] = 0;   /* 1为阻塞模式，0为非阻塞模式*/ 

	//设置串口属性 
	ret = tcsetattr(fd, TCSANOW, &tty);

	if(ret<0){ 
		close(fd); 
		return -1; 
	}

	tcflush(fd, TCIOFLUSH); 

	return fd; 
}

int libserial_close(int fd)
{
	int status;

	status = close(fd);

	return(status);
}

void libserial_chang_baud(int fd, int speed)
{
	int i;
	struct termios tty;

	// 取得当前串口参数
	tcgetattr(fd,&tty);

	//设置波特率 
	for(i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++){
		if(speed == name_arr[i]){ 
			cfsetispeed(&tty, speed_arr[i]); // 输入波特率
			cfsetospeed(&tty, speed_arr[i]); // 输出波特率
			break; 
		} 
		if(name_arr[i] == 0){ 
			printf("speed %d is not support,set to 115200\n",speed); 
			cfsetispeed(&tty, B115200); 
			cfsetospeed(&tty, B115200); 
		} 
	}

	// 设置新的串口参数
	tcsetattr(fd,TCSANOW,&tty);
}

void libserial_null_read(int fd)
{
	struct termios options;
	struct termios old;
	int rc;
	char tmp[128];

	libserial_save_set(fd, &old);

	// 设置超时时间
	tcgetattr(fd, &options);
	options.c_cc[VTIME]=1;
	tcsetattr(fd, TCSANOW, &options);

	while (1){
		rc = read(fd, tmp, sizeof(tmp));
		//if (rc != 1) break;
		if (rc <= 0) break;
	}

	libserial_re_set(fd, &old);
}

void libserial_set_rts(int fd, int st)
{
	int linedata;

	ioctl(fd, TIOCMGET, &linedata);

	if (st){
		linedata |= TIOCM_RTS;
	}
	else {
		linedata &= ~TIOCM_RTS;
	}

	ioctl(fd, TIOCMSET, &linedata);
	usleep(2);

}


int libserial_send_package(int fd, const char *buf, const int buflen)
{
	// 刷新读写数据
	/*tcflush(fd, TCIOFLUSH);*/

	return write(fd, buf, buflen);
}

int libserial_recv_package(int fd, const char begin, const char end, char *buf, int buflen, const int tmout)
{
	struct termios old, curr;
	int rc = 0,record = 0;
	int i = 0;
	char c;
	int result = -1;

	libserial_save_set(fd, &old);

	tcgetattr(fd, &curr);
	curr.c_cc[VTIME] = tmout*10;
	tcsetattr(fd, TCSANOW, &curr);

	while (1){
		rc = read(fd, &c, 1); // 接收字符

		/*printf("c=0x%02X\n", c);*/
		if (rc != 1){
			// 超时
			/*printf("sr tmout\n");*/
			result = 0;
			break;
		}

		if (c == begin) record=1; // 收到起始字符

		if (1 == record){ // 开始记录数据包
			buf[i++] = c;
			if (buflen == i){
				result = -1;
				/*printf("\n");*/
				break; // 数据包超长
			}
		}

		if (c == end){
			result = i;
			break;
		}
	}

	libserial_re_set(fd, &old);

	return result;
}
