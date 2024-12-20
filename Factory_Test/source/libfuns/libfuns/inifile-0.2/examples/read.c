/*
ʹ��inifile�������
���赱ǰĿ¼����һ�ļ�test.ini���������£�
; test.ini
[section1]
entry1 = test1
entry2 = 123

[section2]
entry1 = 10.32abc
entry2 = adfasd dsf 100

����Ĵ�����ʾ����ζ�ȡ���ļ�,��û���ж��κδ���
*/

#include <stdio.h>
#include "inifile.h"

/* ��ȡsection1�µ�entry1����ӡ */
void print1()
{
    char buf[80];
    GetPrivateProfileString("section1","entry1","default value",buf,80,"test.ini");
    printf("section1:entry1 -- %s\n",buf);
}

/* ��ȡsection2:entry2 ����ӡ */
void print2()
{
    char buf1[80],buf2[80];
    int i;
    PCONFIG pCfg;
    
    cfg_init (&pCfg, "test.ini", 0);
    cfg_get_item(pCfg, "section2", "entry2", "%s%s%d",buf1,buf2,&i);
    /* ע�⣺�����%s��ʽ�����ַ���ĩβһ��Ҫ��һ���ո���������Ա�ʶ�����ķ��ţ��ο��й�scanf��ʽ�����й�˵���� */
    cfg_done(pCfg);
    printf("section2:entry2 -- %s %s %d\n",buf1,buf2,i);
}

/* ��ȡ�������ò���ӡ */
void print3()
{
    char buf[128];
    PCONFIG pCfg;
    
    cfg_init (&pCfg, "test.ini", 0);
    
    cfg_getstring(pCfg, "section1", "entry1", buf);
    printf("section1:entry1 -- %s\n",buf);
    cfg_getstring(pCfg, "section1", "entry2", buf);
    printf("section1:entry2 -- %s\n",buf);
    cfg_getstring(pCfg, "section2", "entry1", buf);
    printf("section2:entry1 -- %s\n",buf);
    cfg_getstring(pCfg, "section2", "entry2", buf);
    printf("section2:entry2 -- %s\n",buf);
    
    cfg_done(pCfg);
}

int main()
{
    print1();
    print2();
    print3();
}