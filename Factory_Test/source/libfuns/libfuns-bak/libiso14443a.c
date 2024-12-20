/*
 * =====================================================================================
 *
 *       Filename:  libiso14443a.c
 *
 *    Description:  ISO14443A协议
 *
 *        Version:  1.0
 *        Created:  2007年06月03日 11时53分27秒 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian@revenco.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#include <string.h>
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

static int grf_fd = -1;			/* 串口描述符 */

int	libiso14443a_open(char *dev, int speed)
{
	if (grf_fd < 0)
		grf_fd = libserial_init(dev, speed, 'n', 8, 1, 0, 0);

	if (grf_fd < 0)
		return -1;

	return 0;
}

int	libiso14443a_close(void)
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
int	libiso14443a_put_fd(void)
{
	return grf_fd;
}

//  获取fd
int	libiso14443a_get_fd(void)
{
	grf_fd = libiso7816_put_fd();

	return grf_fd;
}


int	libiso14443a_init(void)
{
	int err = -1;
	char send[32];
	char recv[32];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	/*Send: [0x09]0130000084[0x0D]*/
	sprintf(send, "%c0130000084%c", 0x09, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0) {
		if (strstr(recv, "013000")){
			return 0;
		}
		
		return -2;
	}

	return -1;
}

int	libiso14443a_request(unsigned char req_code, unsigned char *atq)
{
	int err;
	char send[32];
	char recv[32];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	/*Send: [0x09]0131000085[0x0D]*/

	sprintf(send, "%c0131000085%c", 0x09, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0){
		if (strstr(recv, "013100")){
			if (atq != NULL)
				libconv_ascs2hex((char *)atq, &recv[9], 2);
			return 0;
		}

		return -2;
	}

	return -1;
}

int	libiso14443a_anticoll(unsigned char select_code, unsigned char bcnt, unsigned char *snr)
{
	int err;
	char send[32];
	char recv[32];
	char strsnr[9];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	/*Send: [0x09]0132000086[0x0D]*/

	sprintf(send, "%c0132000085%c", 0x09, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	/*Recv: [0x0A]01320004< 8字节序列号 >< 2字节校验 >[0x0E]*/
	if (err > 0){
		if (strstr(recv, "013200")){
			if (snr != NULL){
				bzero(strsnr, sizeof(strsnr));
				memcpy(strsnr, &recv[9], 8);
				libconv_ascs2hex((char *)snr, strsnr, strlen(strsnr));
			}
			return 0;
		}
		return -2;
	}

	return -1;

}

int	libiso14443a_select(unsigned char select_code, unsigned char *snr, unsigned char *sak)
{
	int err;
	char send[32];
	char recv[32];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	/*Send: [0x09]01330004< 8字节序列号 >< 2字节校验 >[0x0D] */ // 取消
	/*Send: [0x09]01330004< 2字节校验 >[0x0D] */
	sprintf(send, "%c0133000000%c", 0x09, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	/*Recv: [0x0A]01330001< 2字节ask >< 2字节校验 >[0x0E]*/
	if (err > 0){
		if (strstr(recv, "013300")){
			if (sak != NULL)
				libconv_ascs2hex((char *)sak, &recv[9], 2);
			return 0;
		}

		return -2;
	}

	return -1;
}

int	libiso14443a_authentication(unsigned char auth_mode, const unsigned char *snr, 
				const unsigned char *keys,
				const unsigned char block)
{
	int err;
	char send[32];
	char recv[32];
	char strkeys[13];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));
	bzero(strkeys, sizeof(strkeys));

	libconv_hex2str(strkeys, (char *)keys, 6);
	/*Send: [0x09]013D0008< 2字节密钥类型 >< 2字节块号 ><12字节密钥>< 2字节校验 >[0x0D]*/
	sprintf(send, "%c013D0008%02X%02X%s00%c", 0x09, auth_mode, block, strkeys, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0){
		if (strstr(recv, "013D00")){ /*需要修改*/
			return 0;
		}
		return -2;
	}

	return -1;
}

int	libiso14443a_readblock(unsigned char blknum, unsigned char *block_data)
{
	int err;
	char send[32];
	char recv[64];
	char data[33];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	/*Send: [0x09]01350001< 2字节块号 >< 2字节校验 >[0x0D]*/
	sprintf(send, "%c01350001%02X00%c", 0x09, blknum, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0){
		if (strstr(recv, "013500")){
			bzero(data, sizeof(data));
			memcpy(data, &recv[9], 32);
			libconv_ascs2hex((char *)block_data, data, strlen(data));
			return 0;
		}

		return -2;
	}

	return -1;
}

