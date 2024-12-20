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
#define PIN_CTL 134
// #define PIN_CTL 39
#define HIGH 1
#define LOW 0
#define CMD "sudo cat /sys/class/thermal/thermal_zone0/temp"

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
    // unsigned char gpio_num;
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

unsigned char led_crl(int gpio, int fd, int temp, char *temp_threshold)
{
    if (temp > atoi(temp_threshold))
    {
        write_gpio(gpio, HIGH, fd);
    }
    else
    {
        write_gpio(gpio, LOW, fd);
    }
    return 0;
}

int temp_read()
{
    FILE *fp = NULL;
    int ret = 0;
    char buf[1024];
    char result[1024];
    if ((fp = popen(CMD, "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {
            strcat(result, buf);
            if (strlen(result) > 1024)
                break;
        }
        pclose(fp);
        fp = NULL;
    }
    else
    {
        printf("popen %s error", buf);
    }
    ret = atoi(buf);
    // system("sudo cat /sys/class/thermal/thermal_zone0/temp");
    ret = ret / 1000;
    // printf("temp = %d\n", ret);
    return ret;
}

int main(int argc, char *argv[])
{
    int file;
    int temp = 0;
    // daemon(0, 0);
    file = open_file();
    char *temp_set;

    temp_set = argv[1];
   // printf("temp=%s\n", argv[1]);

    if (file < 0)
    {
        printf("open rockchip-test failed\n");
        return FAIL;
    }
    else
        while (1)
        {
            temp = temp_read();
            led_crl(PIN_CTL, file, temp, temp_set);
            sleep(5);
        }
    return 0;
}