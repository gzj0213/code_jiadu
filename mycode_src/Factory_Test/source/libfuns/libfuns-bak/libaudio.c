/*
 * =====================================================================================
 *
 *       Filename:  libaudio.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年03月01日 11时02分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/soundcard.h>
#include <alsa/asoundlib.h>

#include "libaudio.h"

#define MP3PLAYER "madplay"
#define WAVPLAYER "wavplay"

#define MP3PLAY_ALSA	"mpg123"


static pid_t curply_pid = -1;

/* 用来存储所有可用混音设备的名称 */
const char *sound_device_names[] = SOUND_DEVICE_NAMES;

int libaudio_play_file(char *pfile, char *param, int wait_stop, bool alsa)
{
	int statloc;
	const char *player = MP3PLAYER;
	char *p1 = "--no-tty-control";
	char *p2 = "-Q";

	// 使用alsa接口
	if (alsa){
		player = MP3PLAY_ALSA;
		p1 = "-q";
		p2 = "-m";
	}

	if (strstr(pfile, ".wav") != NULL){
		player = WAVPLAYER;
		if (param == NULL)
			p1 = "12000";
		else
			p1 = param;
		p2 = NULL;
	}

	if (curply_pid != -1)
		kill(curply_pid, SIGKILL);

	curply_pid = fork();
	
	if (curply_pid < 0){
		printf("%s fork err!\n", __FILE__);
		return -1;
	}
	else if (curply_pid == 0) {
		if (execlp(player, player, p1, p2, pfile, param, NULL) == -1)
			printf("execlp failed\n");
	} else {
//		fprintf(stderr, "%s curply_pid %d write ok\n", __FILE__, curply_pid);
		if (wait_stop)
			wait(&statloc);
		usleep(100);
	}

	/*system("madplay wmsj.mp3 &");//利用system函数调用madplay播放器播放*.mp3音乐 */

	return 0;
}


int libaudio_kill_play(void)
{
	int retval = kill(curply_pid, SIGKILL);

	if (retval == 0)
		curply_pid = -1;

	/*system("killall -9 madplay");*/

	return retval;
}

int libaudio_pause_play(void)
{
	/*system("killall -STOP madplay &");*/

	return kill(curply_pid, SIGSTOP);
}

int libaudio_continue_play(void)
{
	/*system("killall -CONT madplay &");*/

	return kill(curply_pid, SIGCONT);
}