int	libiso14443a_writeblock(const unsigned char blknum, const unsigned char *block_data)
{
	int err;
	char send[64];
	char recv[33];
	char strdata[33];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));
	bzero(strdata, sizeof(strdata));

	/*Send: [0x09]01360011< 2字节块号 >< 32字节内容数据 >< 2字节校验 >[0x0D] */
	libconv_hex2str(strdata, (char *)block_data, 16);
	sprintf(send, "%c01360011%02X%s00%c", 0x09, blknum, strdata, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0){
		if (strstr(recv, "013600")){
			return(0);
		}
		return(-2);
	}

	return(-1);
}

int	libiso14443a_value(unsigned char mode, unsigned long value)
{
	if (mode == PICC_INC){
		return libiso14443a_increment(value);
	}
	else if (mode == PICC_DEC){
		return libiso14443a_decrement(value);
	}

	return -1;
}

int	libiso14443a_increment(unsigned int val)
{
	int err;
	char send[32];
	char recv[32];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	sprintf(send, "%c01390004%04X00%c", 0x09, val, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0){
		if (strstr(recv, "013900")){
			return(0);
		}
		return(-2);
	}

	return(-1);
}

int	libiso14443a_decrement(unsigned int val)
{
	int err;
	char send[32];
	char recv[32];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	sprintf(send, "%c013A0004%04X00%c", 0x09, val, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0){
		if (strstr(recv, "013A00")){
			return(0);
		}
		return(-2);
	}

	return -1;
}

int	libiso14443a_transfer(unsigned char blknum)
{
	int err;
	char send[32];
	char recv[32];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	/*Send: [0x09]01380001< 2字节块号 >< 2字节校验 >[0x0D]*/
	sprintf(send, "%c01380001%02X00%c", 0x09, blknum, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0){
		if (strstr(recv, "013800")){
			return(0);
		}
		return(-2);
	}

	return -1;
}

int	libiso14443a_restore(unsigned char blknum)
{
	int err;
	char send[32];
	char recv[32];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	/*Send: [0x09]01370001< 2字节块号 >< 2字节校验 >[0x0D]*/
	sprintf(send, "%c01370001%02X00%c", 0x09, blknum, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0){
		if (strstr(recv, "013700")){
			return(0);
		}
		return(-2);
	}

	return -1;
}

