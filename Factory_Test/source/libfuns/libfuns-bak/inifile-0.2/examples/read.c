/*
使用inifile库的例子
假设当前目录下有一文件test.ini，内容如下：
; test.ini
[section1]
entry1 = test1
entry2 = 123

[section2]
entry1 = 10.32abc
entry2 = adfasd dsf 100

下面的代码演示了如何读取该文件,但没有判断任何错误。
*/

#include <stdio.h>
#include "inifile.h"

/* 读取section1下的entry1并打印 */
void print1()
{
    char buf[80];
    GetPrivateProfileString("section1","entry1","default value",buf,80,"test.ini");
    printf("section1:entry1 -- %s\n",buf);
}

/* 读取section2:entry2 并打印 */
void print2()
{
    char buf1[80],buf2[80];
    int i;
    PCONFIG pCfg;
    
    cfg_init (&pCfg, "test.ini", 0);
    cfg_get_item(pCfg, "section2", "entry2", "%s%s%d",buf1,buf2,&i);
    /* 注意：如果用%s格式符，字符串末尾一定要有一个空格或其它可以标识结束的符号，参看有关scanf格式符的有关说明。 */
    cfg_done(pCfg);
    printf("section2:entry2 -- %s %s %d\n",buf1,buf2,i);
}

/* 读取所有配置并打印 */
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