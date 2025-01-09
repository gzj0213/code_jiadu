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
#include <fcntl.h> /* open */

#include <stdlib.h>
#include <string.h>
#include <linux/mii.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#define FAIL -1

#define gpio_num1 27
#define gpio_num2 106

#define GPIO_IOC_MAGIC 'T'
#define READ_GPIO _IO(GPIO_IOC_MAGIC, 0)
#define WRITE_GPIO _IO(GPIO_IOC_MAGIC, 1)
#define IO_CTL _IO(GPIO_IOC_MAGIC, 2)
#define configfile "/home/linaro/test.conf"
// #define mount_buf "sudo cp -rf /home/linaro/AGM "

static int gpio_fd = -1;
static int test_times = 0;

/*char uart_dev[] = "ttyS0,ttyS1,ttyS3,ttyS4,\
                ttyWCH0,ttyWCH1,ttyWCH2,ttyWCH3,ttyWCH4,ttyWCH5,\
                ttyWCH6,ttyWCH7,ttyWCH8,ttyWCH9,ttyWCH10,ttyWCH11,\
                ttyWCH12,ttyWCH13,ttyWCH14,ttyWCH15,ttyWCH16,ttyWCH17,\
                ttyWCH18,ttyWCH19,ttyWCH20,ttyWCH21,ttyWCH22,ttyWCH23,\
                ttyWCH24,ttyWCH25,ttyWCH26,ttyWCH27,ttyWCH28,ttyWCH29,\
                ttyWCH30,ttyWCH31";

char lan_dev[] = "eth0,eth1,enP4p65s0";
char extlan_dev[] = "enP1p65s0,enP1p6s0,enP1p67s0,enP1p68s0,enP1p71s0,\
                enP1p72s0,enP1p73s0,enP1p74s0,enP1p53s0,enP1p54s0,\
                enP1p55s0,enP1p56s0,enP1p59s0,enP1p60s0,enP1p61s0,\
                enP1p62s0,enp5s0,enp6s0,enp7s0,enp8s0,enp11s0,\
                enp12s0,enp13s0,enp14s0,enp17s0,enp18s0,enp19s0,\
                enp20s0,enp23s0,enp24s0,enp25s0,enp26s0,enP3p81s0";
char display_dev[] = "HDMI-1,HDMI-2,DP-1,DP-2";
char minipcie_dev[] = "enP2p35s0,enP2p36s0";
char usb_dev[] = "Logitech,Lenovo,Lite-On,Terminus,QinHeng";*/
char can_dev[] = "can0";

typedef struct _TEST_ITEM
{
    int uart_num;
    int loclan_num;
    int extlan_num;
    int lan_num;
    int usb_num;
    int display_num;
    int minipcie_num;
    int ssd_num;
    int can_num;

    char uart_test_val[10];
    char lan_test_val[10];
    char usb_test_val[10];
    char display_test_val[10];
    char minipcie_test_val[10];
    char can_test_val[10];
    char soundcard_test_val[10];
    char ssd_test_val[10];
    char sata_test_val[10];
    char vga_test_val[10];

    char ssd_mount_dir[20];

    char minipcie_dev[1024];
    char lan_dev[1024];
    char usb_dev[1024];
    char uart_dev[1024];
    char display_dev[1024];
    char sata_dev[1024];
    char can_dev[1024];
    char soundcard_dev[1024];
    char ssd_dev[1024];
} TEST_ITEM;

TEST_ITEM item;

/*
int is_in(char source_word[], char search_word[], int search_len, char *adjust_word)
{
    int i, j;
    int flag = FAIL;
    static char comb[1024];
    static int source_len;
    //, search_len;
    // char adjust_word[1024];

    source_len = strlen(source_word);
    // search_len = strlen(search_word);
    // memcpy(adjust_word, search_word, (search_len - 1));
    for (i = 0; i < source_len; i++)
    {
        if (source_word[i] == search_word[0])
        {
            for (j = 0; j < search_len - 1; j++)
            {
                comb[j] = source_word[i + j];

                if (strcmp(comb, adjust_word) == 0)
                {
                    flag = 1;
                    printf("comb=%s\n", comb);
                    return flag;
                }
                else
                {
                    flag = FAIL;
                }
            }
        }
    }
    return flag;
}*/