int	libiso14443a_halt(void)
{
	int err;
	char send[32];
	char recv[32];

	bzero(send, sizeof(send));
	bzero(recv, sizeof(recv));

	/*Send: [0x09]013B000096[0x0D]*/
	sprintf(send, "%c013B000000%c", 0x09, 0x0D);
	libconv_add_bcc(send, strlen(send));
	libserial_send_package(grf_fd, send, strlen(send));

	err = libserial_recv_package(grf_fd, 0x0A, 0x0E, recv, sizeof(recv), 3);
	if (err > 0){
		if (strstr(recv, "013B00")){
			return(0);
		}
		return(-2);
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////





#if 0
int iso14443a_init(int fd)
{
    int st;
    char send[32]; // senddata
    char recv[32]; // recvdata

    bzero(send, sizeof(send));
    bzero(recv, sizeof(recv));

    /*sprintf(send, "%c0130000084%c", 0x09, 0x0D);*/
    sprintf(send, "%c01300000FF%c", 0x09, 0x0D);
    add_bcc(send, strlen(send));
    libserial_send_package(fd, send, strlen(send));

    st = libserial_recv_package(fd, 0x0A, 0x0E, recv, sizeof(recv), 4);
    
    if (st > 0){
        if (strstr(recv, "013000")){
            return(0);
        }
        return(-2);
    }

    return(-1);
}

int iso14443a_request(int fd)
{
    int st;
    char send[32];
    char recv[32];

    bzero(send, sizeof(send));
    bzero(recv, sizeof(recv));

    sprintf(send, "%c01310000FF%c", 0x09, 0x0D);
    add_bcc(send, strlen(send));
    libserial_send_package(fd, send, strlen(send));

    st = libserial_recv_package(fd, 0x0A, 0x0E, recv, sizeof(recv), 4);
    
    if (st > 0){
        if (strstr(recv, "013100")){
            return(0);
        }
        return(-2);
    }

    return(-1);
}

//********************************************************/
// 函数名:  防冲突
//********************************************************/
int iso14443a_anticoll(int fd, char *physics_id)
{
    int st;
    char send[32];
    char recv[32];

    bzero(send, sizeof(send));
    bzero(recv, sizeof(recv));

    sprintf(send, "%c01320000FF%c", 0x09, 0x0D);
    add_bcc(send, strlen(send));
    libserial_send_package(fd, send, strlen(send));

    st = libserial_recv_package(fd, 0x0A, 0x0E, recv, sizeof(recv), 4);
    
    if (st > 0){
        if (strstr(recv, "013200")){
            memcpy(physics_id, &recv[9], 8);
            return(0);
        }
        return(-2);
    }

    return(-1);
}



//********************************************************/
// 函数名:  选卡
// 参  数: 
// 返回值: 
//       
// 功  能：
//********************************************************/
int ISO1443A_SELECT(int iSerial, char *szCardID)
{
    int st;
    char SendData[32];
    char RecvData[32];

    memset(SendData, 0, sizeof(SendData));
    memset(RecvData, 0, sizeof(RecvData));

    sprintf(SendData, "%c01330004%s00%c", 0x09,szCardID,0x0D);
    BCC(SendData);

    Com_SendPack(iSerial, SendData);
      
    st = Com_RecvPack(iSerial, 0x0A, 0x0E, RecvData, 3);
    #ifdef DEBUG
      fprintf(stdout,"Send Final \n");
    #endif
    
    if (st == 0)
    {
        if (strstr(RecvData, "013300"))
        {
            #ifdef DEBUG
            fprintf(stdout, "SUCCEED SELECT\n");
            #endif
            return(0);
        }
        #ifdef DEBUG
        fprintf(stdout, "FAIL, return: %s\n", RecvData);
        #endif
        return(-2);
    }

    return(-1);
}

//********************************************************/
// 函数名:  认证
// 参  数: 
// 返回值: 
//       
// 功  能：
//********************************************************/
int ISO1443A_AUTH(int iSerial, int iBlock, int iKeyType)
{
    int st;
    char SendData[32];
    char RecvData[32];

    memset(SendData, 0, sizeof(SendData));
    memset(RecvData, 0, sizeof(RecvData));

    sprintf(SendData, "%c01340002%02X%02X00%c", 0x09,iKeyType,iBlock,0x0D);
    BCC(SendData);

    Com_SendPack(iSerial, SendData);
      
    st = Com_RecvPack(iSerial, 0x0A, 0x0E, RecvData, 3);
    #ifdef DEBUG
      fprintf(stdout,"Send Final \n");
    #endif
    
    if (st == 0)
    {
        if (strstr(RecvData, "013400"))
        {
            #ifdef DEBUG
            fprintf(stdout, "SUCCEED\n");
            #endif
            return(0);
        }
        #ifdef DEBUG
        fprintf(stdout, "FAIL, return: %s\n", RecvData);
        #endif
        return(-2);
    }

    return(-1);
}

//********************************************************/
// 函数名:  认证
// 参  数: 
// 返回值: 
//       
// 功  能：
//********************************************************/
int ISO1443A_AUTH2(int iSerial, int iBlock, int iKeyType, char *szKey)
{
    int st;
    char SendData[32];
    char RecvData[32];

    memset(SendData, 0, sizeof(SendData));
    memset(RecvData, 0, sizeof(RecvData));

    sprintf(SendData, "%c013D0008%02X%02X%s00%c", 0x09, iKeyType, iBlock, szKey, 0x0D);
    BCC(SendData);
    if (debug) printf("ISO1443A_AUTH2 Snd: %s\n", SendData);

    Com_SendPack(iSerial, SendData);
      
    st = Com_RecvPack(iSerial, 0x0A, 0x0E, RecvData, 3);
    #ifdef DEBUG
      fprintf(stdout,"Send Final \n");
    #endif
    
    if (st == 0)
    {
        if (strstr(RecvData, "013D00"))
        {
            if (debug) printf("ISO1443A return SUCCEED\n");
            return(0);
        }
        if (debug) fprintf(stdout, "FAIL, return: %s\n", RecvData);
        return(-2);
    }

    return(-1);
}



//********************************************************/
// 函数名:  读块数据
// 参  数: 
// 返回值: 
//       
// 功  能：
//********************************************************/
int ISO1443A_READBLOCK(int iSerial, int iBlock, char *szData)
{
    int st;
    char SendData[32];
    char RecvData[33];

    memset(SendData, 0, sizeof(SendData));
    memset(RecvData, 0, sizeof(RecvData));

    sprintf(SendData, "%c01350001%02X00%c", 0x09,iBlock,0x0D);
    BCC(SendData);

    Com_SendPack(iSerial, SendData);
      
    st = Com_RecvPack(iSerial, 0x0A, 0x0E, RecvData, 3);
    #ifdef DEBUG
      fprintf(stdout,"Send Final \n");
    #endif
    
    if (st == 0)
    {
        if (strstr(RecvData, "013500"))
        {
            #ifdef DEBUG
            fprintf(stdout, "SUCCEED\n");
            #endif
            memcpy(szData,&RecvData[9],32);
            return(0);
        }
        #ifdef DEBUG
        fprintf(stdout, "FAIL, return: %s\n", RecvData);
        #endif
        return(-2);
    }

    return(-1);
}

//********************************************************/
// 函数名:  写块数据
// 参  数: 
// 返回值: 
//       
// 功  能：
//********************************************************/
int ISO1443A_WRITEBLOCK(int iSerial, int iBlock, char *szData)
{
    int st;
    char SendData[32];
    char RecvData[33];

    memset(SendData, 0, sizeof(SendData));
    memset(RecvData, 0, sizeof(RecvData));

    sprintf(SendData, "%c01360011%02X%s00%c", 0x09,iBlock,szData,0x0D);
    BCC(SendData);

    Com_SendPack(iSerial, SendData);
      
    st = Com_RecvPack(iSerial, 0x0A, 0x0E, RecvData, 3);
    #ifdef DEBUG
      fprintf(stdout,"Send Final \n");
    #endif
    
    if (st == 0)
    {
        if (strstr(RecvData, "013600"))
        {
            #ifdef DEBUG
            fprintf(stdout, "SUCCEED\n");
            #endif
            return(0);
        }
        #ifdef DEBUG
        fprintf(stdout, "FAIL, return: %s\n", RecvData);
        #endif
        return(-2);
    }

    return(-1);
}

//********************************************************/
// 函数名:  将数据块作主钱包调入
// 参  数: 
// 返回值: 
//       
// 功  能：
//********************************************************/
int ISO1443A_RESTORE(int iSerial, int iBlock)
{
    int st;
    char SendData[32];
    char RecvData[32];

    memset(SendData, 0, sizeof(SendData));
    memset(RecvData, 0, sizeof(RecvData));

    sprintf(SendData, "%c01370001%02X00%c", 0x09,iBlock,0x0D);
    BCC(SendData);

    Com_SendPack(iSerial, SendData);
      
    st = Com_RecvPack(iSerial, 0x0A, 0x0E, RecvData, 3);
    #ifdef DEBUG
      fprintf(stdout,"Send Final \n");
    #endif
    
    if (st == 0)
    {
        if (strstr(RecvData, "013700"))
        {
            #ifdef DEBUG
            fprintf(stdout, "SUCCEED\n");
            #endif
            return(0);
        }
        #ifdef DEBUG
        fprintf(stdout, "FAIL, return: %s\n", RecvData);
        #endif
        return(-2);
    }

    return(-1);
}

//********************************************************/
// 函数名:  将钱包格式的数据传送到数据块内
// 参  数: 
// 返回值: 
//       
// 功  能：
//********************************************************/
int ISO1443A_TRANSFER(int iSerial, int iBlock)
{
    int st;
    char SendData[32];
    char RecvData[32];

    memset(SendData, 0, sizeof(SendData));
    memset(RecvData, 0, sizeof(RecvData));

    sprintf(SendData, "%c01380001%02X00%c", 0x09,iBlock,0x0D);
    BCC(SendData);

    Com_SendPack(iSerial, SendData);
      
    st = Com_RecvPack(iSerial, 0x0A, 0x0E, RecvData, 3);
    #ifdef DEBUG
      fprintf(stdout,"Send Final \n");
    #endif
    
    if (st == 0)
    {
        if (strstr(RecvData, "013800"))
        {
            #ifdef DEBUG
            fprintf(stdout, "SUCCEED\n");
            #endif
            return(0);
        }
        #ifdef DEBUG
        fprintf(stdout, "FAIL, return: %s\n", RecvData);
        #endif
        return(-2);
    }

    return(-1);
}

//********************************************************/
// 函数名:  增加调入的钱包
// 参  数: 
// 返回值: 
//       
// 功  能：
//********************************************************/
//int ISO1443A_INCREMENT(int iSerial, unsigned long iValue)
int ISO1443A_INCREMENT(int iSerial, unsigned int iValue)
{
    int st;
    char SendData[32];
    char RecvData[32];

    memset(SendData, 0, sizeof(SendData));
    memset(RecvData, 0, sizeof(RecvData));

    sprintf(SendData, "%c01390004%04X00%c", 0x09, iValue, 0x0D);
    BCC(SendData);

    Com_SendPack(iSerial, SendData);
      
    st = Com_RecvPack(iSerial, 0x0A, 0x0E, RecvData, 3);
    #ifdef DEBUG
      fprintf(stdout,"Send Final \n");
    #endif
    
    if (st == 0)
    {
        if (strstr(RecvData, "013900"))
        {
            #ifdef DEBUG
            fprintf(stdout, "SUCCEED\n");
            #endif
            return(0);
        }
        #ifdef DEBUG
        fprintf(stdout, "FAIL, return: %s\n", RecvData);
        #endif
        return(-2);
    }

    return(-1);
}

//********************************************************/
// 函数名:  减少调入的钱包
// 参  数: 
// 返回值: 
//       
// 功  能：
//********************************************************/
//int ISO1443A_DECREMENT(int iSerial, unsigned long iValue)
int ISO1443A_DECREMENT(int iSerial, unsigned int iValue)
{
    int st;
    char SendData[32];
    char RecvData[32];

    memset(SendData, 0, sizeof(SendData));
    memset(RecvData, 0, sizeof(RecvData));

    sprintf(SendData, "%c013A0004%04X00%c", 0x09, iValue, 0x0D);
    BCC(SendData);

    Com_SendPack(iSerial, SendData);
      
    st = Com_RecvPack(iSerial, 0x0A, 0x0E, RecvData, 3);
    #ifdef DEBUG
      fprintf(stdout,"Send Final \n");
    #endif
    
    if (st == 0)
    {
        if (strstr(RecvData, "013A00"))
        {
            #ifdef DEBUG
            fprintf(stdout, "SUCCEED\n");
            #endif
            return(0);
        }
        #ifdef DEBUG
        fprintf(stdout, "FAIL, return: %s\n", RecvData);
        #endif
        return(-2);
    }

    return(-1);
}

//********************************************************/
// 函数名:  停止卡操作
// 参  数: 
// 返回值: 
//       
// 功  能：
//********************************************************/
//int ISO1443A_HALT(int iSerial, char *szCardID)
int ISO1443A_HALT(int iSerial)
{
    int st;
    char SendData[32];
    char RecvData[32];

    memset(SendData, 0, sizeof(SendData));
    memset(RecvData, 0, sizeof(RecvData));

    sprintf(SendData, "%c013B000000%c", 0x09,0x0D);
    BCC(SendData);

    Com_SendPack(iSerial, SendData);
      
    st = Com_RecvPack(iSerial, 0x0A, 0x0E, RecvData, 3);
    #ifdef DEBUG
      fprintf(stdout,"Send Final \n");
    #endif
    
    if (st == 0)
    {
        if (strstr(RecvData, "013B00"))
        {
            if (debug) printf("ISO1443_HALT SUCCEED\n");
            return(0);
        }
        #ifdef DEBUG
        fprintf(stdout, "FAIL, return: %s\n", RecvData);
        #endif
        return(-2);
    }

    return(-1);
}

//********************************************************/
// 函数名:  装入密钥，与auth配合使用
// 参  数: 
// 返回值: 
//       
// 功  能：
//********************************************************/
int ISO1443A_LOADKEY(int iSerial, int iKeyType, int iBlock, char *szKey)
{
    int st;
    char SendData[32];
    char RecvData[32];

    memset(SendData, 0, sizeof(SendData));
    memset(RecvData, 0, sizeof(RecvData));

    sprintf(SendData, "%c013C0008%02X%02X%s00%c", 0x09, iKeyType, iBlock, szKey, 0x0D);
    BCC(SendData);

    Com_SendPack(iSerial, SendData);
      
    st = Com_RecvPack(iSerial, 0x0A, 0x0E, RecvData, 3);
    #ifdef DEBUG
      fprintf(stdout,"Send Final \n");
    #endif
    
    if (st == 0)
    {
        if (strstr(RecvData, "013C00"))
        {
            #ifdef DEBUG
            fprintf(stdout, "SUCCEED\n");
            #endif
            return(0);
        }
        #ifdef DEBUG
        fprintf(stdout, "FAIL, return: %s\n", RecvData);
        #endif
        return(-2);
    }

    return(-1);
}

#endif
