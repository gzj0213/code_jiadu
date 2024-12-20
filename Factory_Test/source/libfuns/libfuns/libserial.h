/*
 * =====================================================================================
 *
 *       Filename:  libserial.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011��03��10�� 09ʱ23��25�� UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __LIBSERIAL_H__
#define __LIBSERIAL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>

extern int libserial_save_set(int fd, struct termios *save_tios);



extern int libserial_re_set(int fd, struct termios *save_tios);



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  libserial_init
 *  Description:  ��ʼ�����нӿ�
 * 					dev�������豸��
 * 					speed�����ڲ����ʣ�������115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300, 0��
 * 					parity����żУ�顣ֵΪ'N','E','O','S'�� 
 * 					databits������λ��ֵ��5��6��7����8, (��չ����ֻ����8)��
 * 					stopbits��ֹͣλ��ֵ��1����2�� 
 * 					hwf��Ӳ�������ơ�1Ϊ�򿪣�0Ϊ�رա�
 * 					swf�����������ơ�1Ϊ�򿪣�0Ϊ�رա�
 * =====================================================================================
 */
extern int libserial_init(char *dev, int speed, int parity, int databits, int stopbits, int hwf, int swf);



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  libserial_close
 *  Description:  �رմ��нӿ�
 *  Param:			fd - �򿪵Ľӿھ��
 * =====================================================================================
 */
extern int libserial_close(int fd);



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  libserial_change_baud
 *  Description:  ���ô򿪴��ڲ�����
 *  Param:			fd - �򿪵Ľӿھ��
 *  				speed - ������
 * =====================================================================================
 */
extern void libserial_chang_baud(int fd, int speed);


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  libserial_null_read
 *  Description:  ��մ��нӿڻ�������
 *  Param:			fd - �򿪵Ľӿھ��
 * =====================================================================================
 */
extern void libserial_null_read(int fd);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  libserial_set_rts
 *  Description:  ����rts��,��ʱ����
 *  Param:			fd - �򿪵Ľӿھ��
 * =====================================================================================
 */
extern void libserial_set_rts(int fd, int st);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  
 *  Description:  
 *  Param:
 * =====================================================================================
 */
extern int libserial_send_package(int fd, const char *buf, const int buflen);


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  
 *  Description:  
 *  Param:
 * =====================================================================================
 */
extern int libserial_recv_package(int fd, const char begin, const char end, char *buf, int buflen, const int tmout);

#endif // #ifndef __SERIAL_H__
