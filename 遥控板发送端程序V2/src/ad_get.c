/**********************************
ADC_CONTR�Ĵ���
|ADC_POWER|SPEED1|SPEED0|ADC_FLAG|ADC_START|CHS2|CHS1|CHS0|
ADC_POWER 1:ON 0:OFF

CHS2	CHS1	CHS0
000 1.0
001 1.1
010 1.2
011 1.3
100 1.4
101 1.5
110 1.6
111 1.7
**********************************/
#include <intrins.h>
#include "stc12c5a60s2.h"
#include "ad_get.h"
#include "delay.h"
/**********************************************************************************************/
/*********************************************************************************************
��������10λA/Dת������
��  �ã�read_adc(unsigned char port_num);
��  ����port_num  ȡֵ��Χ0-7����ʾѡ���ͨ��
����ֵ��adcֵ��10λADC���ݸ�8λ�����ADC_RES�У���2λ�����ADC_RESL�У�
��  ��������ָ��ADC�ӿڵ�A/Dת��ֵ����������ֵ
��  ע��������STC12C5A60S2ϵ�е�Ƭ��������ʹ��STC12C5A60S2.hͷ�ļ���
/**********************************************************************************************/
unsigned int  read_adc(unsigned char port_num)
{//port��Χ0~7
	unsigned int Ad_value=0;

	P1M0|=1<<port_num; //�õ��Ǹ��˿ھͰ��ĸ��˿�����Ϊ��©,�������P10       
    P1M1|=1<<port_num;	

	ADC_CONTR = 0xE0;//����AD��Դ������ѡ�������ת��
	ADC_CONTR |=port_num;			 
	//��Ҫ����ʱ
	//_nop_();_nop_();_nop_();_nop_();_nop_();
	//_nop_();_nop_();_nop_();_nop_();_nop_();
	delay(100);
	ADC_RES=0;     //�߰�λ����
	ADC_RESL=0;    //����λ����
	ADC_CONTR|=0x08; //����ADת��
	//��Ҫ����ʱ
	_nop_();_nop_();_nop_();_nop_();_nop_();
	while(!(ADC_CONTR&0x10)); 
	ADC_CONTR &= 0xE7; //1110 0111 ��ADC_FLAGλ, �ر�A/Dת��
	Ad_value=ADC_RES*4 + ADC_RESL; //10 wei
	ADC_CONTR=0;
	return Ad_value;
}