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

����ɾ��section1:entry1,Ȼ��ɾ������section2
*/

#include "inifile.h"

int main()
{
    PCONFIG pCfg;
    cfg_init (&pCfg, "test.ini", 0);
    cfg_write (pCfg, "section1", "entry1", NULL);  /* ɾ��section1:entry1 */
    cfg_write (pCfg, "section2", NULL, NULL);	/* ɾ��section2 */
    cfg_commit (pCfg);	/* ���� */
    cfg_done (pCfg);
}