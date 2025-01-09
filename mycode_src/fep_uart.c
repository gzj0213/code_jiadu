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
#include <stdint.h>

int i2c_write_bytes(int fd, uint8_t slave_addr, uint8_t reg_addr, uint8_t *values, uint8_t len)
{
    uint8_t *outbuf = NULL;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

    outbuf = malloc(len + 1);
    if (!outbuf)
    {
        printf("Error: No memory for buffer\n");
        return -1;
    }

    outbuf[0] = reg_addr;
    memcpy(outbuf + 1, values, len);

    messages[0].addr = slave_addr;
    messages[0].flags = 0;
    messages[0].len = len + 1;
    messages[0].buf = outbuf;

    /* Transfer the i2c packets to the kernel and verify it worked */
    packets.msgs = messages;
    packets.nmsgs = 1;
    if (ioctl(fd, I2C_RDWR, &packets) < 0)
    {
        printf("Error: slave_addr %0x Unable to send data\n", slave_addr);
        free(outbuf);
        return -1;
    }

    free(outbuf);

    return 0;
}

int set_serial_protocol(char port[], char type[])
{
    int i2c_fd;
    int i;
    char t = port[2];
    u_int8_t slave_addr;
    u_int8_t direct_buf[2];
    u_int8_t output_buf[2];

    i2c_fd = open("/dev/i2c-5", O_RDWR);
    if (i2c_fd < 0)
    {
        printf("open /dev/i2c-5 error\n");
        return -1;
    }

    if (strcmp(type, "232") == 0)
    {
        output_buf[0] = 0x00; // 将所有IO口的输出低电平
        output_buf[1] = 0x00;
    }

    else if (strcmp(type, "485") == 0)
    {
        output_buf[0] = 0xff; // 将所有IO口的输出低电平
        output_buf[1] = 0x00;
    }

    else if (strcmp(type, "422") == 0)
    {
        output_buf[0] = 0xff; // 将所有IO口的输出低电平
        output_buf[1] = 0xff;
    }

    else
    {
        close(i2c_fd);
        return -1;
    }

    direct_buf[0] = 0x00; // 将所有IO口设置为输出
    direct_buf[1] = 0x00;
    // i2c slave 地址分别是0x20,0x21,0x22,0x24,0x26
    /*
    for (i = 0; i < 4; i++)
    {
        slave_addr = 0x20 + i * 2;
        i2c_write_bytes(i2c_fd, slave_addr, 0x06, direct_buf, 2); // 0x06 0x07寄存器均写0
        i2c_write_bytes(i2c_fd, slave_addr, 0x02, output_buf, 2);
    }
    // 单独写0x21设备
    i2c_write_bytes(i2c_fd, 0x21, 0x06, direct_buf, 2);
    // i2c_write_bytes(i2c_fd, 0x21, 0x07, direct_buf + 1, 1);
    i2c_write_bytes(i2c_fd, 0x21, 0x02, output_buf, 2);
    // i2c_write_bytes(i2c_fd, 0x21, 0x03, output_buf + 1, 1);
    */
    if (strcmp(port, "-a") == 0)
    {
        for (i = 0; i < 4; i++)
        {
            slave_addr = 0x20 + i * 2;
            i2c_write_bytes(i2c_fd, slave_addr, 0x06, direct_buf, 2); // 0x06 0x07寄存器均写0
            i2c_write_bytes(i2c_fd, slave_addr, 0x02, output_buf, 2);
        }
        i2c_write_bytes(i2c_fd, 0x21, 0x06, direct_buf, 2);
        i2c_write_bytes(i2c_fd, 0x21, 0x02, output_buf, 2
        );
    }
    else
    {
        i = t - '0';
        if (i > 1)
        {
            slave_addr = 0x20 + (i - 2) * 2;
            i2c_write_bytes(i2c_fd, slave_addr, 0x06, direct_buf, 2); // 0x06 0x07寄存器均写0
            i2c_write_bytes(i2c_fd, slave_addr, 0x02, output_buf, 2);
        }
        else
        {
            i2c_write_bytes(i2c_fd, 0x21, 0x06, direct_buf, 2);
            i2c_write_bytes(i2c_fd, 0x21, 0x02, output_buf, 2);
        }
    }

    close(i2c_fd);
    return 0;
}