/*
 * =====================================================================================
 *
 *       Filename:  wavplay.c
 *
 *    Description: 使用方法：wavplay [文件名] [采样率] 
 *    						./wavplay xingfu.wav 11025
 *
 *        Version:  1.0
 *        Created:  2011年03月10日 18时10分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *        Company:  
 *
 * =====================================================================================
 */
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#define BUF_SIZE 4096
#define DEVICE_NAME "/dev/dsp"

static int audio_fd; //声卡
static FILE *file_fd; //文件
static int file_len; //文件长度
/*static const char *file_name = "test.wav";*/
static unsigned char audio_buffer[BUF_SIZE];
static unsigned char *file_name_creat;
static unsigned int audio_rate;

//void delay(long x)
//{
//	unsigned long i;
//	for(i=0; i<x; i++);
//}

int main(int argc, char *argv[])
{
	int i=0;

	/*printf("This is a wav play program.\n");*/
	/*printf("Please add a file name and the audio rate! such as \"./oss test.wav 44100\" \n\n\n");*/
	/*delay(100000);*/

	file_name_creat = argv[1];
	sscanf(argv[2],"%d", &audio_rate);

	/*printf("the file name is %s audio rate is %d \n",file_name_creat,audio_rate);*/
	/*delay(100000);*/

	/*打开音频设备，准备play*/
	if ((audio_fd = open(DEVICE_NAME, O_WRONLY)) == -1){
		printf("open error: %s\n", DEVICE_NAME);
		return -1;
	}

	/*设置采样格式*/
	int format;
	format = AFMT_S16_LE;

	if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &format) == -1){
		/* fatal error */
		printf("SNDCTL_DSP_SETFMT error\n");
		close(audio_fd);
		return -1;  
	}

	if (format != AFMT_S16_LE){
		/* 本设备不支持选择的采样格式. */
		printf("sep4020 oss driver does not support AFMT_S16_LE");
	}

	/*设置通道数*/
	int channels = 2; /* 1=mono, 2=stereo */

	if (ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &channels) == -1){
		/* Fatal error */
		printf("SNDCTL_DSP_CHANNELS error");
		close(audio_fd);
		return -1;
	}

	if (channels != 2){
		/* 本设备不支持立体声模式 ... */
		printf("sep4020 oss driver does ");
	}

	/*设置采样速率*/
	int speed = audio_rate;

	if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &speed)==-1){
		/* Fatal error */
		printf("SNDCTL_DSP_SPEED error\n");
		close(audio_fd);
		return -1;
	}

	/*printf("the wav speed is %d\n",speed);*/


	/*打开并计算文件长度*/
	file_fd = fopen(file_name_creat, "r");
	fseek(file_fd, 0, SEEK_END);     //定位到文件末
	file_len = ftell(file_fd);     //文件长度

	int loops = file_len/BUF_SIZE;

	/*重新定位到文件头*/
	fclose(file_fd);
	file_fd = fopen(file_name_creat, "r");
	/*播放wav文件*/
	for(i=0;i<loops;i++){
		fread(audio_buffer, BUF_SIZE, 1, file_fd);
		write(audio_fd,audio_buffer, BUF_SIZE);
	}

	/*关闭设备和文件*/
	fclose(file_fd);
	close(audio_fd);

	return 0;
}

//AUTHOR("aokikyon@gmail.com");
//DESCRIPTION("sep4020 uda1341 sound card test program");
