/*
 * =====================================================================================
 *
 *       Filename:  libace_lcd.c
 *
 *    Description:  日电lcd屏通讯协议
 *
 *        Version:  1.0
 *        Created:  2011年08月26日 10时47分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian0517@gmail.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#include <string.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/types.h>

#include "libtypes.h"
#include "libserial.h"
#include "libconv.h"
#include "libmisc.h"
#include "libtm.h"
#include "libace_lcd.h"

/*
 * 控制液晶屏显示的每条命令，遵循如下格式：
 * CHR(2,1byte，固定) + 地址（“LD1“，3byte，固定）＋长度（length,2byte）＋內容＋ CHR(3,1byte) + checksum(1byte)
 * 长度（length，2 byte），是内容的byte数组长度，分高低位显示，低位在前，高位在后。
 * 低位= length除以256取余数，高位= length除以256取整。
 * Checksum，是从地址开始到CHR(3,1byte)按位求异或的值。
*/

static int glcd_fd = -1;			/* 串口描述符 */

static void	libace_lcd_chk_xy(uint16_t *X, uint16_t *Y)
{
	*X = (*X==0) ? 1 : *X;
	*Y = (*Y==0) ? 1 : *Y;

	*X = (*X>MAX_LCD_WIDTH) ? MAX_LCD_WIDTH : *X;
	*Y = (*Y>MAX_LCD_HEIGHT) ? MAX_LCD_HEIGHT : *Y;
}

static int	libace_lcd_serial_write(int fd, const uint8_t *buf, const uint16_t buflen)
{
	/*int		i;*/
	int		retval;
	uint8_t		*p = NULL;
	u8_to_u16_t	len;

	p = malloc(sizeof(uint8_t)*(buflen+10));
	if (p == NULL)
		return -1;

	memset(p, 0, buflen+10);

	len.u16 = buflen;

	memcpy(p, "\x02\x4C\x44\x31", 4);
	// 低位在前
	memcpy(p+4, len.u8, 2);
	memcpy(p+6, buf, buflen);
	*(p+6+buflen) = 0x03;
	// 校验
	*(p+6+buflen+1) = libmisc_calc_xorval(p+1, buflen+6);
	*(p+6+buflen+2) = 0x00;

//	printf("lcd_send:");
//	for (i=0; i<(buflen+9); i++)
//		printf("%02X,", p[i]);
//	printf("\n");

	libserial_null_read(fd);

	retval = libserial_send_package(fd, (char *)p, buflen+9);

	free(p);

	return retval;
}

/*
 * 打开lcd端口
 * dev 端口地址
 * speed 波特率
 */
int	libace_lcd_open(char *dev, int speed)
{
	if (glcd_fd < 0)
		glcd_fd = libserial_init(dev, speed, 'n', 8, 1, 0, 0);

	if (glcd_fd < 0)
		return -1;

	return 0;
}

/*
 * 关闭lcd端口
 */
int	libace_lcd_close(void)
{
	int retval = -1;

	if (glcd_fd != -1)
		retval = libserial_close(glcd_fd);

	if (retval < 0)
		return retval;

	glcd_fd = -1;

	return 0;
}

/*
 * 液晶屏的打开和关闭
 * 参数sCmd设置液晶屏的打开和关闭，
 * sCmd= LCDCMD_OPENPAN打开液晶屏显示，
 * sCmd= LCDCMD_CLOSEPAN关闭液晶屏显示
 */
int	libace_lcd_openclose_disp(uint8_t sCmd)
{
	uint8_t buf[2];

	buf[0] = sCmd;
	buf[1] = 0;

	return libace_lcd_serial_write(glcd_fd, buf, 2);
}

/*
 * 液晶屏清屏
 * 参数设置清屏的起始坐标（X,Y），
 * 清屏高度（nHeight）和宽度（nWidth），
 * 以及清屏颜色（nColor） 
 */
int	libace_lcd_clear(uint16_t X, uint16_t Y, uint16_t nWidth, uint16_t nHeight, uint16_t nColor)
{
	u8_to_u16_t	val;
	uint8_t		buf[12];

	memset(buf, 0, sizeof(buf));

	libace_lcd_chk_xy(&X, &Y);

	buf[0] = LCDCMD_CLEARPAN;

	val.u16 = X;
	buf[1] = val.u8[0];
	buf[2] = val.u8[1];

	val.u16 = Y;
	buf[3] = val.u8[0];
	buf[4] = val.u8[1];

	val.u16 = nWidth;
	buf[5] = val.u8[0];
	buf[6] = val.u8[1];

	val.u16 = nHeight;
	buf[7] = val.u8[0];
	buf[8] = val.u8[1];

	val.u16 = nColor;
	buf[9] = val.u8[0];
	buf[10] = val.u8[1];

	return libace_lcd_serial_write(glcd_fd, buf, 11);
}


