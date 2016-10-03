/**********************************
ADC_CONTR寄存器
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
函数名：10位A/D转换函数
调  用：read_adc(unsigned char port_num);
参  数：port_num  取值范围0-7，表示选择的通道
返回值：adc值（10位ADC数据高8位存放在ADC_RES中，低2位存放在ADC_RESL中）
结  果：读出指定ADC接口的A/D转换值，并返回数值
备  注：适用于STC12C5A60S2系列单片机（必须使用STC12C5A60S2.h头文件）
/**********************************************************************************************/
unsigned int  read_adc(unsigned char port_num)
{//port范围0~7
	unsigned int Ad_value=0;

	P1M0|=1<<port_num; //用到那个端口就把哪个端口设置为开漏,这里测试P10       
    P1M1|=1<<port_num;	

	ADC_CONTR = 0xE0;//启动AD电源，并且选择最高速转换
	ADC_CONTR |=port_num;			 
	//必要的延时
	//_nop_();_nop_();_nop_();_nop_();_nop_();
	//_nop_();_nop_();_nop_();_nop_();_nop_();
	delay(100);
	ADC_RES=0;     //高八位清零
	ADC_RESL=0;    //低两位清零
	ADC_CONTR|=0x08; //启动AD转换
	//必要的延时
	_nop_();_nop_();_nop_();_nop_();_nop_();
	while(!(ADC_CONTR&0x10)); 
	ADC_CONTR &= 0xE7; //1110 0111 清ADC_FLAG位, 关闭A/D转换
	Ad_value=ADC_RES*4 + ADC_RESL; //10 wei
	ADC_CONTR=0;
	return Ad_value;
}