int GetIniKeyString(char *title, char *key, char *buf)
{
    static FILE *initfile_fp;
    int flag = 0;
    char sTitle[64], *wTmp;
    char sLine[1024];
    sprintf(sTitle, "[%s]", title);

    if (NULL == (initfile_fp = fopen(configfile, "r")))
    {
        printf("读取配置文件失败\n");
        perror("fopen");
        // return -1;
        exit(0);
    }
    while (NULL != fgets(sLine, 1024, initfile_fp))
    {
        // 这是注释行
        if (0 == strncmp("//", sLine, 2))
            continue;
        if ('#' == sLine[0])
            continue;
        wTmp = strchr(sLine, '=');
        if ((NULL != wTmp) && (1 == flag))
        {
            if (0 == strncmp(key, sLine, strlen(key)))
            { // 长度依文件读取的为准
                sLine[strlen(sLine) - 1] = '\0';
                fclose(initfile_fp);
                while (*(wTmp + 1) == ' ')
                {
                    wTmp++;
                }
                strcpy(buf, wTmp + 1);
                return 0;
            }
        }
        else
        {
            if (0 == strncmp(sTitle, sLine, strlen(sTitle)))
            {             // 长度依文件读取的为准
                flag = 1; // 找到标题位置
            }
        }
    }
    fclose(initfile_fp);
    return -1;
}

void config_init(void)
{
    char uart_num[2];
    char usb_num[2];
    // char ext_num[2];
    // char loc_num[2];
    char lan_num[2];
    char display_num[2];
    char minipcie_num[2];
    char ssd_num[2];
    char can_num[2];

    GetIniKeyString("setting", "uart_dev", item.uart_dev);
    GetIniKeyString("setting", "uart_num", uart_num);
    // GetIniKeyString("setting", "extlan_dev", item.ext_lan);
    // GetIniKeyString("setting", "loclan_dev", item.loc_lan);
    // GetIniKeyString("setting", "extlan_num", ext_num);
    // GetIniKeyString("setting", "loclan_num", loc_num);
    GetIniKeyString("setting", "lan_dev", item.lan_dev);
    GetIniKeyString("setting", "lan_num", lan_num);
    GetIniKeyString("setting", "usb_dev", item.usb_dev);
    GetIniKeyString("setting", "usb_num", usb_num);
    GetIniKeyString("setting", "display_dev", item.display_dev);
    GetIniKeyString("setting", "display_num", display_num);
    GetIniKeyString("setting", "minipcie_dev", item.minipcie_dev);
    GetIniKeyString("setting", "minipcie_num", minipcie_num);
    GetIniKeyString("setting", "ssd_num", ssd_num);
    GetIniKeyString("setting", "can_num", can_num);

    GetIniKeyString("setting", "minipcie_test", item.minipcie_test_val);
    GetIniKeyString("setting", "ssd_test", item.ssd_test_val);
    GetIniKeyString("setting", "sata_test", item.sata_test_val);
    GetIniKeyString("setting", "soundcard_test", item.soundcard_test_val);
    GetIniKeyString("setting", "display_test", item.display_test_val);
    GetIniKeyString("setting", "uart_test", item.uart_test_val);
    GetIniKeyString("setting", "lan_test", item.lan_test_val);
    GetIniKeyString("setting", "usb_test", item.usb_test_val);
    GetIniKeyString("setting", "can_test", item.can_test_val);
    GetIniKeyString("setting", "vga_test", item.vga_test_val);

    GetIniKeyString("setting", "ssd_mount_dir", item.ssd_mount_dir);

    GetIniKeyString("setting", "sata_dev", item.sata_dev);
    GetIniKeyString("setting", "can_dev", item.can_dev);
    GetIniKeyString("setting", "soundcard_dev", item.soundcard_dev);
    GetIniKeyString("setting", "ssd_dev", item.ssd_dev);

    item.uart_num = atoi(uart_num);
    item.usb_num = atoi(usb_num);
    // item.extlan_num = atoi(ext_num);
    // item.loclan_num = atoi(loc_num);
    item.lan_num = atoi(lan_num);
    item.display_num = atoi(display_num);
    item.minipcie_num = atoi(minipcie_num);
    item.ssd_num = atoi(ssd_num);
    item.can_num = atoi(can_num);
}

