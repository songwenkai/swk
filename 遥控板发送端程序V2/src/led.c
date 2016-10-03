#include "led.h"
#include "io_cfg.h"
#include "delay.h"
		  
void led_init(void){
	led_off_all();
}
 /******************************************************************
 - ��������:�رջ���� ָ��LED����
 - ����ģ�飺��������ģ��
 - �������ԣ��ⲿ���û��ɵ���
 - ����˵����ledx  ��ʾָ����LED  state ��ʾled״ֵ̬  0 ���� 1         
 - ����˵������
 ******************************************************************/
void led_state(u8 ledx, u8 state) {
	switch(ledx) {
		case 0:led1 = state;break;
		case 1:led2 = state;break;
		case 2:led3 = state;break;
		case 3:led4 = state;break;
		default:break;
	}
}
 /******************************************************************
 - ��������:��������4��led������Ϩ��
 - ����ģ�飺��������ģ��
 - �������ԣ��ⲿ���û��ɵ���
 - ����˵������      
 - ����˵������
 ******************************************************************/
void led_test(void) {
	char i = 0;
	for(i = 0;i < 4; i++) {
		led_state(i, LED_ON);
		mdelay(1000);
	}
	
	for(i = 3;i > -1; i--) {
		led_state(i, LED_OFF);
		mdelay(1000);
	}
	return;
}



