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

#define GPIO_IOC_MAGIC 'T'
#define READ_GPIO _IO(GPIO_IOC_MAGIC, 0)
#define WRITE_GPIO _IO(GPIO_IOC_MAGIC, 1)
#define UART7_PINA 84
#define UART7_PINB 150
#define UART8_PINA 76
#define UART8_PINB 77

typedef struct gpio_source
{
    int gpioa;
    int gpiob;
} source;

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

int uart_232(int fd, source gpio)
{
    write_gpio(gpio.gpioa, 0, fd);
    write_gpio(gpio.gpiob, 0, fd);
    return 0;
}

int uart_485(int fd, source gpio)
{
    write_gpio(gpio.gpioa, 1, fd);
    write_gpio(gpio.gpiob, 0, fd);
    return 0;
}

int uart_422(int fd, source gpio)
{
    write_gpio(gpio.gpioa, 1, fd);
    write_gpio(gpio.gpiob, 1, fd);
    return 0;
}

int main(int argc, char *argv[])
{
    source uart7;
    source uart8;
    uart7.gpioa = UART7_PINA;
    uart7.gpiob = UART7_PINB;
    uart8.gpioa = UART8_PINA;
    uart8.gpiob = UART8_PINB;
    int FD;
    if (argc != 3)
    {
        printf("usage:please input right command\n");
        printf("exp:./test mmode \n");
        return -1;
    }
    else
    {
        FD = open("/dev/rockchip-gpio", O_RDWR);
        if (FD < 0)
        {
            printf("open file failed\n");
            return -1;
        }
        else
        {
            if (strcmp(argv[1], "-232") == 0)
            {
                if (strcmp(argv[2], "7") == 0)
                {
                    uart_232(FD, uart7);
                }
                else if (strcmp(argv[2], "8") == 0)
                {
                    uart_232(FD, uart8);
                }
                printf("uart select 232 mode\n");
            }
            else if (strcmp(argv[1], "-485") == 0)
            {
                if (strcmp(argv[2], "7") == 0)
                {
                    uart_485(FD, uart7);
                }
                else if (strcmp(argv[2], "8") == 0)
                {
                    uart_485(FD, uart8);
                }
                printf("uart select 485 mode\n");
            }
            else if (strcmp(argv[1], "-422") == 0)
            {
                if (strcmp(argv[2], "7") == 0)
                {
                    uart_422(FD, uart7);
                }
                else if (strcmp(argv[2], "8") == 0)
                {
                    uart_422(FD, uart8);
                }
                printf("uart select 422 mode\n");
            }
            else if (strcmp(argv[1], "-v") == 0)
            {
                printf("uart_mode version 1.0\n");
            }
        }
    }
    return 0;
}