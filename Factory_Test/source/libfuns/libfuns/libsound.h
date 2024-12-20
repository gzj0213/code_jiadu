/*
 * =====================================================================================
 * 
 *       Filename:  libsound.h
 * 
 *    Description:  音频控制库
 * 
 *        Version:  1.0
 *        Created:  2007年08月03日 16时16分32秒 UTC
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  ZhouJian (), zhoujian@revenco.com
 *        Company:  Goldsign
 * 
 * =====================================================================================
 */

/*说明:编译时要加库-lasound*/

#ifndef  __LIBSOUND_H__
#define  __LIBSOUND_H__

#include <sys/soundcard.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>

#define DEV_SOUND "/dev/mixer"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  alsa_snd_init
 *  Description:  初始化
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
void alsa_snd_init(void);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  alsa_snd_read
 *  Description:  读音量值
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
int alsa_snd_read(void);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  alsa_snd_write
 *  Description:  写入音量
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
void alsa_snd_write(int leftright);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  alsa_snd_close
 *  Description:  退出
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
void alsa_snd_close(void);

#endif   /*#ifndef  __LIBSOUND_H__*/
