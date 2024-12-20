/*
 * =====================================================================================
 *
 *       Filename:  lib3des.h
 *
 *    Description:  GuangZhou Metro AFC
 *
 *        Version:  1.0
 *        Created:  2007年02月05日 09时23分25秒 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian@revenco.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#ifndef _LIB3DES_H_
#define _LIB3DES_H_

#define ENCRYPT   0      /* MODE == encrypt */
#define DECRYPT   1      /* MODE == decrypt */

extern void single_des(unsigned char *dest, unsigned char *src, char *inkey, int flg);

#endif
