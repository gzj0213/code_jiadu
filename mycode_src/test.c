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

int set_serial_protocol(char port[], char type[]);

int main(void)
{
    char port[] = "-a";
    char type[] = "485";

    printf("intput port %c\n", port[2]);
    set_serial_protocol(port, type);
    return 0;
}