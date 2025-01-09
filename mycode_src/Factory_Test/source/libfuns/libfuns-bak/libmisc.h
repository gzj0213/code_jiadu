/*
 * =====================================================================================
 *
 *       Filename:  libmisc.h
 *
 *    Description:  杂项函数库集合
 *
 *        Version:  1.0
 *        Created:  2011年07月29日 10时34分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian0517@gmail.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#ifndef __LIBMISC_H__
#define __LIBMISC_H__

#include <stdint.h>

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_netlink_status
 *  Description:  检测函数连接
 *        Calls:  // 被本函数调用的函数清单
 *    Called By:  // 调用本函数的函数清单
 *        Input: if_name like "ath0", "eth0". Notice: call this function
 *    In-Output:
 *       Return: -1 -- no root
 *               -2 -- error , details can check errno
 *               1 -- interface link up
 *		 0 -- interface link down.
 *       Others: need root privilege.
 *       Author:  ZhouJian
 *     Revision:  none
 * =====================================================================================
 */
extern int libmisc_get_netlink_status(int chkroot, const char *if_name);


extern uint8_t libmisc_calc_addval(const uint8_t *src, const int len);

extern uint8_t libmisc_calc_xorval(const uint8_t *src, const int len);

#endif
