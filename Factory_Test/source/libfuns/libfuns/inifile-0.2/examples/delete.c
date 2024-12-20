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

现在删掉section1:entry1,然后删掉整个section2
*/

#include "inifile.h"

int main()
{
    PCONFIG pCfg;
    cfg_init (&pCfg, "test.ini", 0);
    cfg_write (pCfg, "section1", "entry1", NULL);  /* 删掉section1:entry1 */
    cfg_write (pCfg, "section2", NULL, NULL);	/* 删掉section2 */
    cfg_commit (pCfg);	/* 存盘 */
    cfg_done (pCfg);
}