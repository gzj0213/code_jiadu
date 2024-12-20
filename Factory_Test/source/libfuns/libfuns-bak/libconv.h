/*
 * =====================================================================================
 * 
 *       Filename:  libchar_transfer.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年04月18日 23时38分36秒 UTC
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  ZhouJian (), zhoujian@revenco.com
 *        Company:  Goldsign
 * 
 * =====================================================================================
 */
#ifndef __LIBCONV_H__
#define __LIBCONV_H__


#ifndef LIBCONV_BCD2HEX
#define LIBCONV_BCD2HEX(n) ((n>>4)*10+(n&0x0f))   //转化公式
#endif

#ifndef LIBCONV_HEX2BCD
#define LIBCONV_HEX2BCD(x) ((((x) / 10) << 4) + (x) % 10)
#endif

extern void libconv_add_bcc(char *buf, const unsigned int buflen);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hex2str
 *  Description:  16进制转换为字符串
 *        Calls:  // 被本函数调用的函数清单
 *    Called By:  // 调用本函数的函数清单
 *        Input:
 *    In-Output:
 *       Return:
 *       Others:
 *       Author:  ZhouJian
 *     Revision:  none
 * =====================================================================================
 */
extern void libconv_hex2str(char *str, const char *hex, const unsigned int hexlen);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  asc2hex
 *  Description:  ASCII码转换为16进制
 *        Calls:  // 被本函数调用的函数清单
 *    Called By:  // 调用本函数的函数清单
 *        Input:
 *    In-Output:
 *       Return:
 *       Others:
 *       Author:  ZhouJian
 *     Revision:  none
 * =====================================================================================
 */
extern unsigned char libconv_asc2hex(const char asccode);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ascs2hex
 *  Description:  ASCII码串转换为16进制串
 *        Calls:  // 被本函数调用的函数清单
 *    Called By:  // 调用本函数的函数清单
 *        Input:
 *    In-Output:
 *       Return:
 *       Others:
 *       Author:  ZhouJian
 *     Revision:  none
 * =====================================================================================
 */
extern int libconv_ascs2hex(char *hex, const char *ascs, const unsigned int asclen);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  str2hex
 *  Description:  字符串转换为16进制
 *        Calls:  // 被本函数调用的函数清单
 *    Called By:  // 调用本函数的函数清单
 *        Input:
 *    In-Output:
 *       Return:
 *       Others:
 *       Author:  ZhouJian
 *     Revision:  none
 * =====================================================================================
 */
extern void libconv_str2hex(char *hex, unsigned int *hex_len, const char *str);


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  
 *  Description:  
 *        Calls:  // 被本函数调用的函数清单
 *    Called By:  // 调用本函数的函数清单
 *        Input:
 *    In-Output:
 *       Return:
 *       Others:
 *       Author:  ZhouJian
 *     Revision:  none
 * =====================================================================================
 */
extern void libconv_uint2bcd(unsigned char *bcd, int len, unsigned int x);


// 将BCD码转成32位的数
// bcd	BCD码
// bcd_len	BCD码的长度(最长4位)
// hex	32 位的数据
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  
 *  Description:  
 *        Calls:  // 被本函数调用的函数清单
 *    Called By:  // 调用本函数的函数清单
 *        Input:
 *    In-Output:
 *       Return:
 *       Others:
 *       Author:  ZhouJian
 *     Revision:  none
 * =====================================================================================
 */
extern void libconv_bcd2ulong(unsigned long *hex, unsigned char *bcd, int bcd_len);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  
 *  Description:  对BCD编码进行判断
 *        Calls:  // 被本函数调用的函数清单
 *    Called By:  // 调用本函数的函数清单
 *        Input:
 *    In-Output:
 *       Return:
 *       Others:
 *       Author:  ZhouJian
 *     Revision:  none
 * =====================================================================================
 */
extern int libconv_chk_bcdformat(unsigned char *bcd, int bcd_len);

// 
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  
 *  Description: 对bcd编码时间格式进行判断  
 *        Calls:  // 被本函数调用的函数清单
 *    Called By:  // 调用本函数的函数清单
 *        Input:
 *    In-Output:
 *       Return:
 *       Others:
 *       Author:  ZhouJian
 *     Revision:  none
 * =====================================================================================
 */
extern int libconv_chk_bcdtime_format(unsigned char *bcdtm, int bcd_len);


extern int libconv_chk_asc(const char ascode);


//代码转换:从一种编码转为另一种编码
extern int  libconv_code_convert(char *from_charset, char *to_charset, 
			const char *inbuf, size_t inlen, 
			char *outbuf, size_t *outlen);

//UNICODE码转为GB2312码
extern int libconv_unicode2gb2312(const char *inbuf, size_t inlen, char *outbuf, size_t *outlen);

#endif
