/*
 * =====================================================================================
 *
 *       Filename:  libace_lcd.h
 *
 *    Description:  日电lcd屏通讯协议
 *
 *        Version:  1.0
 *        Created:  2011年08月26日 10时49分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian0517@gmail.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#ifndef __LIBACE_LCD_H__
#define __LIBACE_LCD_H__

#include <libtypes.h>


// 颜色定义
#define	LCD_CL_NOBACKCOLOR		128		//使用当前背景色
#define	LCD_CL_YELLOW			65504		//黄色
#define	LCD_CL_BLUE			32799		//蓝色
#define	LCD_CL_GREEN			33760		//绿色
#define	LCD_CL_RED			64512		//红色
#define	LCD_CL_BLACK			32768		//黑色
#define	LCD_CL_WHITE			65535		//白色


// 字体大小定义（十进制），用于显示文字，显示时间时对字体大小进行设置。
// 简体
#define	Gb2312CHS_1		4   			// （规格16X16）   //国标简体
#define	Gb2312CHS_2		5   			// （规格24X24）
#define	Gb2312CHS_3		6   			// （规格32X32）
#define	Gb2312CHS_4		7   			// （规格48X48）
// 繁体
#define	Gb2312CHT_1		8   			// （规格16X16）   //国标繁体
#define	Gb2312CHT_2		9   			// （规格24X24）
#define	Gb2312CHT_3		10  			// （规格32X32）
#define	Gb2312CHT_4		11  			// （规格48X48）


// 命令常数定义
#define	LCDCMD_CLEARPAN			240		//清屏命令
#define	LCDCMD_SETPANLIGHT		241		//调节亮度命令
#define	LCDCMD_OPENPAN			242		//开显示
#define	LCDCMD_CLOSEPAN			243		//关显示
#define	LCDCMD_DISPICON			244		//显示图标
#define	LCDCMD_VIEWWORD			245		//显示字符串，汉字与西文 
#define	LCDCMD_SETPANTIME		250		//设置时间
#define	LCDCMD_STARTTIMEWINDOW		251		//在指定位置显示时间
#define	LCDCMD_STOPTIMEWINDOW		252		//关掉时间显示窗口

// 亮度范围
#define	MAX_LCD_LIGHT			15		// 最大亮度
#define	MIN_LCD_LIGHT			0		// 最小亮度

// LCD尺寸定义
#define MIN_LCD_WIDTH			1		// 最小宽度
#define MAX_LCD_WIDTH			800		// 最大宽度

#define MIN_LCD_HEIGHT			1		// 最小高度
#define MAX_LCD_HEIGHT			480		// 最大高度

// picture定义
#define	MAX_LCD_PIC_NUM			25		// 最大图片数量
#define LCD_PIC_DEFADDR			0x2000002	// 图片位置
#define	LCD_PIC_SIZE			0xBB800		// 图片size


/*
 * 打开lcd端口
 * dev 端口地址
 * speed 波特率
 */
extern int	libace_lcd_open(char *dev, int speed);

/*
 * 关闭lcd端口
 */
extern int	libace_lcd_close(void);

/*
 * 液晶屏的打开和关闭
 * 参数sCmd设置液晶屏的打开和关闭，
 * sCmd= LCDCMD_OPENPAN打开液晶屏显示，
 * sCmd= LCDCMD_CLOSEPAN关闭液晶屏显示
 */
extern int	libace_lcd_openclose_disp(uint8_t sCmd);

/*
 * 液晶屏清屏
 * 参数设置清屏的起始坐标（X,Y），
 * 清屏高度（nHeight）和宽度（nWidth），
 * 以及清屏颜色（nColor） 
 */
extern int	libace_lcd_clear(uint16_t X, uint16_t Y, uint16_t nWidth, uint16_t nHeight, uint16_t nColor);


/*
 * 显示字符
 * 参数设置显示字符起始坐标（X,Y），字符串内容（SendWord），
 * 横向显示（Mode=0），一行字符总宽度（RowWidth），
 * 行高（字体高度与行间距之和，RowHeight），字体颜色（nFontColor）,
 * 字体大小（nFontSize），字体背景色（nBackColor）
 */
extern int	libace_lcd_showords(const char *SendWord, uint16_t WordLen, 
				uint8_t Mode, 
				uint8_t RowWidth, uint8_t RowHeight,
				uint16_t X, uint16_t Y, 
				uint16_t nFontColor, uint8_t nFontSize, 
				uint16_t nBackColor);


/*
 * 设置显示亮度
 * 参数（nLight）设置亮度值，范围0～15，
 * nLight=0显示最暗，nLight=15显示最亮
 */
extern int	libace_lcd_setlight(uint8_t nLight);

/*
 * 显示图片
 * 参数设置图片显示的起始坐标（PicX,PicY），
 * 图片的宽度（PicWidth），
 * 图片的高度（PicHeight）
 */
extern int	libace_lcd_showpic(uint32_t PicAddr, uint16_t PicX, uint16_t PicY, 
				uint16_t PicWidth, uint16_t PicHeight);

/*
 * 全屏显示编号的图片
 * PicNum 1-25
 */
extern int	libace_lcd_fullshowpic_num(uint8_t PicNum);

/*
 * 指定尺寸显示编号图片
 * PicNum 1-25
 */
extern int	libace_lcd_sizeshowpic_num(uint8_t PicNum, uint16_t PicWidth, uint16_t PicHeight);

/*
 * 显示时间
 * 参数设置显示时间的起始坐标（X,Y）,
 * 时间字体颜色（FontColor），
 * 显示时间的背景色（BackColor），
 * 显示时间的字体大小（FontSize），
 * 横向显示模式（mode=0）
 */
extern int	libace_lcd_viewtime(uint16_t X, uint16_t Y, 
		uint16_t FontColor, uint16_t BackColor, 
		uint8_t FontSize, bool nMode);

/*
 * 设置时间
 * 无参数。将液晶屏显示时间修改为电脑时间
 */
extern int	libace_lcd_settime(void);

/*
 * 停止时间显示
 */
extern int	libace_lcd_stoptime(void);

#endif
