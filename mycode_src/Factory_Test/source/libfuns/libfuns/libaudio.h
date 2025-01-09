/*
 * =====================================================================================
 *
 *       Filename:  libsaudio.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年03月01日 14时03分10秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __LIBAUDIO_H__
#define __LIBAUDIO_H__

#include <libtypes.h>

// 是否阻塞
#define AUDIO_NO_WAIT_STOP			0
#define AUDIO_WAIT_STOP				1	// 注意,调用阻塞方式时,需要关闭看门狗,或调整看门狗时长到mp3预定长度 

// Error return values

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  audio_play_file
 *  Description:  播放指定的音频文件 (文件为mp3编码,或wav文件),最好使用绝对路径指定文件 如: "/usr/share/test1.mp3" 或 "/usr/share/test1.wav"
 *  Param:			pfile - 播放文件路径,文件名  注: 此函数靠文件扩展名识别是mp3文件还是wav文件,小写.wav 或 .mp3
 *  				param - 播放参数,mp3文件详细参数见madplay帮助文件 #madplay --help
 *  				举例:常用参数: "-t 0:0:10" - 指定播放时长为10秒
 *  				     如果不使用任何参数 设置为NULL
 *  				         如果是播放文件为wav文件, 则此参数固定为采样率 如 "12000"
 *  				wait_stop - 是否阻塞等待播放完毕,即是否等待播放完毕,才返回, AUDIO_NO_WAIT_STOP - 不阻塞
 *  				alsa - 使用的音频API接口，true
 *  																			AUDIO_WAIT_STOP    - 阻塞
 * =====================================================================================
 */
extern int libaudio_play_file(char *pfile, char *param, int wait_stop, bool alsa);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  audio_kill_play
 *  Description:  停止当前播放的文件
 * =====================================================================================
 */
extern int libaudio_kill_play(void);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: audio_pause_play 
 *  Description:  暂停当前播放的文件
 * =====================================================================================
 */
extern int libaudio_pause_play(void);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  audio_continue_play
 *  Description:  继续播放暂停的文件
 * =====================================================================================
 */
extern int libaudio_continue_play(void);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  audio_adjust_the_volume
 *  Description:  调整音频音量
 *  Param:			dev: 当前调整混音设备名 一般设置为"vol"
 *  				支持的设备列表:"vol" "bass" "treble" "mic" "igain"
 *  				left_vol_per:	左声道百分比 10-100
 *  				right_vol_per:	右声道百分比 10-100
 *  				一般设置左右声道值一样
 *
 *  				音量值在重启后恢复为100%
 * =====================================================================================
 */
extern int libaudio_oss_adjust_the_volume(char *dev, int left_vol_per, int right_vol_per);


extern int libaudio_alsa_adjuest_the_volume(int leftright_vol);

#endif
