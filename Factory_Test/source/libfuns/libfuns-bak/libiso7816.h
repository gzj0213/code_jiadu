/*
 * =====================================================================================
 *
 *       Filename:  libiso7816.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年07月15日 18时22分06秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian0517@gmail.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#ifndef __LIBISO7816_H__
#define __LIBISO7816_H__

// SAM卡槽号定义
#define		SAM_SEL_SK1		0
#define		SAM_SEL_SK2		1
#define		SAM_SEL_SK3		2
#define		SAM_SEL_SK4		3


// SAM卡波特率定义
#define		SAM_B9600		0
#define		SAM_B19200		1
#define		SAM_B38400		2
#define		SAM_B57600		3
#define		SAM_B115200		4

extern int	libiso7816_open(char *dev, int speed);

extern int	libiso7816_get_fd(void);

extern int	libiso7816_put_fd(void);

extern int	libiso7816_socket_set(unsigned char socket, unsigned char rate_idx);

extern int	libiso7816_sam_reset(unsigned char socket, char *atr, unsigned char *atrlen);

extern int	libiso7816_exchange(unsigned char socket, 
			const char *in, const unsigned char inlen, 
			char *out, unsigned char *outlen);

#endif
