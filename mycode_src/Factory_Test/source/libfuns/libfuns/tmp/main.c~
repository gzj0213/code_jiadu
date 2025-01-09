/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年06月02日 17时55分15秒 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian@revenco.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */
#include "../libttys.h"
#include "../libiso14443a.h"

int main(void)
{
    char i;
    char err = -1;
    char snr[5] = {0};
    char atq_ask[2];
    char keys[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
    char block_data[17];

    giserial = serialport_init(USB1, 115200, 8, 'N', 1, 0, 0, 0, 10);
    if (giserial == -1){
        printf("serialport_init fail!\n");
        goto error;
    }
    printf("serialport_init succ!\n");

#if 1
    err = ISO14443A_Init();
    if (err < 0){
        printf("ISO14443A_Init fail %d\n", err);
        goto error;
    }
    printf("ISO14443A_Init succ %d\n", err);

    err = ISO14443A_Request(0, atq_ask);
    if (err < 0){
        printf("ISO14443A_Request fail %d\n", err);
        goto error;
    }
    printf("ISO14443A_Request succ %d\n", err);

    bzero(snr, sizeof(snr));
    err = ISO14443A_Anticoll(0, 0, snr);
    if (err < 0){
        printf("ISO14443A_Anticoll fail %d\n", err);
        goto error;
    }
    printf("snr:0x%02X%02X%02X%02X\n", snr[0], snr[1], snr[2], snr[3]);
    printf("ISO14443A_Anticoll succ %d\n", err);

    err = ISO14443A_Select(0, snr, atq_ask);
    if (err < 0){
        printf("ISO14443A_Select fail %d\n", err);
        goto error;
    }
    printf("ISO14443A_Select succ %d\n", err);

#endif

    err = ISO14443A_Authentication(0x60, snr, keys, 1);
    if (err < 0){
        printf("ISO14443A_Authentication fail %d\n", err);
        goto error;
    }
    printf("ISO14443A_Authentication succ %d\n", err);

    bzero(block_data, sizeof(block_data));
    err = ISO14443A_ReadBlock(1, block_data);
    if (err < 0){
        printf("ISO14443A_ReadBlock fail %d\n", err);
        goto error;
    }
    for (i=0; i<16; i++){
        printf("%02X", block_data[i]);
    }
    printf("\nISO14443A_ReadBlock succ %d\n", err);

    err = ISO14443A_WriteBlock(1, block_data);
    if (err < 0){
        printf("ISO14443A_WriteBlock fail %d\n", err);
        goto error;
    }
    printf("ISO14443A_WriteBlock succ %d\n", err);

    err = ISO14443A_Halt();
    if (err < 0){
        printf("ISO14443A_Halt fail %d\n", err);
        goto error;
    }
    printf("ISO14443A_Halt succ %d\n", err);



    printf("operation success!!\n");
error:
    serialport_close(giserial);
    return(0);
}
