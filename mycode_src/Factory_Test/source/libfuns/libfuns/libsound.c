/*
 * =====================================================================================
 *
 *       Filename:  libsound.c
 *
 *    Description:  音频控制库
 *
 *        Version:  1.0
 *        Created:  2007年08月03日 16时03分34秒 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian@revenco.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#include "libsound.h"

snd_mixer_t *mixer;
snd_mixer_elem_t *pcm_element;

/*
int main (void)
{
    int handle = open (DEV_SOUND, O_RDWR, 0); //O_RDWR：可读可写，O_RDONLY：只读，O_WRONLY：只写
    if (handle == -1){
        perror ("open snd fail\n");
        return -1;
    }

    int leftright = (30 << 8) | (60 & 0xFF); //左Vol=30,右Vol=60
    int result;
    int dev=0; //设备号为：Vol=0,Pcm=4,Spkr=5,Line=6,Mic=7,CD=8,IGam=12,Line1=14,Digital1=17,PhoneIn=20,PhoneOut=21,Video=22

    if (ioctl(handle, MIXER_WRITE(dev), &leftright) == -1){ //写操作
        perror("write_snd_error");
    }

    if (ioctl(handle, MIXER_READ (dev), &result) == -1){ //读操作
        perror("read_snd_error");
    }

    printf ("left Vol = %d\n", result >> 8);     //显示左Volum
    printf ("right Vol=%d\n", result & 0xFF);    //显示右Volum

    close(handle);
    return 0;
}
*/

/*
int main(void)
{
    int value;

    alsa_snd_init();

    value = alsa_snd_read();
    printf("value=%d\n", value);
    return 0;
}
*/

//初始化-----------------------------------------------------
void alsa_snd_init(void)
{
    snd_mixer_open(&mixer, 0);
    snd_mixer_attach(mixer, "default");
    snd_mixer_selem_register(mixer, NULL, NULL);
    snd_mixer_load(mixer);
    //找到Pcm对应的element,方法比较笨拙
    pcm_element = snd_mixer_first_elem(mixer);
    pcm_element = snd_mixer_elem_next(pcm_element);
    pcm_element = snd_mixer_elem_next(pcm_element);
    //
    long int a, b;
    long alsa_min_vol, alsa_max_vol;
    ///处理alsa1.0之前的bug，之后的可略去该部分代码
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_LEFT, &a);
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_RIGHT, &b);

    snd_mixer_selem_get_playback_volume_range(pcm_element,
                                              &alsa_min_vol,
                                              &alsa_max_vol);
    ///设定音量范围
    snd_mixer_selem_set_playback_volume_range(pcm_element, 0, 100);
    
}

//读音量值-----------------------------------------------------
int alsa_snd_read(void)
{
    long ll, lr;
    //处理事件
    snd_mixer_handle_events(mixer);
    //左声道
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_LEFT, &ll);
    //右声道
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_RIGHT, &lr);
    return (ll + lr) >> 1;
}

//写入音量-----------------------------------------------------
void alsa_snd_write(int leftright)
{
    //左音量
    snd_mixer_selem_set_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_LEFT,
                                        leftright);
    //右音量
    snd_mixer_selem_set_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_RIGHT,
                                        leftright);
}

//退出-----------------------------------------------------
void alsa_snd_close(void)
{
    snd_mixer_close(mixer);
}
