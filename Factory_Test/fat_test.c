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

#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <linux/rtc.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

#include "source/libfuns/libfuns/libserial.h"
#include "zlog.h"
#define filename "fat.conf"
// #define logfile "FACTORY_TEST.log"
#define POLYNOMIAL 0xEDB88320

typedef struct __PARAM
{
    char ip[50][20];
} PARAM;
PARAM net;

typedef struct __UARTINIT
{
    char dev_tx[20][30];
    char dev_rx[20][30];
    int parity;
    int databit;
    int baudrate;
    int uart_num;
    int tx_fd[1024];
    int rx_fd[1024];
    int buflen;
} UARTINIT;

UARTINIT uart_conf;

uint32_t crc_table[256];

pthread_t net_threads[100];
int debug_mode = 0;
int auto_mode = 0;
static char logfile[50] = "FACTORY_TEST.log";

/*zlog_category_t *zc;
void log_init(void)
{
    int rc;

    char hex_buf[32];

    rc = zlog_init("zlog.conf"); // 指定配置文件路径，若当前文件夹直接写名字
    if (rc)
    {
        printf("EEROR: zlog init failed\n");
    }

    zc = zlog_get_category("zlog");
    if (!zc)
    {
        printf("EEROR: zlog get cat fail\n");
        zlog_fini();
    }
}*/

int GetIniKeyString(char *title, char *key, char *buf)
{
    static FILE *initfile_fp;
    int flag = 0;
    char sTitle[64], *wTmp;
    char sLine[1024];
    sprintf(sTitle, "[%s]", title);

    if (NULL == (initfile_fp = fopen(filename, "r")))
    {
        printf("读取配置文件失败\n");
        perror("fopen");
        return -1;
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

int logfile_init(void)
{
    static char rxbuf[30];
    int fd, len_sum, rx_len, ret;
    char val;
    char scan_dev[30];

    ret = GetIniKeyString("setting", "scan_dev", scan_dev);
    if (ret == 0)
    {
        // printf("读取的数据为:%s\n", serial_dev_tx);
    }
    else
    {
        printf("请指定条形码扫描设备名\n");
        exit(0);
    }

    fd = libserial_init(scan_dev, 115200, atoi("n"), 8, 1, 0, 0);
    printf("等待条形码扫描\n");

    while (1)
    {
        len_sum = read(fd, rxbuf, 30);

        if (len_sum > 0)
        {
            strncat(logfile, rxbuf, len_sum - 1);
            // memset(rxbuf, 0, sizeof(rxbuf));
            printf("扫码结果为%s\n", rxbuf);
            printf("条形码是否已扫描成功(Y or N):");
            scanf("%s", &val);
            if (val == 'Y' || val == 'y')
            {
                // printf("logfile = %s\n", logfile);
                return 0;
            }
            else if (val == 'N' || val == 'n')
            {
                logfile_init();
            }
        }
    }
}

int select_option(void)
{
    static char test_selection;
    printf("请选择是否继续测试?(Y or N):");
    scanf("%s", &test_selection);
    if (test_selection == 'Y' || test_selection == 'y')
    {
        return 1;
    }
    else if (test_selection == 'N' || test_selection == 'n')
    {
        exit(0);
    }
    else
    {
        printf("请输入正确值(Y or N)\n");
        select_option();
    }
}

int fail_info(void)
{
    static int ret;

    if (auto_mode == 1)
    {
        ret = select_option();
        if (ret == 1)
        {
            return 0;
        }
        else
            exit(0);
    }
    else
        return 0;
}

int OutputResult(char *pFileName, char *log_buf) // 这是要保存的名字和地址
{
    FILE *pFile;
    time_t now_time;
    char *time_c;
    int i;

    time(&now_time);
    pFile = fopen(pFileName, "a"); // 这个用“w”是写文件，覆盖原内容，若不想覆盖则用“a”
    if (NULL == pFile)
    { // 文件打开错误
        printf("error");
        return 0;
    }

    time_c = ctime(&now_time);

    for (i = 0; i < strlen(time_c); i++)
    {
        if (*(time_c + i) == '\n')
        {

            *(time_c + i) = '\0';
        }
    }

    fprintf(pFile, "%s:%s", time_c, log_buf);
    fclose(pFile); // 最后一定记得关闭文件
}

void debug_info(char *debug_buf)
{
    if (debug_mode)
    {
        printf("%s", debug_buf);
    }
    OutputResult(logfile, debug_buf);
}

void windows_info(char *window_buf)
{
    printf("%s", window_buf);
    OutputResult(logfile, window_buf);
}

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
        // printf("Error: slave_addr %0x Unable to send data\n", slave_addr);
        free(outbuf);
        return -1;
    }

    free(outbuf);

    return 0;
}

