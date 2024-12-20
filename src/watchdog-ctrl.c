#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
//#include <linux/delay.h>

#define GPIO_IOC_MAGIC 'T'
#define READ_GPIO _IO(GPIO_IOC_MAGIC, 0)
#define WRITE_GPIO _IO(GPIO_IOC_MAGIC, 1)
#define WATCHDDOG_PIN 40
#define MCU_ADDR 0x5a
//#define MCU_REG 0xb4
#define WATCHDOG_OPEN 0x02
#define WATCHDOG_CLOSE 0x01

int i2c_write(unsigned char device_addr, unsigned char sub_addr, int ByteNo)
{
    int i2c_fd, ret;
    unsigned char buftmp[32];
    struct i2c_rdwr_ioctl_data i2c_data;
    const char *i2c_dev = "/dev/i2c-7";

    // init
    i2c_fd = open(i2c_dev, O_RDWR);
    if (i2c_fd < 0)
    {
        printf("could not open i2c device\n");
        return -1;
    }

    i2c_data.nmsgs = 1;
    i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
    if (i2c_data.msgs == NULL)
    {
        printf("malloc error");
        close(i2c_fd);
        return -1;
    }

    ioctl(i2c_fd, I2C_TIMEOUT, 1);
    ioctl(i2c_fd, I2C_RETRIES, 2);

    memset(buftmp, 0, 32);
    buftmp[0] = sub_addr;
    //memcpy(buftmp + 1, buff, ByteNo);
    i2c_data.msgs[0].len = ByteNo + 1;
    ;
    i2c_data.msgs[0].addr = MCU_ADDR;
    i2c_data.msgs[0].flags = 0; // 0: write 1:read
    i2c_data.msgs[0].buf = buftmp;
    ret = ioctl(i2c_fd, I2C_RDWR, (unsigned long)&i2c_data);
    if (ret < 0)
    {
        printf("write reg %x %x error\r\n", device_addr, sub_addr);
        close(i2c_fd);
        free(i2c_data.msgs);
        return 1;
    }
    free(i2c_data.msgs);
    close(i2c_fd);
}

int write_gpio(int gpio_num, int value, int fd)
{
    //unsigned char write_num;
    unsigned char str[3];

    if (gpio_num > 256)
    {
        str[0] = (unsigned char)(gpio_num >> 8);
        str[1] = (unsigned char)255;
        str[2] = (unsigned char)value;
        ioctl(fd, WRITE_GPIO, str);
    }
    else
    {
        str[0] = 0;
        str[1] = gpio_num;
        str[2] = (unsigned char)value;
        ioctl(fd, WRITE_GPIO, str);
    }
}

int gpio_flipping(int gpio)
{
    int FD;
    FD = open("/dev/rockchip-gpio", O_RDWR);

    if (FD < 0)
    {
        printf("open file failed\n");
        return -1;
    }
    while (1)
    {
        write_gpio(gpio, 1, FD);
        sleep(2);
        write_gpio(gpio, 0, FD);
        sleep(2);
    }
    return 0;
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("usage:please input right command\n");
        printf("exp:./test gpio_num \n");
        return -1;
    }
    else
    {
        if (strcmp(argv[1], "-d") == 0)
        {
            gpio_flipping(WATCHDDOG_PIN);
        }
        else if (strcmp(argv[1], "-o") == 0)
        {
            i2c_write(MCU_ADDR, WATCHDOG_OPEN, 2);
            printf("watchdog enabled\n");
        }
        else if (strcmp(argv[1], "-c") == 0)
        {
            i2c_write(MCU_ADDR, WATCHDOG_CLOSE, 2);
            printf("watchdog disabled\n");
        }
        else if (strcmp(argv[1], "-v") == 0)
        {
            printf("watchdog version 1.0\n");
        }
    }
    return 0;
}