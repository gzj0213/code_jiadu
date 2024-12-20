/*
 * =====================================================================================
 *
 *       Filename:  libmisc.c
 *
 *    Description:  杂项函数库
 *
 *        Version:  1.0
 *        Created:  2011年07月29日 10时23分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian0517@gmail.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>

#include "libmisc.h"

//struct ethtool_value {
//	__uint32_t      cmd;
//	__uint32_t      data;
//};


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  libmisc_get_netlink_status
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
int libmisc_get_netlink_status(int chkroot, const char *if_name)
{
	int skfd;
	struct ifreq ifr;
	struct ethtool_value edata;

	if (chkroot && (getuid() != 0)){
		fprintf(stderr, "Netlink Status Check Need Root Power.\n");
		return -1;
	}

	edata.cmd = ETHTOOL_GLINK;
	edata.data = 0;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name) - 1);
	ifr.ifr_data = (char *) &edata;

	if (( skfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0)
		return -2;

	if(ioctl( skfd, SIOCETHTOOL, &ifr ) == -1){
		close(skfd);
		return -2;
	}

	close(skfd);

	return edata.data;
}


// 计算累加和值
uint8_t libmisc_calc_addval(const uint8_t *src, const int len)
{
	int		i = 0;
	uint8_t		j = 0;

	for (i=0, j=0; i<len; i++){
		j += *(src+i);
	}

	return j;
}


uint8_t libmisc_calc_xorval(const uint8_t *src, const int len)
{
	int	i;
	uint8_t	xor = *src;
	
	for(i=1; i<len; i++)
	{
		xor ^= *(src+i);
	}

	return xor;
}

