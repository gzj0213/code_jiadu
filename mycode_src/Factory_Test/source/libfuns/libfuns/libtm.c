/*
 * =====================================================================================
 *
 *       Filename:  libtime.c
 *
 *    Description:  time function
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "libtm.h"
#include "libconv.h"


void libtm_getdate(date_type_t *d)
{
	long ti;
	struct tm *_tm;

	time(&ti);
	_tm = localtime(&ti);

	d->year = _tm->tm_year+1900;
	d->mon  = _tm->tm_mon+1;
	d->day  = _tm->tm_mday;
}

void libtm_gettime(time_type_t *t)
{
	long ti;
	struct tm *_tm;

	time(&ti);
	_tm = localtime(&ti);

	t->year = _tm->tm_year+1900;
	t->mon  = _tm->tm_mon+1;
	t->day  = _tm->tm_mday;
	t->hour = _tm->tm_hour;
	t->min  = _tm->tm_min;
	t->sec  = _tm->tm_sec;
}

// 将当前时间转换为7byte bcd编码时间 0x20, 0x11, 0x07, 0x24...
void libtm_get_bcdtime(unsigned char *bcdtm)
{
	time_type_t t;

	if (bcdtm == NULL)
		return;

	libtm_gettime(&t);

	bcdtm[0] = 0x20;	// 固定为0x20
	bcdtm[1] = LIBCONV_HEX2BCD(t.year-2000);
	bcdtm[2] = LIBCONV_HEX2BCD(t.mon);
	bcdtm[3] = LIBCONV_HEX2BCD(t.day);
	bcdtm[4] = LIBCONV_HEX2BCD(t.hour);
	bcdtm[5] = LIBCONV_HEX2BCD(t.min);
	bcdtm[6] = LIBCONV_HEX2BCD(t.sec);
}


void libtm_bcdtm2hextm(unsigned char *hextm, unsigned char *bcdtm)
{
	hextm[0] = LIBCONV_BCD2HEX(bcdtm[0]);
	hextm[1] = LIBCONV_BCD2HEX(bcdtm[1]);
	hextm[2] = LIBCONV_BCD2HEX(bcdtm[2]);
	hextm[3] = LIBCONV_BCD2HEX(bcdtm[3]);
	hextm[4] = LIBCONV_BCD2HEX(bcdtm[4]);
	hextm[5] = LIBCONV_BCD2HEX(bcdtm[5]);
	hextm[6] = LIBCONV_BCD2HEX(bcdtm[6]);
}

// BCD日期比较,函数没有做日期格式判断
// bcdtm1 = bcdtm2 return = 0
// bcdtm1 > bcdtm2 return >0
// bcdtm1 < bcdtm2 return <0
int libtm_bcdtime_cmp(const unsigned char *bcdtm1, const unsigned char *bcdtm2, const unsigned char len)
{
	unsigned char i;

	for (i=0; i<len; i++){
		if (bcdtm1[i]!=bcdtm2[i])
			break;
	}

	if (i>=len)
		return 0;   // 日期相等
	else{
		if (bcdtm1[i]>bcdtm2[i])
			return i;
		else
			return -i;
	}
}

//int libtm_setsystime(char *dt)
//{
//	struct rtc_time tm;
//	struct tm _tm;
//	struct timeval tv;
//	time_t timep;
//
//
//	sscanf(dt, "%d-%d-%d %d:%d:%d", &tm.tm_year,
//			&tm.tm_mon, &tm.tm_mday,&tm.tm_hour,
//			&tm.tm_min, &tm.tm_sec);
//
//	_tm.tm_sec = tm.tm_sec;
//	_tm.tm_min = tm.tm_min;
//	_tm.tm_hour = tm.tm_hour;
//	_tm.tm_mday = tm.tm_mday;
//	_tm.tm_mon = tm.tm_mon - 1;
//	_tm.tm_year = tm.tm_year - 1900;
//
//	timep = mktime(&_tm);
//	tv.tv_sec = timep;
//	tv.tv_usec = 0;
//
//	if (settimeofday(&tv, (struct timezone *) 0) < 0)
//		return -1;
//		/*printf("Set system datatime error!\n");*/
//	else
//		/*printf("Set system datatime successfully!\n");*/
//		return 0;
//}


/*
 * 设置bcd时间
 * 参数 时间格式bcd字符串，"20110906112330"
 */
int libtm_set_sysbcdtime(char *bcddt)
{
	struct tm _tm;
	struct timeval tv;
	time_t timep;
	int	retval;

	if (libconv_chk_bcdtime_format((unsigned char *)bcddt, 7) != 0)
		return -1;

	_tm.tm_year = (LIBCONV_BCD2HEX(bcddt[0])*100+LIBCONV_BCD2HEX(bcddt[1]))-1900;
	_tm.tm_mon = LIBCONV_BCD2HEX(bcddt[2])-1;
	_tm.tm_mday = LIBCONV_BCD2HEX(bcddt[3]);
	_tm.tm_hour = LIBCONV_BCD2HEX(bcddt[4]);
	_tm.tm_min = LIBCONV_BCD2HEX(bcddt[5]);
	_tm.tm_sec = LIBCONV_BCD2HEX(bcddt[6]);

	timep = mktime(&_tm);
	tv.tv_sec = timep;
	tv.tv_usec = 0;

	if (settimeofday(&tv, (struct timezone *) 0) < 0)
		return -2;

	retval = system("hwclock -w");

	return 0;
}