int is_in(char source_word[], char search_word[])
{
    int i, j, k, n;
    int flag = FAIL;
    // char comb[1024];
    char *comb;
    int source_len, search_len;
    char adjust_word[1024];

    source_len = strlen(source_word);
    search_len = strlen(search_word);
    comb = (char *)malloc(search_len);

    for (i = 0; i < search_len; i++)
    {
        if (search_word[i] == '\n')
        {
            search_word[i] = '\0';
        }
    }

    memcpy(adjust_word, search_word, (search_len));

    for (i = 0; i < source_len; i++)
    {
        if (source_word[i] == search_word[0])
        {
            for (j = 0; j < search_len - 1; j++)
            {
                comb[j] = source_word[i + j];

                // 去除数组空余

                /*for (k = 0; comb[k]; k++)
                {
                    if (comb[k] == ' ')
                    {
                        comb[k+1] = '\0';
                    }
                }*/
            }
            for (k = 0, n = 0; comb[k]; k++)
            {
                if (comb[k] != ' ')
                {
                    comb[n++] = comb[k];
                }
            }
            comb[n] = '\0';
            // printf("comb=%s\n", comb);
            if (strcmp(comb, adjust_word) == 0)
            {
                flag = 1;
                printf("comb=%s\n", comb);
                return flag;
            }
            else
            {
                flag = FAIL;
            }
        }
    }
    return flag;
}