int libaudio_alsa_adjuest_the_volume(int leftright_vol)
{
	int retval;
	snd_mixer_t		*mixer;
	snd_mixer_elem_t	*elem;
	long alsa_min_vol, alsa_max_vol;
	long volume;

	if (leftright_vol>100)
		leftright_vol = 100;

	volume = leftright_vol*(65536/100);

	// 打开混音器
	retval = snd_mixer_open(&mixer, 0);
	if (retval<0){
		printf("snd_mixer_open error!\n");
		mixer = NULL;
		return -1;
	}

	// 
	retval = snd_mixer_attach(mixer, "default");
	if (retval<0){
		printf("snd_mixer_attach error!\n");
		snd_mixer_close(mixer);
		mixer = NULL;
		return -2;
	}

	// 注册混音器
	retval = snd_mixer_selem_register(mixer, NULL, NULL);
	if (retval<0){
		printf("snd_mixer_selem_register error!\n");
		snd_mixer_close(mixer);
		mixer = NULL;
		return -3;
	}

	// 加载混音器
	retval = snd_mixer_load(mixer);
	if (retval<0){
		printf("snd_mixer_load error!\n");
		snd_mixer_close(mixer);
		mixer = NULL;
		return -4;
	}

	for (elem=snd_mixer_first_elem(mixer); elem; elem=snd_mixer_elem_next(elem)){
		if (snd_mixer_elem_get_type(elem) == SND_MIXER_ELEM_SIMPLE &&
				snd_mixer_selem_is_active(elem)){ // 找到可以用的, 激活的elem
			if (strcmp(snd_mixer_selem_get_name(elem), "Master") == 0){
				snd_mixer_selem_get_playback_volume_range(elem, &alsa_min_vol, &alsa_max_vol);
				//printf("min = %ld, max = %ld\r\n", alsa_min_vol, alsa_max_vol);
				// 设置音量
				snd_mixer_selem_set_playback_volume_all(elem, volume);
			}
		}
	}



//	//找到Pcm对应的element,方法比较笨拙
//	pcm_element = snd_mixer_first_elem(mixer);
//	pcm_element = snd_mixer_elem_next(pcm_element);
//	pcm_element = snd_mixer_elem_next(pcm_element);
//	//
//	///处理alsa1.0之前的bug，之后的可略去该部分代码
////	snd_mixer_selem_get_playback_volume(pcm_element,
////			SND_MIXER_SCHN_FRONT_LEFT, &a);
////	snd_mixer_selem_get_playback_volume(pcm_element,
////			SND_MIXER_SCHN_FRONT_RIGHT, &b);
//
//	snd_mixer_selem_get_playback_volume_range(pcm_element,
//			&alsa_min_vol,
//			&alsa_max_vol);
//
//	printf("min = %ld, max = %ld\r\n", alsa_min_vol, alsa_max_vol);
//
//	///设定音量范围
//	snd_mixer_selem_set_playback_volume_range(pcm_element, 0, 100);
//
//	//左音量
//	snd_mixer_selem_set_playback_volume(pcm_element,
//			SND_MIXER_SCHN_FRONT_LEFT,
//			leftright_vol);
//	//右音量
//	snd_mixer_selem_set_playback_volume(pcm_element,
//			SND_MIXER_SCHN_FRONT_RIGHT,
//			leftright_vol);

	snd_mixer_close(mixer);

	return 0;
}

int libaudio_oss_adjust_the_volume(char *dev, int left_vol_per, int right_vol_per)
{
	int level;			/* 增益设置 */
	int status;			/* 系统调用的返回值 */
	int device;			/* 选用的混音设备 */
	int fd;
	int i;
	int devmask, stereodevs;	/* 混音器信息对应的位图掩码 */

	left_vol_per = left_vol_per%100;
	right_vol_per = right_vol_per%100;

	if (left_vol_per<10)
		left_vol_per = 10;

	if (right_vol_per<10)
		right_vol_per = 10;

	fd = open("/dev/mixer", O_RDONLY);
	if (fd == -1) {
		perror("unable to open /dev/mixer");
		return -1;
	}

	/* 获得所需要的信息 */
	status = ioctl(fd, SOUND_MIXER_READ_DEVMASK, &devmask);
	if (status == -1){
		perror("SOUND_MIXER_READ_DEVMASK ioctl failed");
	}

	status = ioctl(fd, SOUND_MIXER_READ_STEREODEVS, &stereodevs);
	if (status == -1){
		perror("SOUND_MIXER_READ_STEREODEVS ioctl failed");
	}

	/* 保存用户输入的混音器名称 */
	/*dev = argv[1];*/
	/* 确定即将用到的混音设备 */
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++)
		if (((1 << i) & devmask) && !strcmp(dev, sound_device_names[i]))
			break;
	if (i == SOUND_MIXER_NRDEVICES) {	/* 没有找到匹配项 */
		fprintf(stderr, "%s is not a valid mixer device\n", dev);
		close(fd);
		return -2;
	}
	/* 查找到有效的混音设备 */
	device = i;

	/* 对非立体声设备给出警告信息 */
	if ((left_vol_per != right_vol_per) && !((1 << i) & stereodevs)) {
		fprintf(stderr, "warning: %s is not a stereo device\n", dev);
	}

	/* 将两个声道的值合到同一变量中 */
	level = (right_vol_per << 8) + left_vol_per;

	/* 设置增益 */
	status = ioctl(fd, MIXER_WRITE(device), &level);
	if (status == -1) {
		perror("MIXER_WRITE ioctl failed");
		close(fd);
		return -3;
	}

	close(fd);

	return 0;
}
