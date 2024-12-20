/*
 * =====================================================================================
 * 
 *       Filename:  libiso14443a.h
 * 
 *    Description:  读写器底层通讯协议
 * 
 *        Version:  1.0
 *        Created:  2007年08月07日 16时32分46秒 UTC
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  ZhouJian (), zhoujian@revenco.com
 *        Company:  Goldsign
 * 
 * =====================================================================================
 */

#ifndef  __LIBISO14443A_H__
#define  __LIBISO14443A_H__

#define PICC_AUTHA        	0x60       /*验证A密钥*/
#define PICC_AUTHB        	0x61       /*验证B密钥*/

#define PICC_DEC		0xC0       /*扣款*/
#define PICC_INC		0xC1       /*充值*/


extern int	libiso14443a_open(char *dev, int speed);

extern int	libiso14443a_close(void);

extern int	libiso14443a_put_fd(void);

extern int	libiso14443a_get_fd(void);

extern int	libiso14443a_init(void);

extern int	libiso14443a_request(unsigned char req_code, unsigned char *atq);

extern int	libiso14443a_anticoll(unsigned char select_code, unsigned char bcnt, unsigned char *snr);

extern int	libiso14443a_select(unsigned char select_code, unsigned char *snr, unsigned char *sak);

extern int	libiso14443a_authentication(unsigned char auth_mode, const unsigned char *snr, 
				const unsigned char *keys,
				const unsigned char block);

extern int	libiso14443a_readblock(unsigned char blknum, unsigned char *block_data);

extern int	libiso14443a_writeblock(const unsigned char blknum, const unsigned char *block_data);

extern int	libiso14443a_value(unsigned char mode, unsigned long value);

extern int	libiso14443a_increment(unsigned int val);

extern int	libiso14443a_decrement(unsigned int val);

extern int	libiso14443a_transfer(unsigned char blknum);

extern int	libiso14443a_restore(unsigned char blknum);

extern int	libiso14443a_halt(void);

#endif