/*
 * 显示字符
 * 参数设置显示字符起始坐标（X,Y），字符串内容（SendWord），
 * 横向显示（Mode=0），一行字符总宽度（RowWidth），
 * 行高（字体高度与行间距之和，RowHeight），字体颜色（nFontColor）,
 * 字体大小（nFontSize），字体背景色（nBackColor）
 */
int	libace_lcd_showords(const char *Word, uint16_t WordLen,
				uint8_t Mode, 
				uint8_t RowWidth, uint8_t RowHeight,
				uint16_t X, uint16_t Y, 
				uint16_t nFontColor, uint8_t nFontSize, 
				uint16_t nBackColor)
{
	u8_to_u16_t	val;
	uint8_t		*p = NULL;
	int		retval;
	int outlen;

	/*libace_lcd_chk_xy(&X, &Y);*/

	p = malloc(sizeof(uint8_t)*(WordLen+16));
	if (p == NULL)
		return -1;

	memset(p, 0, WordLen+16);

	p[0] = LCDCMD_VIEWWORD;
	p[1] = !!Mode;
	p[2] = RowWidth;
	p[3] = RowHeight;

	val.u16 = X;
	p[4] = val.u8[0];
	p[5] = val.u8[1];

	val.u16 = Y;
	p[6] = val.u8[0];
	p[7] = val.u8[1];

	val.u16 = nFontColor;
	p[8] = val.u8[0];
	p[9] = val.u8[1];

	val.u16 = nBackColor;
	p[10] = val.u8[0];
	p[11] = val.u8[1];

	p[12] = nFontSize;

	/*p[13] = 128; // ?*/
	p[13] = 0; // ?
	p[14] = 2; // ?

	outlen = WordLen;
	/*printf("in outlen = %d\n", outlen);*/
	retval = libconv_unicode2gb2312(Word, (size_t)WordLen, (char *)&p[15], (size_t *)&outlen);
	if (retval)
		return -2;
	/*printf("out outlen = %d\n", outlen);*/

	/*memcpy(&p[15], Word, WordLen);*/
	/*p[15+WordLen] = 0;*/
	p[15+outlen*2] = 0;

	retval = libace_lcd_serial_write(glcd_fd, p, 16+outlen*2);

	free(p);

	return retval;
}


/*
 * 设置显示亮度
 * 参数（nLight）设置亮度值，范围0～15，
 * nLight=0显示最暗，nLight=15显示最亮
 */
int	libace_lcd_setlight(uint8_t nLight)
{
	uint8_t buf[2];

	buf[0] = LCDCMD_SETPANLIGHT;
	buf[1] = nLight;

	return libace_lcd_serial_write(glcd_fd, buf, 2);
}


/*
 * 显示图片
 * 参数设置图片显示的起始坐标（PicX,PicY），
 * 图片的宽度（PicWidth），
 * 图片的高度（PicHeight）
 * PicAddr，图片在控制板flash中的存储地址，该地址由存储图片的操作员提供。
 * PicX，显示图片的横坐标，1～800
 * PicY，显示图片的纵坐标，1～480
 * PicWidth，图片宽度
 * PicHeight，图片高度
 * PicX+ PicWidth不能大于显示屏宽度800
 * PicY+ PicHeight不能大于显示屏高度480
 * 在显示满屏规格800×480的图片时，应该设置坐标（PicX，PicY）为（1，1），
 * PicWidth=800，PicHeight=480
 */