int set_serial_protocol(char port[], char type[])
{
    int i2c_fd;
    static int i, ret = 1;
    char t = port[2];
    u_int8_t slave_addr;
    u_int8_t direct_buf[2];
    u_int8_t output_buf[2];

    i2c_fd = open("/dev/i2c-8", O_RDWR);
    if (i2c_fd < 0)
    {
        printf("open /dev/i2c-8 error\n");
        ret = fail_info();
        return ret;
    }

    if (strcmp(type, "232") == 0 || strcmp(type, "1") == 0)
    {
        output_buf[0] = 0x00; // 将所有IO口的输出低电平
        output_buf[1] = 0x00;
    }

    else if (strcmp(type, "485") == 0 || strcmp(type, "2") == 0)
    {
        output_buf[0] = 0xff; // 将所有IO口的输出低电平
        output_buf[1] = 0x00;
    }

    else if (strcmp(type, "422") == 0 || strcmp(type, "3") == 0)
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
        i2c_write_bytes(i2c_fd, 0x21, 0x02, output_buf, 2);
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
    return ret;
}

void *run_ping(void *ipaddr)
{
    PARAM netbuf = *(PARAM *)ipaddr;
    int result = 0;
    pid_t pid;

    static FILE *net_fp = NULL;
    char ping_buf[1024];
    char debug_buf[1024];
    static FILE *fp = NULL;
    int fd, nfd;

    fd = open(logfile, O_WRONLY | O_APPEND | O_CREAT, 0644);
    nfd = dup(STDOUT_FILENO);

    if ((pid = vfork()) < 0)
    {
        printf("%s: vfork error: %s\n", __FUNCTION__, strerror(errno));
        // return -1;
    }
    else if (pid == 0)
    { // 子进程
        printf("ip=%s\n", netbuf.ip);
        if (!debug_mode)
        {
            dup2(fd, STDOUT_FILENO);
        }
        close(fd);
        if (execlp("ping", "ping", netbuf.ip, "-s", "65500", "-c", "50", (char *)0) < 0)
        {
            printf("%s: execlp error: %s\n", __FUNCTION__, strerror(errno));
        }
    }

    waitpid(pid, &result, 0);
    if (WIFEXITED(result))
    {
        if (WEXITSTATUS(result) != 0)
        {
            printf("%s: execute command: %s failed(%d)\n", __FUNCTION__, "ping", WEXITSTATUS(result));
            result = -1;
        }
        else
        {
            result = 0;
        }
    }
    else
    {
        result = 0;
    }

    if (result)
    {
        // printf("ping %s error!\n", netbuf.ip);
        sprintf(debug_buf, "(FAILED)ping%s失败!\n", netbuf.ip);
        debug_info(debug_buf);
        pthread_exit((void *)&result);
        // exit(0);
        //  return 0;
    }
    else
    {
        // printf("ping %s ok!\n", netbuf.ip);
        sprintf(debug_buf, "(SUCCESS)ping%s成功!\n", netbuf.ip);
        debug_info(debug_buf);
        pthread_exit((void *)&result);
        // return 0;
    }
}

int network_test(void)
{
    static FILE *net_fp = NULL;
    static int ret;
    int i = 0, j = 0, k = 0, a = 0, b = 0, val = 0;
    static char ip_buf[1024];
    static int *pret;

    windows_info("--------------网络测试开始--------------\n");

    if ((net_fp = popen("nmcli device show|grep IP4.ADDRESS |awk '{print $2}'|awk -F '/' '{print $1}'", "r")) != NULL)
    {
        while (fgets(ip_buf, 1024, net_fp) != NULL)
        {
            for (int i = 0; i < strlen(ip_buf); i++)
            {
                net.ip[k][i] = ip_buf[i];
                if (net.ip[k][i] == '\n')
                {
                    net.ip[k][i] = '\0';
                    k = k + 1;
                }
            }
            j = j + 1;
        }

        for (k = 0; k < j; k++)
        {
            if (strcmp(net.ip[k], "127.0.0.1") != 0)
            {
                for (a = 0; a < strlen(net.ip[k]); a++)
                {
                    if (net.ip[k][a] == '.')
                    {
                        b = b + 1;
                        if (b == 3)
                        {
                            net.ip[k][a + 1] = '1';
                            net.ip[k][a + 2] = '1';
                            net.ip[k][a + 3] = '\0';
                            b = 0;
                        }
                    }
                }
            }
        }

        pclose(net_fp);
        net_fp = NULL;

        for (k = 0; k < j; k++)
        {
            pthread_create(&net_threads[k], NULL, run_ping, (void *)&net.ip[k]);
        }

        /*for (k = 0; k < j; k++)
        {
            pthread_join(net_threads[k], NULL);
        }*/
        for (k = 0; k < j; k++)
        {
            pthread_join(net_threads[k], (void *)&pret);
            if (*pret != 0)
            {
                val = 1;
            }
        }

        if (val != 0)
        {
            ret = fail_info();
            return ret;
        }

        windows_info("(SUCCESS)网络测试成功\n");
        return 0;
    }
}

