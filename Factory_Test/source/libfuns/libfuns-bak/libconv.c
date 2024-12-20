/*
 * =====================================================================================
 *
 *       Filename:  libchar_tranfer.c
 *
 *    Description:  字符转换函数
 *
 *        Version:  1.0
 *        Created:  2007年04月18日 23时35分41秒 UTC
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
#include <stdlib.h>
#include <unistd.h>
#include <iconv.h>
#include "libconv.h"

/* 添加BCC校验 */
void libconv_add_bcc(char *buf, const unsigned int buflen)
{
	int i;
	int loop = buflen-4;
	unsigned char bcc = 0;
	char tmp[3];

	bzero(tmp, sizeof(tmp));

	for (i=0; i<loop; i++)
		bcc += buf[i+1];
	sprintf(tmp, "%02X", bcc);

	buf[buflen-3] = tmp[0];
	buf[buflen-2] = tmp[1];
}



void libconv_hex2str(char *str, const char *hex, const unsigned int hexlen)
{
	int i;
	unsigned char l4,h4;

	for(i=0; i<hexlen; i++) {
		h4=(hex[i] & 0xf0)>>4;
		l4=hex[i] & 0x0f;
		if (h4<=9) 
			str[2*i] = h4 + ('0' -0);
		else
			str[2*i] = h4 + ('A'-10);
		
		if (l4<=9) 
			str[2*i+1] = l4 + ('0' -0);
		else
			str[2*i+1] = l4 + ('A'-10);
	}
	str[2*i]=0;
}

unsigned char libconv_asc2hex(const char ascode)
{
	unsigned char ret;

	if('0'<=ascode && ascode<='9')
		ret=ascode-'0';
	else if('a'<=ascode && ascode<='f')
		ret=ascode-'a'+10;
	else if('A'<=ascode && ascode<='F')
		ret=ascode-'A'+10;
	else
		ret=0;

	return ret;
}

int libconv_chk_asc(const char ascode)
{
	if (('0'<=ascode) && (ascode<='9'))
		return 0;
	else if (('a'<=ascode) && (ascode<='f'))
		return 0;
	else if (('A'<=ascode) && (ascode<='F'))
		return 0;

	return -1;
}

int libconv_ascs2hex(char *hex, const char *ascs, const unsigned int asclen)
{
	unsigned char l4,h4;
	unsigned int i,lenstr;

	lenstr = asclen;
	if(lenstr==0){
		return -1;
	}

	if(lenstr%2)
		return -1;

	for(i=0; i<lenstr; i+=2){
		h4 = libconv_asc2hex(ascs[i]);
		l4 = libconv_asc2hex(ascs[i+1]);
		hex[i/2]=(h4<<4)+l4;
	}

	return 0;
}

void libconv_str2hex(char *hex, unsigned int *hex_len, const char *str)
{
	unsigned char l4,h4;
	int i;
	int lenstr = strlen(str);

	if (lenstr==0){
		*hex_len = 0;
		return;
	}

	for (i=0; i<lenstr-(lenstr%2); i+=2){
		h4 = libconv_asc2hex(str[i]);
		l4 = libconv_asc2hex(str[i+1]);
		hex[i/2]=(h4<<4)+l4;
	}

	if (lenstr%2)
		hex[(lenstr+1)/2-1] = libconv_asc2hex(str[lenstr-1]) << 4;

	*hex_len=(lenstr+1)/2;
}

void libconv_uint2bcd(unsigned char *bcd, int len, unsigned int x)
{
	char szTemp[21] = {0};
	unsigned char bTemp;
	int nLength;
	int nMax;
	int n;
	
	sprintf(szTemp, "%d", x);
	nLength = (int)strlen(szTemp);
	nMax	= len-1;

	memset(bcd, 0, sizeof(unsigned char)*len);

	bTemp	= 0x00;
	for(n = nLength-1; n >= 0; n--){
		bTemp = szTemp[n] - 0x30;
		n--;
		if(n >= 0) bTemp |= ((szTemp[n] - 0x30) << 4);
		bcd[nMax--] = bTemp;
	}
}

// 将BCD码转成32位的数
// bcd	BCD码
// bcd_len	BCD码的长度(最长4位)
// hex	32 位的数据
void libconv_bcd2ulong(unsigned long *hex, unsigned char *bcd, int bcd_len)
{
	char		szTemp[5] = {0};
	unsigned char	bTemp	  = 0;
	int n;
	
	*hex	= 0;
	if(bcd_len > 4) return;

	for(n=0; n<bcd_len; n++){
		sprintf(szTemp, "%X", bcd[n]);
		bTemp = atoi(szTemp);
		//hex += (pow(100,(bcd_len-n-1)) *bTemp);
		*hex = *hex * 100 + bTemp;
	}
}

// 对BCD编码进行判断
int libconv_chk_bcdformat(unsigned char *bcd, int bcd_len)
{
	int i = 0;

	while(i<bcd_len){
		if (((( bcd[i] >> 4) & 0x0F) > 0x09) || (( bcd[i] & 0x0F ) > 0x09)){
			return -1;
		}

		i++;
	}

	return 0;
}


int libconv_chk_bcdtime_format(unsigned char *bcdtm, int bcd_len)
{
	int retval;

	retval = libconv_chk_bcdformat(bcdtm, bcd_len);
	if (retval)
		return retval;

	if (bcdtm[0] != 0x20) // year H
		return -2;

	if ((bcdtm[2]==0) || (bcdtm[2]>0x12)) // month 
		return -3;

	if ((bcdtm[3]==0) || (bcdtm[3]>0x31)) // day
		return -4;

	if (bcdtm[4] > 0x23) // hour
		return -5;

	if (bcdtm[5] > 0x59 || bcdtm[6] > 0x59)// min , sec 
		return -6;

	return 0;
}

//代码转换:从一种编码转为另一种编码
int  libconv_code_convert(char *from_charset, char *to_charset, 
			const char *inbuf, size_t inlen, 
			char *outbuf, size_t *outlen)
{
	iconv_t cd;
	char **pin = (char **)&inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);
	if (cd==0)
		return -1;

	memset(outbuf, 0, *outlen);

	if (iconv(cd, pin, &inlen, pout, outlen) == -1){
		iconv_close(cd);
		return -2;
	}

	iconv_close(cd);

	return 0;
}

//UNICODE码转为GB2312码
int libconv_unicode2gb2312(const char *inbuf, size_t inlen, char *outbuf, size_t *outlen)
{
	return libconv_code_convert("utf-8","gbk", inbuf, inlen, outbuf, outlen);
}

//void udp_printf(char *fmt, ...)
//{
//	va_list ap;
//	int   len1, len; 
//	unsigned char string[256],str[256];   
//	va_start(ap, fmt);
//	len=vsprintf(string, fmt, ap);
//	u2g(string,strlen(string),str,OUTLEN);//编码转换
//	client_sockaddr.sin_port = htons(port_debug);
//	sendto(sockfd, str, strlen(string), 0, (struct sockaddr *)&client_sockaddr, sizeof(client_sockaddr));//udp发送数据
//	va_end(ap);
//}
