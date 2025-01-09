/*
 * =====================================================================================
 *
 *       Filename:  libiso14443hex.c
 *
 *    Description:  读写器iso14443a 16进制库
 *
 *        Version:  1.0
 *        Created:  2011年09月21日 17时00分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian0517@gmail.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#include <string.h>
#include <stdio.h>

#include "libtypes.h"
#include "libmisc.h"
#include "libserial.h"
#include "libconv.h"
#include "libiso14443hex.h"
#include "libiso7816.h"

// 通讯包头
#define		PACKHEAD_AA		0xAA
#define		PACKHEAD_BB		0xBB
// 设备标示符
#define		MACHID_L		0x00
#define		MACHID_H		0x00

// 指令码
#define	ISO14443_CMD_INIT		0x30
#define ISO14443_CMD_SCK_SET		0x80
#define ISO14443_CMD_SAM_RST		0x81
#define ISO14443_CMD_SAM_CMD		0x82


/* 
 *****************************************************************
 *  上位机命令格式
 *  命令头 + 长度字 + 设备标识符 ＋ 命令码 + 命令内容 + 校验字
 *  命令头	：2BYTE，0xAABB
 *  长度字	：2BYTE(高位在前)，从设备标识符到校验字的字节数
 *  设备标识	：2BYTE
 *  命令码	：1BYTE
 *  命令内容	：命令报文（可以为空）
 *  校验字	：1BYTE、从长度字到命令内容的逐字节异或
 *****************************************************************
 *  模块应答格式
 *  命令头 + 长度字 + 设备标识符＋命令码 + 状态字 + 数据 + 校验字
 *  命令头	：2BYTE，0xAABB 
 *  长度字	：2BYTE(高位在前)，从设备标识符到校验字的字节数
 *  设备标识	：2BYTE
 *  命令码	：1BYTE
 *  状态字	：1BYTE 、0 = 成功；
 *  数据	：返回的数据
 *  校验字	：1BYTE、从长度字到数据内容的逐字节异或
 *****************************************************************
 */

static int grf_fd = -1;			/* 串口描述符 */

int	libiso14443hex_open(char *dev, int speed)
{
	if (grf_fd < 0)
		grf_fd = libserial_init(dev, speed, 'n', 8, 1, 0, 0);

	if (grf_fd < 0)
		return -1;

	return 0;
}

int	libiso14443hex_close(void)
{
	int retval = -1;

	if (grf_fd != -1)
		retval = libserial_close(grf_fd);

	if (retval < 0)
		return retval;

	grf_fd = -1;

	return 0;
}

// 输出fd
int	libiso14443hex_put_fd(void)
{
	return grf_fd;
}

int	libiso14443hex_sendpack(uint8_t cmd, char *buf, uint16_t buflen)
{
	char		chk;
	char		send[8];
	u8_to_u16_t 	len;
//	int		i;

	memset(send, 0, sizeof(send));

	len.u16 = 4+buflen;

//	if (len.u16 > 500)
//		return -1;

	send[0] = PACKHEAD_AA;
	send[1] = PACKHEAD_BB;

	send[2] = len.u8[1];
	send[3] = len.u8[0];
	send[4] = MACHID_L;
	send[5] = MACHID_H;
	send[6] = cmd;

	chk = len.u8[1]^len.u8[0]^MACHID_L^MACHID_H^cmd;
	if ((buf != NULL) && (buflen != 0))
		chk ^= libmisc_calc_xorval((const uint8_t *)buf, (const int)buflen);

//	printf("SEND:");
//	for (i=0; i<7; i++){
//		printf("%02X", (unsigned char)send[i]);
//	}
//	printf("\n");

	libserial_send_package(grf_fd, send, 7);

	if ((buf != NULL) && (buflen != 0))
		libserial_send_package(grf_fd, buf, buflen);

	libserial_send_package(grf_fd, &chk, 1);

	return 0;
}


