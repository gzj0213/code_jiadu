/*
 * =====================================================================================
 *
 *       Filename:  libiso7816.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年07月15日 18时21分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian0517@gmail.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "libserial.h"
#include "libconv.h"
#include "libiso14443a.h"
#include "libiso7816.h"

/*
主机数据结构: 
起始码<0x09>+地址[2字节]+命令[2字节]+留用[2字节]+长度[2字节]+内容[不定]+校验[2字节]+结束码<0x0D> 

设备数据结构: 
起始码<0x0A>+地址[2字节]+命令[2字节]+状态[2字节]+长度[2字节]+内容[不定]+校验[2字节]+结束码<0x0E> 
*/

static int gsam_fd = -1;			/* 串口描述符 */

int	libiso7816_open(char *dev, int speed)
{
	if (gsam_fd < 0)
		gsam_fd = libserial_init(dev, speed, 'n', 8, 1, 0, 0);

	if (gsam_fd < 0)
		return -1;

	return 0;
}

int	libiso7816_close(void)
{
	int retval = -1;

	if (gsam_fd != -1)
		retval = libserial_close(gsam_fd);

	if (retval < 0)
		return retval;

	gsam_fd = -1;

	return 0;
}

int	libiso7816_get_fd(void)
{
	gsam_fd = libiso14443a_put_fd();

	return gsam_fd;
}

// 输出fd
int	libiso7816_put_fd(void)
{
	return gsam_fd;
}


int	libiso7816_socket_set(unsigned char socket, unsigned char rate_idx)
{
	int err;
	char send[32];
	char recv[32];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	/*Send: [0x09]01800002< 2字节socket ><2byte rate>< 2字节校验 >[0x0D]*/
	sprintf(send, "%c01800002%02X%02X00%c", 0x09, socket, rate_idx, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(gsam_fd, send, strlen(send));

	err = libserial_recv_package(gsam_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0){
		if (strstr(recv, "018000"))
			return 0;

		return -2;
	}

	return -1;
}

int	libiso7816_sam_reset(unsigned char socket, char *atr, unsigned char *atrlen)
{
	int err;
	char send[32];
	char recv[128];
	unsigned char data_len;

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	/*Send: [0x09]01810001< 2字节socket >< 2字节校验 >[0x0D]*/
	sprintf(send, "%c01810001%02X00%c", 0x09, socket, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(gsam_fd, send, strlen(send));

	err = libserial_recv_package(gsam_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0){
		if (strstr(recv, "018100")){
			libconv_ascs2hex((char *)&data_len, &recv[7], 2);
			if ((data_len!=0) && (atr!=NULL))
				libconv_ascs2hex(atr, &recv[9], data_len*2);
			if (atrlen != NULL)
				*atrlen = data_len;

			return 0;
		}

		return -2;
	}

	return -1;
}

int	libiso7816_exchange(unsigned char socket, 
			const char *in, const unsigned char inlen, 
			char *out, unsigned char *outlen)
{
	int err;
	char send[512];
	char recv[512];
	char data[255];
	unsigned char data_len;

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));
	bzero(data, sizeof(data));

	/*Send: [0x09]01820001< 2字节socket >< 2字节校验 >[0x0D]*/

	libconv_hex2str(data, in, inlen);
	sprintf(send, "%c018200%02X%02X%s00%c", 0x09, inlen+1, socket, data, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(gsam_fd, send, strlen(send));

	err = libserial_recv_package(gsam_fd, 0x0A, 0x0E, recv, sizeof(recv), 20);
	if (err > 0){
		if (strstr(recv, "018200")){
			libconv_ascs2hex((char *)&data_len, &recv[7], 2);
			/*printf("data_len=%d\n", data_len);*/
			if (data_len!=0)
				libconv_ascs2hex(data, &recv[9], data_len*2);
			if (out != NULL)
				memcpy(out, data, data_len);
			if (outlen != NULL)
				*outlen = data_len;
			/*printf("%02X, %02X\n", data[data_len-2], data[data_len-1]);*/
			if (((unsigned char)data[data_len-2]==0x90) 
			&& ((unsigned char)data[data_len-1]==0x00))
				return 0;

			return -3;
		}

		return -2;
	}

	return -1;
}