int uart_test()
{
    static FILE *fp = NULL;
    char buf[1024];
    char uart_buf[1024];
    int len;
    static int uart_num = 0;

    if ((fp = popen("ls /dev/|grep ttyS", "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {

            if (is_in(item.uart_dev, buf) == 1)
                uart_num = uart_num + 1;
        }
        pclose(fp);
        fp = NULL;
        // printf("uart num=%d\n", uart_num);
    }

    if ((fp = popen("ls /dev/|grep ttyW", "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {
            if (is_in(item.uart_dev, buf) == 1)
                uart_num = uart_num + 1;
        }
        pclose(fp);
        fp = NULL;
    }

    if (uart_num == item.uart_num)
    {
        printf("uart device start success!\n");
        return 0;
    }
    else
    {
        if (test_times == 3)
        {
            printf("uart not start totally!\n");
            exit(0);
        }
        else
        {
            sleep(3);
            printf("uart test times=%d\n", test_times);
            test_times++;
            uart_test();
        }
    }
}

// int display_test(int num)
int display_test()
{
    static FILE *fp = NULL;
    char buf[1024];
    int display_num = 0;

    if ((fp = popen("xrandr |grep -w connected|awk '{print $1}'", "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {
            if (is_in(item.display_dev, buf) == 1)
                display_num = display_num + 1;
        }
        pclose(fp);
        fp = NULL;
    }
    
    if (display_num == item.display_num)
    {
        printf("display start success!\n");
        return 0;
    }
    else
    {
        if (test_times == 3)
        {
            printf("display not start totally!\n");
            exit(0);
        }
        else
        {
            sleep(3);
            printf("display test times=%d\n", test_times);
            test_times++;
            display_test();
        }
    }
}

/*
int locallan_test()
{
    static FILE *fp = NULL;
    char buf[1024];
    static int lan_num = 0;

    if ((fp = popen("sudo ifconfig |grep eth|grep -v ether|awk -F ':' '{print $1}'", "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {
            if (is_in(item.loc_lan, buf) == 1)
                lan_num = lan_num + 1;
        }
        pclose(fp);
        fp = NULL;
    }

    if (lan_num == item.loclan_num)
    {
        printf("local-lan start success!\n");
        return 0;
    }
    else
    {
        printf("local-lan not start totally!\n");
        exit(0);
    }
}

int extlan_test()
{
    static FILE *fp = NULL;
    char buf[1024];
    static int lan_num = 0;

    if ((fp = popen("sudo ifconfig |grep enP|grep -v ether|awk -F ':' '{print $1}'", "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {
            if (is_in(item.lan_num, buf) == 1)
                lan_num = lan_num + 1;
        }
        pclose(fp);
        fp = NULL;
    }

    if (lan_num == item.lan_num)
    {
        printf("lan start success!\n");
        return 0;
    }
    else
    {
        printf("lan not start totally!\n");
        exit(0);
    }
}*/

int lan_test()
{
    static FILE *fp = NULL;
    char buf[1024];
    int lan_num = 0;

    if ((fp = popen(" ifconfig |grep en|grep flags|awk -F ':' '{print $1}'", "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {
            if (is_in(item.lan_dev, buf) == 1)
                lan_num = lan_num + 1;
        }
        pclose(fp);
        fp = NULL;
    }

    if ((fp = popen(" ifconfig |grep eth|grep -v ether|awk -F ':' '{print $1}'", "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {
            if (is_in(item.lan_dev, buf) == 1)
                lan_num = lan_num + 1;
        }
        pclose(fp);
        fp = NULL;
    }

    if (lan_num == item.lan_num)
    {
        printf("lan device start success!\n");
        return 0;
    }
    else
    {
        if (test_times == 3)
        {
            printf("lan device not start totally!\n");
            exit(0);
        }
        else
        {
            sleep(3);
            printf("lan test times=%d\n", test_times);
            test_times++;
            lan_test();
        }
    }
}

int can_test()
{
    static FILE *fp = NULL;
    char buf[1024];
    int can_num = 0;
    static char can_cmd[1024];

    sprintf(can_cmd, " sudo ifconfig -a |grep %s |awk -F ':' '{print $1}'", item.can_dev);

    if ((fp = popen(can_cmd, "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {
            if (is_in(can_dev, buf) == 1)
                can_num = can_num + 1;
        }
        pclose(fp);
        fp = NULL;
    }

    if (can_num == item.can_num)
    {
        printf("can device start success!\n");
        return 0;
    }
    else
    {
        if (test_times == 3)
        {
            printf("can start failed!\n");
            exit(0);
        }
        else
        {
            sleep(3);
            printf("can test times=%d\n", test_times);
            test_times++;
            can_test();
        }
    }
}

int ssd_test()
{
    static FILE *fp = NULL;
    char buf[1024];
    static char mount_buf[1024] = "sudo cp -rf /home/linaro/AGM ";
    static char ssd_cmd[1024];
    int ssd_num = 0;

    sprintf(ssd_cmd, "df -h|grep nvme", item.ssd_dev);

    if (0)
    {
        if ((fp = popen(ssd_cmd, "r")) != NULL)
        {
            fgets(buf, 1024, fp);
            pclose(fp);
            fp = NULL;
        }

        if (strlen(buf) != 0)
        {
            printf("ssd start success!\n");
            strcat(mount_buf, item.ssd_mount_dir);
            // printf("%s\n",mount_buf);
            system(mount_buf);
            return 0;
        }
        else
        {
            if (test_times == 3)
            {
                printf("ssd start failed!\n");
                exit(0);
            }
            else
            {
                sleep(3);
                printf("ssd test times=%d\n", test_times);
                test_times++;
                ssd_test();
            }
        }
    }

    if ((fp = popen("ls /dev/ |grep nvme", "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {
            if (is_in(item.ssd_dev, buf) == 1)
                ssd_num = ssd_num + 1;
        }
        pclose(fp);
        fp = NULL;
    }

    if (ssd_num == item.ssd_num)
    {
        printf("ssd start success!\n");
        return 0;
    }
    else
    {
        if (test_times == 3)
        {
            printf("ssd not start totally!\n");
            exit(0);
        }
        else
        {
            sleep(3);
            printf("ssd test times=%d\n", test_times);
            test_times++;
            ssd_test();
        }
    }
}

int sata_test()
{
    static FILE *fp = NULL;
    static char buf[1024];
    static char sata_cmd[1024];

    sprintf(sata_cmd, "df -h|grep %s", item.sata_dev);

    if ((fp = popen(sata_cmd, "r")) != NULL)
    {
        fgets(buf, 1024, fp);
        pclose(fp);
        fp = NULL;
    }

    if (strlen(buf) != 0)
    {
        printf("sata start success!\n");
        return 0;
    }
    else
    {
        if (test_times == 3)
        {
            printf("sata start failed!\n");
            exit(0);
        }
        else
        {
            sleep(3);
            printf("sata test times=%d\n", test_times);
            test_times++;
            sata_test();
        }
    }
}

int soundcard_test()
{
    static FILE *fp = NULL;
    static char buf[1024];
    static char soundcard_cmd[1024];

    sprintf(soundcard_cmd, "pactl list short sinks|grep %s", item.soundcard_dev);

    if ((fp = popen(soundcard_cmd, "r")) != NULL)
    {
        fgets(buf, 1024, fp);
        fp = NULL;
    }

    if (strlen(buf) != 0)
    {
        printf("soundcard start success!\n");
        return 0;
    }
    else
    {
        if (test_times == 3)
        {
            printf("soundcard not start totally!\n");
            exit(0);
        }
        else
        {
            sleep(3);
            printf("soundcard test times=%d\n", test_times);
            test_times++;
            soundcard_test();
        }
    }
}

int minipcie_test()
{
    static FILE *fp = NULL;
    char buf[1024];
    int minipcie_num = 0;

    if ((fp = popen("sudo ifconfig |grep enP2p|awk -F ':' '{print $1}'", "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {
            if (is_in(item.minipcie_dev, buf) == 1)
                minipcie_num = minipcie_num + 1;
        }
    }

    if (minipcie_num == item.minipcie_num)
    {
        printf("minipcie start success!\n");

        return 0;
    }
    else
    {
        if (test_times == 3)
        {
            printf("minipcie not start totally!\n");
            exit(0);
        }
        else
        {
            sleep(3);
            printf("minipcie test times=%d\n", test_times);
            test_times++;
            minipcie_test();
        }
    }
}

void writeGpio(int idx, int value)
{
    unsigned char tmp[3];

    tmp[0] = (unsigned char)(idx >> 8);
    tmp[1] = (unsigned char)(idx & 0xFF);
    tmp[2] = (unsigned char)value;
    ioctl(gpio_fd, WRITE_GPIO, tmp);
}

unsigned char readGpio(int idx)
{
    unsigned char tmp[3];

    tmp[0] = (unsigned char)(idx >> 8);
    tmp[1] = (unsigned char)(idx & 0xFF);
    ioctl(gpio_fd, READ_GPIO, tmp);

    return tmp[0];
}

unsigned char gpio_init(void)
{
    if (gpio_fd == -1)
        gpio_fd = open("/dev/rockchip-gpio", O_RDWR);

    if (gpio_fd > 0)
    {

        return 0;
    }
    else
    {
        return 1;
    }
}

unsigned char gpio_close(void)
{
    int fdsta;

    fdsta = close(gpio_fd);
    if (fdsta == 0)
    {
        gpio_fd = -1;
        return 0x00;
    }
    else
    {
        return 1;
    }
}

int vga_test()
{
    unsigned int mode1 = 0;
    unsigned int mode2 = 0;
    unsigned char retvar = 1;
    unsigned int i, j;
    retvar = gpio_init();
    //   printf("gpio_init retvar = %d \n", retvar);

    // if(argc == 1 || !strcmp(argv[1], "-h")){
    //     help();
    // }

    for (i = 0; i < 20000; i++)
    {
        // mode1 = readGpio(gpio_num2);
        //		printf("get gpio_pin: %d,status = %d \n",gpio_pin, mode);

        mode1 = readGpio(gpio_num1);
        if (1 == mode1)
        {
            /*for (j = 0; j < 20000; j++)
            {
                mode2 = readGpio(gpio_num2);
                if (1 == mode2)
                {
                    printf("vga signal1=%d,vga signal2=%d\n", mode1, mode2);
                    return 0;
                }
            }*/
            printf("vga signal1=%d\n", mode1);
            return 0;
        }
    }
    return 1;
}

int usb_test()
{
    static FILE *fp = NULL;
    char buf[1024];
    static int usb_num = 0;

    if ((fp = popen("lsusb |awk '{print $7}'", "r")) != NULL)
    {
        while (fgets(buf, 1024, fp) != NULL)
        {
            if (is_in(item.usb_dev, buf) == 1)
                usb_num = usb_num + 1;
        }
        if (usb_num == item.usb_num)
        {
            printf("usb start success!\n");
            pclose(fp);
            fp = NULL;
            return 0;
        }
        else
        {
            if (test_times == 3)
            {
                printf("usb not start totally\n");
                exit(0);
            }
            else
            {
                sleep(3);
                printf("usb test times=%d\n", test_times);
                test_times++;
                usb_test();
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int ch;
    int num;
    int val = 1;

    config_init();

    /*while ((ch = getopt(argc, argv, "ouspmclaevdh")) != -1)
    {
        switch (ch)
        {
        case 'o':
            soundcard_test();
            break;
        case 'u':
            usb_test();
            break;
        case 's':
            sata_test();
            break;
        case 'p':
            ssd_test();
            break;
        case 'm':
            minipcie_test();
            break;
        case 'c':
            uart_test();
            break;
        case 'l':
            locallan_test();
            break;
        case 'a':
            can_test();
            break;
        case 'e':
            extlan_test();
            break;
        case 'v':
            val = vga_test();
            if (val != 0)
            {
                printf("vga signal detect fail\n");
                exit(0);
            }
            else
                printf("vga signal detect success\n");
            break;
        case 'd':
            //num = atoi(optarg);
            //display_test(num);
            display_test();
            break;

        default:;
        }
    }*/

    if (strcmp(item.can_test_val, "true") == 0)
    {
        can_test();
    }
    test_times = 0;

    if (strcmp(item.display_test_val, "true") == 0)
    {
        display_test();
    }
    test_times = 0;

    if (strcmp(item.uart_test_val, "true") == 0)
    {
        uart_test();
    }
    test_times = 0;

    if (strcmp(item.lan_test_val, "true") == 0)
    {
        lan_test();
    }
    test_times = 0;

    if (strcmp(item.ssd_test_val, "true") == 0)
    {
        ssd_test();
    }
    test_times = 0;

    if (strcmp(item.sata_test_val, "true") == 0)
    {
        sata_test();
    }
    test_times = 0;

    if (strcmp(item.usb_test_val, "true") == 0)
    {
        usb_test();
    }
    test_times = 0;

    if (strcmp(item.minipcie_test_val, "true") == 0)
    {
        minipcie_test();
    }
    test_times = 0;

    if (strcmp(item.soundcard_test_val, "true") == 0)
    {
        soundcard_test();
    }
    test_times = 0;

    if (strcmp(item.vga_test_val, "true") == 0)
    {
        val = vga_test();
        if (val != 0)
        {
            printf("vga signal detect fail\n");
            exit(0);
        }
        else
            printf("vga signal detect success\n");
    }
    test_times = 0;

    for (int i = 0; i < 3; i++)
    {
        printf("auto-test already success!\n");
        sleep(1);
    }

    return 0;
}