int	libiso14443hex_recvpack(piso14443hex_package_t ack, const int tmout)
{
	bool			recv_ok_flg;
	bool			recvAA_flg;
	bool			recv_start_flg;
	uint8_t 		chksum;
	uint8_t			buf[500];
	uint8_t			c;
	ssize_t 		rc;
	uint16_t 		i;
	uint16_t 		uLen;
	uint16_t		recv_cnt;
	int			retval = LIBISO14443_ERR_TIMOUT;
	struct termios old, curr;

	libserial_save_set(grf_fd, &old);

	tcgetattr(grf_fd, &curr);
	curr.c_cc[VTIME] = tmout*10;
	tcsetattr(grf_fd, TCSANOW, &curr);

	chksum = 0;
	recv_cnt = 0;
	recvAA_flg = 0;
	recv_start_flg = 0;
	recv_ok_flg = 0;
	memset(buf, 0, sizeof(buf));

	while(1){
		rc = read(grf_fd, &c, 1);

		if (rc != 1){
			// 超时
			retval = LIBISO14443_ERR_TIMOUT;
			break;
		}

		if(recvAA_flg){
			recvAA_flg = 0;
			if (PACKHEAD_BB == c){
				recv_start_flg = 1;
				recv_cnt = 0;
			}
		}
		else{
			if ((PACKHEAD_AA == c) && (recv_start_flg == 0)){
				recvAA_flg = 1;
			}

			if (recv_start_flg){
				buf[recv_cnt++] = c;

				uLen = buf[0]*0x100+buf[1]+2;
				if (recv_cnt == uLen){
					//Device ID 一致
					if ((buf[2] == MACHID_L) && (buf[3] == MACHID_H)){
						/*printf("RECV:");*/
						for (i=0; i<uLen; i++){
							/*printf("%02X", buf[i]);*/
							chksum ^= buf[i];
						}
						/*printf("\n");*/

						// 检查校验码
						if (0 == chksum){
							retval = LIBISO14443_ERR_NONE;
						}
						else{
							retval = LIBISO14443_ERR_CHKSUM;
						}
						break;
					}
					// 设备标识不匹配
					retval = LIBISO14443_ERR_ID;
					break;
				}
				if (recv_cnt >= sizeof(buf)){
					// 缓冲长度超出
					retval = LIBISO14443_ERR_LEN;
					break;
				}
			}
		}
	}

	libserial_re_set(grf_fd, &old);

	// 分解数据
	if (retval == LIBISO14443_ERR_NONE){
		// 检查最小长度
		if (recv_cnt < 7)
			retval = LIBISO14443_ERR_MINLEN;
		else{
			ack->cmd = buf[4];
			ack->status = buf[5];
			ack->buflen = recv_cnt-7;
			if ((ack->buflen != 0) && (ack->buflen <= RECV_PACKBUF_MAX_NUM))
				memcpy(ack->buf, &buf[6], ack->buflen);
			if (ack->status != 0)
				retval = ack->status;
		}
	}

	return retval;
}

int	libiso14443hex_init(void)
{
	iso14443hex_package_t	ack;

	libiso14443hex_sendpack(ISO14443_CMD_INIT, NULL, 0);

	return libiso14443hex_recvpack(&ack, 1);
}

int	libiso14443hex_sam_socket_set(uint8_t socket, uint8_t rate_idx)
{
	iso14443hex_package_t	ack;
	char	buf[2];

	buf[0] = socket;
	buf[1] = rate_idx;

	libiso14443hex_sendpack(ISO14443_CMD_SCK_SET, buf, 2);

	return libiso14443hex_recvpack(&ack, 1);
}

int	libiso14443hex_sam_reset(uint8_t socket, char *atr, uint8_t *atrlen)
{
	int retval;
	iso14443hex_package_t	ack;

	libiso14443hex_sendpack(ISO14443_CMD_SAM_RST, (char *)&socket, 1);

	retval = libiso14443hex_recvpack(&ack, 2);
	if (retval == 0){
		if (atrlen != NULL)
			*atrlen = ack.buflen;
		if (atr != NULL)
			memcpy(atr, ack.buf, ack.buflen);
	}

	return retval;
}

int	libiso14443hex_sam_exchange(unsigned char socket, 
			const char *in, const unsigned char inlen, 
			char *out, unsigned char *outlen)
{
	int retval;
	iso14443hex_package_t	ack;
	char buf[500];

	memset(buf, 0, sizeof(buf));
	buf[0] = socket;
	memcpy(&buf[1], in, inlen);

	libiso14443hex_sendpack(ISO14443_CMD_SAM_CMD, buf, 1+inlen);

	retval = libiso14443hex_recvpack(&ack, 2);
	if (retval == 0){
		if (outlen != NULL)
			*outlen = ack.buflen;
		if (out != NULL)
			memcpy(out, ack.buf, ack.buflen);

		if ((ack.buf[ack.buflen-2] == 0x90)
		&& (ack.buf[ack.buflen-1] == 0x00))
			return 0;
		else
			return LIBISO14443_ERR_SAM_ST;
	}

	return retval;
}
