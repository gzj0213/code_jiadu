/*
 * =====================================================================================
 *
 *       Filename:  libtime.c
 *
 *    Description:  时间操作相关函数
 *
 *        Version:  1.0
 *        Created:  2007年04月18日 22时55分01秒 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian@revenco.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#ifndef __LIBTM_H__
#define __LIBTM_H__

#include <time.h>

/* 日期结构 */
typedef struct date_type{
    int year;
    int mon;
    int day;
}date_type_t;

/* 时间结构 */
typedef struct time_type{
    int		year;
    char	mon;
    char	day;
    char	hour;
    char	min;
    char	sec;
}time_type_t;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gettime
 *  Description:  取当前时间
 *        Calls:  // 被本函数调用的函数清单
 *    Called By:  // 调用本函数的函数清单
 *        Input:
 *       Output:
 *       Return:
 *       Others:
 * =====================================================================================
 */
extern void libtm_getdate(date_type_t *d);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gettime
 *  Description:  取当前时间
 *        Calls:  // 被本函数调用的函数清单
 *     alled By:  // 调用本函数的函数清单
 *        Input:
 *       Output:
 *       Return:
 *       Others:
 * =====================================================================================
 */
extern void libtm_gettime(time_type_t *t);

// 将当前时间转换为7byte bcd编码时间 0x20, 0x11, 0x07, 0x24...
extern void libtm_get_bcdtime(unsigned char *bcdtm);


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gettime
 *  Description:  取当前时间
 *        Calls:  // 被本函数调用的函数清单
 *     alled By:  // 调用本函数的函数清单
 *        Input:
 *       Output:
 *       Return:
 *       Others:
 * =====================================================================================
 */
extern void libtm_bcdtm2hextm(unsigned char *hextm, unsigned char *bcdtm);

// BCD日期比较,函数没有做日期格式判断
// bcdtm1 = bcdtm2 return = 0
// bcdtm1 > bcdtm2 return >0
// bcdtm1 < bcdtm2 return <0
extern int libtm_bcdtime_cmp(const unsigned char *bcdtm1, const unsigned char *bcdtm2, const unsigned char len);

extern int libtm_set_sysbcdtime(char *bcddt);

#endif /*#ifndef __LIBDATE_TIME_H__*/
