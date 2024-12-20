#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/err.h>
#include <linux/semaphore.h>

// IOCTL
#define GPIO_IOC_MAGIC 'T'
#define READ_GPIO _IO(GPIO_IOC_MAGIC, 0)
#define WRITE_GPIO _IO(GPIO_IOC_MAGIC, 1)
#define IO_CTL _IO(GPIO_IOC_MAGIC, 2)

struct semaphore g_sem; // 信号量,用于互斥访问

#define ON 1
#define OFF 0
#define LOW 0
#define HIG 1

#define DEVICE_NAME "rockchip-gpio"

struct gpios_data
{
    const char *label;
    bool input;
    unsigned gpio;
    unsigned dft;
};

static struct gpios_misc
{
    struct miscdevice misc;
    struct gpios_data *data;
    int gpio_count;
} *gpios_misc;

static int gpios_open(struct inode *inodp, struct file *filp)
{
    return 0;
}

static int gpios_release(struct inode *inodp, struct file *filp)
{
    if (!gpios_misc)
        return -ENODEV;
    return 0;
}

unsigned char gpio_write_index(int idx, int value)
{
    // gpio_direction_output(idx, LOW);
    gpio_direction_output(idx, value);
    gpio_set_value(idx, value);

    return 0x00;
}

unsigned char gpio_read_index(int idx)
{
    unsigned int gpio_vlue;
    gpio_direction_input(idx);
    gpio_vlue = gpio_get_value(idx);
    return gpio_vlue;
}

static long gpios_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    // int beep_freq;
    unsigned char __data[255] = {0};
    unsigned char status = 0;
    unsigned long res = 0;
    unsigned char gpio_num = 0;
    unsigned char read_data[1];

    if (down_interruptible(&g_sem) != 0)
    {
        return -EBUSY;
    }
    else
    {

        if (_IOC_TYPE(cmd) != GPIO_IOC_MAGIC)
            return -ENOTTY;

        switch (cmd)
        {
        case READ_GPIO:
            res = copy_from_user(__data, (unsigned char *)arg, 3);
            if (res == 0)
            {
                gpio_num = (__data[0] << 8) + __data[1];
            }
            read_data[0] = gpio_read_index(gpio_num);
            res = copy_to_user((unsigned char *)arg, read_data, 1);
            break;

        case WRITE_GPIO:
            res = copy_from_user(__data, (unsigned char *)arg, 3);
            if (res == 0)
            {
                gpio_num = (__data[0] << 8) + __data[1];
                status = gpio_write_index(gpio_num, __data[2]);
            }
            break;

        default:
            status = -EINVAL;
            break;
        }
    }
    up(&g_sem);
    return 0;
}

static const struct file_operations gpios_fops = {
    .owner = THIS_MODULE,
    .open = gpios_open,
    .release = gpios_release,
    .unlocked_ioctl = gpios_ioctl,
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &gpios_fops,
};

static int __init gpios_driver_init(void)
{
    misc_register(&misc);
    sema_init(&g_sem, 1);
    return 0;
}

static void __exit gpios_driver_exit(void)
{
    misc_deregister(&misc);
}

module_init(gpios_driver_init);
module_exit(gpios_driver_exit);

MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:gpios");
