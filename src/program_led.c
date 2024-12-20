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
#include <syslog.h>

#define GPIO_IOC_MAGIC 'T'
#define READ_GPIO _IO(GPIO_IOC_MAGIC, 0)
#define WRITE_GPIO _IO(GPIO_IOC_MAGIC, 1)
#define IO_CTL _IO(GPIO_IOC_MAGIC, 2)
#define FAIL -1
#define HIGH 1
#define LOW 0
#define PROG1 135
#define PROG2 106
#define ERR_PIN 122

int open_file(void)
{
    int fd;
    fd = open("/dev/rockchip-gpio", O_RDWR);
    if (fd == FAIL)
    {
        return FAIL;
    }
    return fd;
}

int write_gpio(int gpio_num, int value, int fd)
{
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

int read_gpio(int gpio_num, int fd)
{
    unsigned char str[3];

    if (gpio_num > 256)
    {
        str[0] = (unsigned char)(gpio_num >> 8);
        str[1] = (unsigned char)255;
        ioctl(fd, READ_GPIO, str);
    }
    else
    {
        str[0] = 0;
        str[1] = gpio_num;
        ioctl(fd, READ_GPIO, str);
    }
}

int main(int argc, char *argv[])
{
    int FD;
    if (argc != 3)
    {
        printf("usage:please input right command\n");
        printf("exp:./programe_led -p1 \n");
        return FAIL;
    }
    else
    {
        FD = open_file();
        if (strcmp(argv[1], "p1") == 0)
        {
            if (strcmp(argv[2], "-o") == 0)
            {
                write_gpio(PROG1, HIGH, FD);
            }
            else if (strcmp(argv[2], "-c") == 0)
            {
                write_gpio(PROG1, LOW, FD);
            }
        }

        else if (strcmp(argv[1], "p2") == 0)
        {
            if (strcmp(argv[2], "-o") == 0)
            {
                write_gpio(PROG2, HIGH, FD);
            }
            else if (strcmp(argv[2], "-c") == 0)
            {
                write_gpio(PROG2, LOW, FD);
            }
        }

        else if (strcmp(argv[1], "err") == 0)
        {
            if (strcmp(argv[2], "-o") == 0)
            {
                write_gpio(ERR_PIN, HIGH, FD);
            }
            else if (strcmp(argv[2], "-c") == 0)
            {
                write_gpio(ERR_PIN, LOW, FD);
            }
        }

        else if (strcmp(argv[1], "-v") == 0)
        {
            printf("program_led version 1.0.0\n");
        }

        else
        {
            printf("selcet correct mode!\n");
            printf("p1=PROG1 p2=PROG2 err=error\n");
            return FAIL;
        }
    }
    return 0;
}