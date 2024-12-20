/*
使用inifile库的例子
假设要创建一个new.ini，内容如下：
; new.ini
[section1]
entry1 = abc
entry2 = 123 0X12AC

然后把entry2改为256 300,再用另外的方法创建：
[section2]
entry1 = test
*/

#include "inifile.h"

int main()
{
    PCONFIG pCfg;
    cfg_init (&pCfg, "new.ini", 1);	/* 若new.ini不存在，创建new.ini */
    cfg_write (pCfg, "section1", "entry1", "abc");	/* 写section1:entry1 */
    cfg_write_item (pCfg, "section1", "entry2", "%d 0X%0X", 123,0X12AC);  /* 用另外的方法写section1:entry2 */
    cfg_commit(pCfg);	/* 存盘 */
    cfg_done(pCfg);
    WritePrivateProfileString("section1", "entry2", "256 300","new.ini");
    WritePrivateProfileString("section2", "entry1", "test","new.ini");
}