int	libace_lcd_showpic(uint32_t PicAddr, uint16_t PicX, uint16_t PicY,
			uint16_t PicWidth, uint16_t PicHeight)
{
	u8_to_u16_t	val;
	u8_to_u32_t	ulval;
	uint8_t		buf[15];

	libace_lcd_chk_xy(&PicX, &PicY);
	libace_lcd_chk_xy(&PicWidth, &PicHeight);

	if ((PicX+PicWidth) > (MAX_LCD_WIDTH+1))
		return -1;

	if ((PicY+PicHeight) > (MAX_LCD_HEIGHT+1))
		return -2;

	memset(buf, 0, sizeof(buf));

	buf[0] = LCDCMD_DISPICON;

	val.u16 = PicX;
	buf[1] = val.u8[0];
	buf[2] = val.u8[1];

	val.u16 = PicY;
	buf[3] = val.u8[0];
	buf[4] = val.u8[1];

	val.u16 = PicWidth;
	buf[5] = val.u8[0];
	buf[6] = val.u8[1];

	val.u16 = PicHeight;
	buf[7] = val.u8[0];
	buf[8] = val.u8[1];

	ulval.u32 = PicAddr;
	buf[9] = ulval.u8[0];
	buf[10] = ulval.u8[1];
	buf[11] = ulval.u8[2];
	buf[12] = ulval.u8[3];

	buf[13] = 137; //?

	return libace_lcd_serial_write(glcd_fd, buf, 14);
}


/*
 * 全屏显示编号的图片
 * PicNum 1-25
 */
int	libace_lcd_fullshowpic_num(uint8_t PicNum)
{
	uint32_t PicAddr;

	PicNum = (PicNum==0) ? 1 : PicNum;
	PicNum = (PicNum>MAX_LCD_PIC_NUM) ? MAX_LCD_PIC_NUM : PicNum;

	PicAddr = LCD_PIC_DEFADDR+(LCD_PIC_SIZE*(PicNum-1));

	return libace_lcd_showpic(PicAddr, 1, 1, 800, 480);
}

/*
 * 指定尺寸显示编号图片
 * PicNum 1-25
 */
int	libace_lcd_sizeshowpic_num(uint8_t PicNum, uint16_t PicWidth, uint16_t PicHeight)
{
	uint32_t PicAddr;

	PicNum = (PicNum==0) ? 1 : PicNum;
	PicNum = (PicNum>MAX_LCD_PIC_NUM) ? MAX_LCD_PIC_NUM : PicNum;

	PicAddr = LCD_PIC_DEFADDR+(LCD_PIC_SIZE*(PicNum-1));

	return libace_lcd_showpic(PicAddr, 1, 1, PicWidth, PicHeight);
}

/*
 * 显示时间
 * 参数设置显示时间的起始坐标（X,Y）,
 * 时间字体颜色（FontColor），
 * 显示时间的背景色（BackColor），
 * 显示时间的字体大小（FontSize），
 * 横向显示模式（mode=0）
 */
int	libace_lcd_viewtime(uint16_t X, uint16_t Y, 
			uint16_t FontColor, uint16_t BackColor, 
			uint8_t FontSize, bool nMode)
{
	u8_to_u16_t	val;
	uint8_t		buf[12];

	/*libace_lcd_chk_xy(&X, &Y);*/

	memset(buf, 0, sizeof(buf));

	buf[0] = LCDCMD_STARTTIMEWINDOW;

	val.u16 = X;
	buf[1] = val.u8[0];
	buf[2] = val.u8[1];

	val.u16 = Y;
	buf[3] = val.u8[0];
	buf[4] = val.u8[1];

	val.u16 = FontColor;
	buf[5] = val.u8[0];
	buf[6] = val.u8[1];

	val.u16 = BackColor;
	buf[7] = val.u8[0];
	buf[8] = val.u8[1];

	buf[9] = FontSize;

	buf[10] = !!nMode;

	return libace_lcd_serial_write(glcd_fd, buf, 11);
}

/*
 * 设置时间
 * 无参数。将液晶屏显示时间修改为电脑时间
 */
int	libace_lcd_settime(void)
{
	time_type_t	t;
	uint8_t		tms[8];

	memset(tms, 0, sizeof(tms));

	libtm_gettime(&t);

	tms[0] = LCDCMD_SETPANTIME;
	tms[1] = (uint8_t)(t.year-2000);
	tms[2] = t.mon;
	tms[3] = t.day;
	tms[4] = t.hour;
	tms[5] = t.min;
	tms[6] = t.sec;

	return libace_lcd_serial_write(glcd_fd, tms, 7);
}


/*
 * 停止时间显示
 */
int	libace_lcd_stoptime(void)
{
	uint8_t buf[2];

	buf[0] = LCDCMD_STOPTIMEWINDOW;
	buf[1] = 0;

	return libace_lcd_serial_write(glcd_fd, buf, 2);
}
