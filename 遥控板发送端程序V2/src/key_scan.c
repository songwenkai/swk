#include "key_scan.h"
#include "io_cfg.h"
#include "type.h"
#include "delay.h"

#define KEY_DELAY 100
/******************************************************************
 - ������������ȡ����ֵ��������key_value��
 - ����ģ�飺��������ģ��
 - �������ԣ��ⲿ���û��ɵ���
 - ����˵������          
 - ����˵�������ذ���ֵkey_value
 ******************************************************************/
u8 key_scan_left(void)
{
  unsigned char key_value=0;
	if(key1==0 || key2==0||key3==0 || key4==0)
	{
		delay_ms(20);
		if(key1==0)
			key_value = 1;
		if(key2==0)
			key_value = 2;
		if(key3==0)
			key_value = 3;
		if(key4==0)
			key_value = 4;

		//while(key1==0 || key2==0||key3==0 || key4==0||K1==0||K2==0||K3==0||K4==0);	
	}
	return key_value;
}  
 /******************************************************************
 - ������������ȡ����ֵ��������key_value��
 - ����ģ�飺��������ģ��
 - �������ԣ��ⲿ���û��ɵ���
 - ����˵������          
 - ����˵�������ذ���ֵkey_value
 ******************************************************************/
u8 key_scan_right(void)
{
  unsigned char key_value=0;
	if(K1==0||K2==0||K3==0||K4==0)
	{
		delay_ms(20);
		if(K1==0)
			key_value = 1;
		if(K2==0)
			key_value = 2;
		if(K3==0)
			key_value = 3;
		if(K4==0)
			key_value = 4;
	}
	return key_value;
}  