/*
 * =====================================================================================
 *
 *       Filename:  libiso14443hex.h
 *
 *    Description:  读写器头文件hex格式
 *
 *        Version:  1.0
 *        Created:  2011年09月21日 18时00分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian0517@gmail.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#ifndef __LIBISO14443HEX_H__
#define __LIBISO14443HEX_H__

// 错误代码表
#define LIBISO14443_ERR_NONE		0	// 成功
#define	LIBISO14443_ERR_TIMOUT		-1	// 通讯超时
#define LIBISO14443_ERR_CHKSUM		-2	// 校验错误
#define LIBISO14443_ERR_ID		-3	// 设备标识不匹配
#define	LIBISO14443_ERR_LEN		-4	// 缓冲溢出
#define LIBISO14443_ERR_MINLEN		-5	// 数据包字段不够
#define LIBISO14443_ERR_SAM_ST		-6	// sam执行失败

#define	RECV_PACKBUF_MAX_NUM	500

typedef struct libiso14443hex_package{
	uint8_t		cmd;		// 命令码
	int		status;		// 状态字
	uint16_t	buflen;		// 数据长度
	uint8_t		buf[RECV_PACKBUF_MAX_NUM];	// 数据
}iso14443hex_package_t;
typedef iso14443hex_package_t* piso14443hex_package_t;


extern int	libiso14443hex_open(char *dev, int speed);

extern int	libiso14443hex_close(void);

extern int	libiso14443hex_put_fd(void);

extern int	libiso14443hex_sendpack(uint8_t cmd, char *buf, uint16_t buflen);

extern int	libiso14443hex_recvpack(piso14443hex_package_t ack, const int tmout);

extern int	libiso14443hex_init(void);

extern int	libiso14443hex_sam_socket_set(uint8_t socket, uint8_t rate_idx);

extern int	libiso14443hex_sam_reset(uint8_t socket, char *atr, uint8_t *atrlen);

extern int	libiso14443hex_sam_exchange(unsigned char socket, 
			const char *in, const unsigned char inlen, 
			char *out, unsigned char *outlen);

#endif