int get_random_str(char *random_str, int random_len)
{
    int i, random_num, seed_str_len;
    struct timeval tv;
    unsigned int seed_num;
    char seed_str[] = "abcdefghijklmnopqrstuvwxyz"
                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; // 随机字符串的随机字符集

    seed_str_len = strlen(seed_str);

    gettimeofday(&tv, NULL);
    seed_num = (unsigned int)(tv.tv_sec + tv.tv_usec); // 超了unsigned int的范围也无所谓，我们要的只是不同的种子数字
    srand(seed_num);

    for (i = 0; i < random_len; i++)
    {
        random_num = rand() % seed_str_len;
        random_str[i] = seed_str[random_num];
    }

    return 0;
}

// 直接计算法计算crc
unsigned short do_crc(unsigned char *ptr, int len)
{
    unsigned int i;
    unsigned short crc = 0xFFFF;

    while (len--)
    {
        crc ^= *ptr++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = (crc >> 1);
        }
    }

    return crc;
}

void init_crc_table(void)
{
    uint32_t crc;
    int i, j;

    for (i = 0; i < 256; i++)
    {
        crc = i;
        for (j = 8; j > 0; j--)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ POLYNOMIAL;
            else
                crc >>= 1;
        }
        crc_table[i] = crc;
    }
}

