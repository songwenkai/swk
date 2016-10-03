#ifndef __IO_CFG_H__
#define __IO_CFG_H__

#include "stc12c5a60s2.h"
#include "sconfig.h"

/* 蜂鸣器定义 */
sbit beep=P4^7;

//控制板led
sbit led1 = P3^4;
sbit led2 = P3^5;
sbit led3 = P3^6;
sbit led4 = P3^7;

//NRF接口
sbit  IRQ  = P0^5;sbit  MISO = P0^4;
sbit  MOSI = P0^3;sbit  SCK  = P0^2;
sbit  CSN  = P0^1;sbit  CE   = P0^0;

//按键定义

sbit key1 = P3^0;     //启用按键控制模式
sbit key2 = P3^1;	  //关闭按键控制模式
sbit key3 = P3^2;	  //小车右转
sbit key4 = P4^3;     //小车左转

sbit K1 =  P4 ^ 1;  //开启陀螺仪重力感应控制
sbit K2 =  P4 ^ 4;  //小车后退
sbit K3 =  P4 ^ 5;  //关闭陀螺仪重力感应控制
sbit K4 =  P4 ^ 6;  //小车前进

/************************/




/************************/
	   
#define led_nrf led1



#endif
