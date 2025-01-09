/*
 * =====================================================================================
 *
 *       Filename:  libtypes.h
 *
 *    Description:  自定义数据类型
 *
 *        Version:  1.0
 *        Created:  2011年08月26日 15时10分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian0517@gmail.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#ifndef __LIBTYPES_H__
#define __LIBTYPES_H__

#include <linux/types.h>
#include <sys/types.h>
#include <stdint.h>
#include <inttypes.h>

typedef int	bool;

typedef union u8_to_u32{
	uint8_t u8[4];
	uint32_t u32;
}u8_to_u32_t;

typedef union u8_to_u16{
	uint8_t u8[2];
	uint16_t u16;
}u8_to_u16_t;

#endif