uint32_t crc32_lookup(char *str)
{
    uint32_t crc = 0xFFFFFFFF;

    while (*str)
    {
        crc = (crc >> 8) ^ crc_table[(crc ^ ((uint8_t)(*str++))) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}

u_int32_t crc_check(u_int32_t source_crc, char *source_buf, int len)
{
    static char check_buf[5000];
    // static char *check_buf;
    u_int32_t crc;

    // check_buf = (char *)malloc(len + 1);
    strncpy(check_buf, source_buf, len);

    check_buf[len + 1] = '\0';
    // printf("chek buf=%s,check len=%d,source len=%d\n", check_buf, strlen(check_buf), strlen(source_buf));
    crc = crc32_lookup(check_buf);
    memset(check_buf, 0, len);
    // free(check_buf);

    return crc;
}

UARTINIT uart_init(void)
{
    char serial_dev_tx[1024];
    char serial_dev_rx[1024];
    int ret, i = 0, j = 0, k = 0;
    char Parity[1024];
    char Baudrate[1024];
    char Databit[1024];
    char Buflen[1024];

    ret = GetIniKeyString("setting", "serial_dev_tx", serial_dev_tx);
    if (ret == 0)
    {
        // printf("读取的数据为:%s\n", serial_dev_tx);
    }
    else
    {
        printf("请指定发送串口\n");
        exit(0);
    }

    ret = GetIniKeyString("setting", "serial_dev_rx", serial_dev_rx);
    if (ret == 0)
    {
        // printf("读取的数据为:%s\n", serial_dev_rx);
    }
    else
    {
        printf("请指定接收串口\n");
        exit(0);
    }

    ret = GetIniKeyString("setting", "parity", Parity);
    if (ret == 0)
    {
        // printf("读取的数据为:%s\n", Parity);
    }
    else
    {
        printf("请指定串口校验位\n");
        exit(0);
    }

    ret = GetIniKeyString("setting", "baudrate", Baudrate);
    if (ret == 0)
    {
        // printf("读取的数据为:%s\n", Baudrate);
    }
    else
    {
        printf("请指定串口波特率\n");
        exit(0);
    }

    ret = GetIniKeyString("setting", "databit", Databit);
    if (ret == 0)
    {
        // printf("读取的数据为:%s\n", Databit);
    }
    else
    {
        printf("请指定串口传输数据位大小\n");
        exit(0);
    }

    ret = GetIniKeyString("setting", "buflen", Buflen);
    if (ret == 0)
    {
        // printf("读取的数据为:%s\n", Buflen);
    }
    else
    {
        printf("请指定串口传输数据长度\n");
        exit(0);
    }

    uart_conf.parity = atoi(Parity);
    uart_conf.baudrate = atoi(Baudrate);
    uart_conf.databit = atoi(Databit);
    uart_conf.buflen = atoi(Buflen);

    for (i = 0; i < strlen(serial_dev_tx); i++)
    {
        if (serial_dev_tx[i] == ',')
        {
            uart_conf.dev_tx[j][k + 1] = '\0';
            j = j + 1;
            k = 0;
        }
        else
        {
            uart_conf.dev_tx[j][k] = serial_dev_tx[i];
            k = k + 1;
        }
    }

    k = 0;
    j = 0;

    for (i = 0; i < strlen(serial_dev_rx); i++)
    {
        if (serial_dev_rx[i] == ',')
        {
            j = j + 1;
            k = 0;
        }
        else
        {
            uart_conf.dev_rx[j][k] = serial_dev_rx[i];
            k = k + 1;
        }
    }

    uart_conf.uart_num = j + 1;
    memset(serial_dev_tx, 0, sizeof(serial_dev_tx));
    memset(serial_dev_rx, 0, sizeof(serial_dev_rx));

    // return uart_conf;
}

int uart_trans(UARTINIT uart_confure)
{
    // char txbuf[1024];
    // static char package_buf[1024];
    // static char data[1024];
    static char txbuf[5000];
    static char package_buf[5000];
    static char data[5000];

    char rxbuf[30];
    char uart_info[100];
    char crc_send[10];
    uint32_t tx_crc, rx_crc;
    int len_sum = 0, tx_len, rx_len = 0;
    int i, j, k, ret;

    init_crc_table();

    for (i = 0; i < uart_confure.uart_num; i++)
    {
        for (j = 0; j < 10; j++)
        {
            get_random_str(data, uart_confure.buflen);

            tx_crc = crc32_lookup(data);
            sprintf(crc_send, "%08x", tx_crc);
            strcpy(txbuf, data);
            strcat(txbuf, crc_send);

            // tx_len = libserial_send_package(uart_confure.tx_fd[i], txbuf, uart_confure.buflen);
            tx_len = libserial_send_package(uart_confure.tx_fd[i], txbuf, strlen(txbuf));

            while (1)
            {
                len_sum = read(uart_confure.rx_fd[i], rxbuf, 30);

                if (len_sum <= 0)
                {
                    // printf("txbuf=%s\n", txbuf);
                    // printf("rxbuf=%s\n", package_buf);
                    len_sum = 0;
                    break;
                }
                else
                {
                    rx_len += len_sum;
                    strcat(package_buf, rxbuf);
                    memset(rxbuf, 0, sizeof(rxbuf));
                }
            }

            rx_crc = crc_check(tx_crc, package_buf, uart_confure.buflen);
            if (tx_crc == rx_crc)
            {
                // printf("txbuf_crc=%8x,rxbuf_crc=%08x,crc check success\n", tx_crc, rx_crc);

                sprintf(uart_info, "txbuf_crc=%8x,rxbuf_crc=%08x,crc check success\n", tx_crc, rx_crc);
                debug_info(uart_info);
                memset(package_buf, 0, sizeof(package_buf));
                memset(crc_send, 0, sizeof(crc_send));
                memset(txbuf, 0, sizeof(txbuf));
                memset(data, 0, sizeof(data));
            }
            else
            {
                sprintf(uart_info, "(FAILED)串口%s与串口%s通讯异常!\n", uart_confure.dev_tx[i], uart_confure.dev_rx[i]);
                windows_info(uart_info);
                /*for (i = 0; i < uart_conf.uart_num; i++)
                {
                    libserial_close(uart_conf.tx_fd[i]);
                    libserial_close(uart_conf.rx_fd[i]);
                }*/
                /*
                                ret = select_option();
                                if (ret == 1)
                                {
                                    if (auto_mode == 1)
                                    {
                                        return 0;
                                    }
                                    else
                                    {
                                        return 1;
                                        // uart_trans(uart_conf);
                                    }
                                }
                                else
                                    exit(0);*/
                memset(package_buf, 0, sizeof(package_buf));
                memset(crc_send, 0, sizeof(crc_send));
                memset(txbuf, 0, sizeof(txbuf));
                memset(data, 0, sizeof(data));
                ret = fail_info();
                return ret;
            }

            get_random_str(data, uart_confure.buflen);

            tx_crc = crc32_lookup(data);
            sprintf(crc_send, "%08x", tx_crc);
            strcpy(txbuf, data);
            strcat(txbuf, crc_send);

            tx_len = libserial_send_package(uart_confure.rx_fd[i], txbuf, strlen(txbuf));
            while (1)
            {
                len_sum = read(uart_confure.tx_fd[i], rxbuf, 30);

                if (len_sum <= 0)
                {
                    // printf("txbuf=%s\n", txbuf);
                    // printf("rxbuf=%s\n", package_buf);
                    len_sum = 0;
                    break;
                }
                else
                {
                    rx_len += len_sum;
                    strcat(package_buf, rxbuf);
                    memset(rxbuf, 0, sizeof(rxbuf));
                }
            }

            rx_crc = crc_check(tx_crc, package_buf, uart_confure.buflen);
            if (tx_crc == rx_crc)
            {
                // printf("txbuf_crc=%8x,rxbuf_crc=%08x,crc check success\n", tx_crc, rx_crc);
                sprintf(uart_info, "txbuf_crc=%8x,rxbuf_crc=%08x,crc check success\n", tx_crc, rx_crc);
                debug_info(uart_info);
                memset(package_buf, 0, sizeof(package_buf));
                memset(crc_send, 0, sizeof(crc_send));
                memset(txbuf, 0, sizeof(txbuf));
                memset(data, 0, sizeof(data));
            }
            else
            {
                sprintf(uart_info, "(FAILED)串口%s与串口%s通讯异常!\n", uart_confure.dev_tx[i], uart_confure.dev_rx[i]);
                windows_info(uart_info);
                /* for (i = 0; i < uart_conf.uart_num; i++)
                 {
                     libserial_close(uart_conf.tx_fd[i]);
                     libserial_close(uart_conf.rx_fd[i]);
                 }*/

                /*  ret = select_option();
                   if (ret == 1)
                   {
                       if (auto_mode == 1)
                       {
                           return 0;
                       }
                       else
                       {
                           return 1;
                           // uart_trans(uart_conf);
                       }
                   }
                   else
                       exit(0);*/

                memset(package_buf, 0, sizeof(package_buf));
                memset(crc_send, 0, sizeof(crc_send));
                memset(txbuf, 0, sizeof(txbuf));
                memset(data, 0, sizeof(data));

                ret = fail_info();
                return ret;
            }
        }
    }
}

int uart_test(void)
{
    // UARTINIT Uart_conf;
    static int i = 0, j = 0, k = 0, ret;
    char uart_mode[3];

    windows_info("--------------串口功能测试开始--------------\n");

    printf("请输入串口模式:");
    scanf("%s", uart_mode);
    ret = set_serial_protocol("-a", uart_mode);
    if (ret == 0)
    {
        return 0;
    }
    else if (ret == -1)
    {
        printf("请输入正确的串口模式\n");
        uart_test();
    }

    // Uart_conf = uart_init();
    for (i = 0; i < uart_conf.uart_num; i++)
    {
        uart_conf.tx_fd[i] = libserial_init(uart_conf.dev_tx[i], uart_conf.baudrate, uart_conf.parity, uart_conf.databit, 2, 0, 0);
        uart_conf.rx_fd[i] = libserial_init(uart_conf.dev_rx[i], uart_conf.baudrate, uart_conf.parity, uart_conf.databit, 2, 0, 0);
        printf("devtx=%s,devrx=%s\n", uart_conf.dev_tx[i], uart_conf.dev_rx[i]);
    };

    ret = uart_trans(uart_conf);
    if (ret == 1)
    {
        return ret;
    }
    else if (ret == 0)
    {
        windows_info("(FAILED)串口功能测试失败\n");
        return 0;
    }

    for (i = 0; i < uart_conf.uart_num; i++)
    {
        libserial_close(uart_conf.tx_fd[i]);
        libserial_close(uart_conf.rx_fd[i]);
    }

    windows_info("(SUCCESS)串口功能测试成功\n");
    return 0;
}

int ssd_test(void)
{
    static FILE *_ssdfp = NULL;
    static char ssd_buf[1024];
    static int ret;
    static char disk_capacity[10];

    windows_info("--------------SSD固态硬盘测试开始--------------\n");

    ret = GetIniKeyString("setting", "disk_capacity", disk_capacity);

    if ((_ssdfp = popen("df -h|grep /dev/nvme", "r")) != NULL)
    {
        while (fgets(ssd_buf, 1024, _ssdfp) != NULL)
        {
            debug_info(ssd_buf);
        }
        pclose(_ssdfp);
        _ssdfp = NULL;
    }
    if (strlen(ssd_buf) == 0)
    {
        debug_info("(FAILED)ssd固态硬盘挂载失败!\n");
        ret = fail_info();
        return ret;
    }
    else
    {
        debug_info("(SUCCESS)ssd固态硬盘挂载成功!\n");
    }

    if ((_ssdfp = popen("df -h |grep nvme|awk -F' ' '{print $4}'", "r")) != NULL)
    {
        fgets(ssd_buf, 1024, _ssdfp);
        pclose(_ssdfp);
        _ssdfp = NULL;
    }
    if (strcmp(ssd_buf, disk_capacity) != 0)
    {
        debug_info("(FAILED)ssd固态硬盘容量检测正常!\n");
        ret = fail_info();
        return ret;
    }
    else
    {
        debug_info("(SUCCESS)ssd固态硬盘容量检测异常!\n");
    }

    if ((_ssdfp = popen("sudo ./fio -filename=/dev/nvme0n1p1 -ioengine=psync -direct=1 -thread=6 -rw=randwrite \
            -iodepth=64 -bs=4k -size=200G -numjobs=30 -runtime=60 -allow_mounted_write=1 -name=fio_test -group_reporting",
                        "r")) != NULL)
    {
        while (fgets(ssd_buf, 1024, _ssdfp) != NULL)
        {
            debug_info(ssd_buf);
        }
        pclose(_ssdfp);
        _ssdfp = NULL;
    }

    if ((_ssdfp = popen("sudo ./fio -filename=/dev/nvme0n1p1 -ioengine=psync -direct=1 -thread=6 -rw=randread \
             -iodepth=64 -bs=4k -size=200G -numjobs=30 -runtime=60 -allow_mounted_write=1 -name=fio_test -group_reporting",
                        "r")) != NULL)
    {
        while (fgets(ssd_buf, 1024, _ssdfp) != NULL)
        {
            debug_info(ssd_buf);
        }
        pclose(_ssdfp);
        _ssdfp = NULL;
    }

    system("sudo umount /dev/nvme0n1p1");

    if ((_ssdfp = popen("echo y|sudo mkfs.ext4 /dev/nvme0n1p1", "r")) != NULL)
    {
        while (fgets(ssd_buf, 1024, _ssdfp) != NULL)
        {
            debug_info(ssd_buf);
        }
        pclose(_ssdfp);
        _ssdfp = NULL;
    }

    windows_info("(SUCCESS)SSD固态硬盘测试成功\n");
    return 0;
}

int rtc_test(void)
{
    static FILE *rtc_fp = NULL;
    static char time_buf[1024];
    static char time_data[6][5];
    char ntpd_addr[20];
    char ntpd_buf[50];
    static int ret;
    int i = 0, j = 0, k = 0;

    struct rtc_time tm;
    struct tm _tm;
    struct timeval tv;
    time_t timep;

    windows_info("--------------RTC功能测试开始!--------------\n");

    // log_init();
    // zlog_debug(zc, "--------------RTC功能测试开始!--------------");

    ret = GetIniKeyString("setting", "time", time_buf);
    if (ret == 0)
    {
        printf("读取到设置的时间=%s\n", time_buf);
    }
    else
        printf("请在配置文件输入需要设置的时间=%s:\n");

    for (i = 0; i < strlen(time_buf); i++)
    {
        if (time_buf[i] == '-' || time_buf[i] == ':')
        {
            j = j + 1;
            k = 0;
        }
        else
        {
            time_data[j][k] = time_buf[i];
            k = k + 1;
        }
    }

    _tm.tm_year = atoi(time_data[0]) - 1900;
    _tm.tm_mon = atoi(time_data[1]) - 1;
    _tm.tm_mday = atoi(time_data[2]);
    _tm.tm_hour = atoi(time_data[3]);
    _tm.tm_min = atoi(time_data[4]);
    _tm.tm_sec = atoi(time_data[5]);

    timep = mktime(&_tm);
    tv.tv_sec = timep;
    tv.tv_usec = 0;
    if (settimeofday(&tv, (struct timezone *)0) != 0)
    {
        windows_info("(FAILED)RTC时间设置失败\n");
        ret = fail_info();
        return ret;
    }

    system("sudo hwclock -w");
    sleep(5);

    if ((rtc_fp = popen("sudo hwclock -r", "r")) != NULL)
    {
        fgets(time_buf, 1024, rtc_fp);
        debug_info(time_buf);
        pclose(rtc_fp);
        rtc_fp = NULL;
    }

    ret = GetIniKeyString("setting", "ntpd_addr", ntpd_addr);
    if (ret == 0)
    {
        printf("读取到NTPD服务器的IP地址=%s\n", ntpd_addr);
    }
    else
        printf("请在配置文件输入需要设置的NTPD服务器地址=%s:\n");

    sprintf(ntpd_buf, "sudo ntpdate -u %s", ntpd_addr);
    ret = system(ntpd_buf);

    if (ret == 0)
    {
        system("sudo hwclock -w");
        windows_info("(SUCCESS)RTC功能测试成功!\n");
        return ret;
    }
    else
    {
        windows_info("(FAILED)RTC功能测试失败!\n");
        ret = fail_info();
        return ret;
    }
}

int usb_test(void)
{
    char usbfile[1024];
    static int ret;

    windows_info("--------------USB功能测试开始--------------\n");

    ret = GetIniKeyString("setting", "usbfile", usbfile);
    if (ret == 0)
    {
        printf("读取的USB文件为:%s\n", usbfile);
    }
    else
    {
        printf("请指定USB测试文件\n");
        exit(0);
    }

    if (access(usbfile, F_OK) != -1)
    {
        printf("文件 %s 存在\n", usbfile);
        windows_info("(SUCCESS)USB功能测试成功!\n");
        return 0;
    }
    else
    {
        printf("文件 %s 不存在\n", usbfile);
        windows_info("(FAILED)USB功能测试失败!\n");
        ret = fail_info();
        return ret;
    }
}

int led_test(void)
{
    static char sel;
    int i2c_fd, ret;
    u_int8_t slave_addr;
    u_int8_t direct_buf[2];
    u_int8_t output_buf_high[2];
    u_int8_t output_buf_low[2];

    windows_info("--------------LED测试开始!--------------\n");

    i2c_fd = open("/dev/i2c-8", O_RDWR);
    if (i2c_fd < 0)
    {
        windows_info("(FAILED)LED测试失败!open /dev/i2c-8 error!\n");
        ret = fail_info();
        return ret;
    }

    output_buf_high[0] = 0xff; // 将所有IO口的输出高电平
    output_buf_high[1] = 0xff;
    output_buf_low[0] = 0x00; // 将所有IO口的输出d低电平
    output_buf_low[1] = 0x00;
    direct_buf[0] = 0x00; // 将所有IO口设置为输出
    direct_buf[1] = 0x00;

    slave_addr = 0x23;
    i2c_write_bytes(i2c_fd, slave_addr, 0x06, direct_buf, 2);
    for (int i = 0; i < 5; i++)
    {
        i2c_write_bytes(i2c_fd, slave_addr, 0x02, output_buf_high, 2); // 0x06 0x07寄存器均写1
        sleep(1);
        i2c_write_bytes(i2c_fd, slave_addr, 0x02, output_buf_low, 2); // 0x06 0x07寄存器均写0
        sleep(1);
    }

    printf("测试过程中LED显示灯是否均正常显示(Y or N):");
    scanf("%s", &sel);
    if (sel == 'Y' || sel == 'y')
    {
        windows_info("(SUCCESS)LED测试成功!\n");
        return 0;
    }
    else
    {
        windows_info("(FAILED)LED测试失败!\n");
        ret = fail_info();
        return ret;
    }
}

int display_test(void)
{
    static char sel;
    static FILE *_displayfp = NULL;
    static char display_buf[1024];
    int ret;

    windows_info("--------------显示测试开始!--------------\n");
    printf("测试过程中显示接口是否均正常显示(Y or N):");
    scanf("%s", &sel);

    if ((_displayfp = popen("xrandr |grep disconnected", "r")) != NULL)
    {
        while (fgets(display_buf, 1024, _displayfp) != NULL)
        {
            debug_info(display_buf);
        }
        pclose(_displayfp);
        _displayfp = NULL;
    }

    if (sel == 'Y' || sel == 'y')
    {
        if (strlen(display_buf) == 0)
        {
            windows_info("(SUCCESS)显示测试成功!\n");
            return 0;
        }
        else
        {
            windows_info("(FAILED)显示测试失败!\n");
            ret = fail_info();
            return ret;
        }
    }
    else
    {
        windows_info("(FAILED)显示测试失败!\n");
        ret = fail_info();
        return ret;
    }
}

void mac_info(void)
{
    static FILE *_macfp = NULL;
    static char mac_buf[1024];
    int ret;

    if ((_macfp = popen("sudo ifconfig |grep ether", "r")) != NULL)
    {
        while (fgets(mac_buf, 1024, _macfp) != NULL)
        {
            debug_info(mac_buf);
        }
        pclose(_macfp);
        _macfp = NULL;
    }

    if (strlen(mac_buf) == 0)
    {
        debug_info("(FAILED)mac地址获取失败!\n");
        ret = select_option();
        if (ret == 1)
        {
            mac_info();
        }
        else
            exit(0);
    }
    else
        debug_info("(SUCCESS)mac地址获取成功!\n");
}

int auto_test(void)
{
    static int ret = 1;

    mac_info();

    ret = rtc_test();
    sleep(5);

    if (ret == 0)
    {
        ret = ssd_test();
        sleep(5);
    }

    if (ret == 0)
    {
        ret = usb_test();
        sleep(5);
    }

    if (ret == 0)
    {
        for (int i = 0; i < 3; i++)
        {
            ret = uart_test();
            sleep(3);
        }
    }

    if (ret == 0)
    {
        ret = network_test();
        sleep(5);
    }

    if (ret == 0)
    {
        ret = led_test();
        sleep(5);
    }

    if (ret == 0)
    {
        ret = display_test();
        return ret;
    }
}
/*
int select_option(void)
{
    static int test_selection;
    printf("请选择是否继续测试?(Y or N):");
    scanf("%s", &test_selection);
    if (test_selection == 'Y')
    {
        return 1;
    }
    else
        exit(0);
}*/

int test_select()
{
    static int mode, ret;

    printf("请选择要进行测试的选项\n");
    printf("1:全自动测试\n");
    printf("2:网络测试\n");
    printf("3:串口测试\n");
    printf("4:SSD测试\n");
    printf("5:RTC测试\n");
    printf("6:USB测试\n");
    printf("7:LED测试\n");
    printf("8:显示测试\n");
    printf("请输出测试选项:");

    scanf("%d", &mode);

    switch (mode)
    {
    case 1:
        auto_mode = 1;
        ret = auto_test();
        if (ret == 0)
        {
            windows_info("(SUCCESS)自动测试成功\n");
            break;
        }

        // windows_info("(SUCCESS)自动测试成功\n");
        // break;

    case 2:
        network_test();
        ret = select_option();
        if (ret == 1)
        {
            test_select();
        }
        else
            break;

    case 3:
        uart_test();
        ret = select_option();
        if (ret == 1)
        {
            test_select();
        }
        else
            break;

    case 4:
        ssd_test();
        ret = select_option();
        if (ret == 1)
        {
            test_select();
        }
        else
            break;

    case 5:
        rtc_test();
        ret = select_option();
        if (ret == 1)
        {
            test_select();
        }
        else
            break;

    case 6:
        usb_test();
        ret = select_option();
        if (ret == 1)
        {
            test_select();
        }
        else
            break;
    case 7:
        led_test();
        ret = select_option();
        if (ret == 1)
        {
            test_select();
        }
        else
            break;

    case 8:
        display_test();
        ret = select_option();
        if (ret == 1)
        {
            test_select();
        }
        else
            break;

    default:
        break;
    }
    return 0;
}

void usage(void)
{
    printf("默认不加参数,自动化结果将不会同步输出至控制台\n");
    printf("--debug |开启debug模式,自动化测试结果将同步输出至控制台\n");
    printf("-h |显示应用参数含义\n");
}

int main(int argc, char *argv[])
{
    logfile_init();

    uart_init();
    if (argc == 1)
    {
        test_select();
    }
    else
    {
        if (strcmp(argv[1], "--debug") == 0)
        {
            debug_mode = 1;
            test_select();
        }
        else if (strcmp(argv[1], "-h") == 0)
        {
            usage();
            return 0;
        }
        else
        {
            printf("请输入正确的命令\n");
            return 0;
        }
    }
    return 0;
}