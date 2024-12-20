/*
ʹ��inifile�������
����Ҫ����һ��new.ini���������£�
; new.ini
[section1]
entry1 = abc
entry2 = 123 0X12AC

Ȼ���entry2��Ϊ256 300,��������ķ���������
[section2]
entry1 = test
*/

#include "inifile.h"

int main()
{
    PCONFIG pCfg;
    cfg_init (&pCfg, "new.ini", 1);	/* ��new.ini�����ڣ�����new.ini */
    cfg_write (pCfg, "section1", "entry1", "abc");	/* дsection1:entry1 */
    cfg_write_item (pCfg, "section1", "entry2", "%d 0X%0X", 123,0X12AC);  /* ������ķ���дsection1:entry2 */
    cfg_commit(pCfg);	/* ���� */
    cfg_done(pCfg);
    WritePrivateProfileString("section1", "entry2", "256 300","new.ini");
    WritePrivateProfileString("section2", "entry1", "test","new.ini");
}

