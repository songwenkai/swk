#ifndef __IIC_H__
#define __IIC_H__

#include<stc12c5a60s2.h>
#include "type.h"


sbit IIC_dat=P1^2;              /*模拟I2C数据传送位*/
sbit IIC_clk=P1^3;              /*模拟I2C时钟控制位*/

void IIC_start(); //启动IIC（使用前要启动，main里并不需要初始化）
void IIC_stop();  //停止IIC
bit IIC_SendByte(u8 c);	//发送一个字节
u8 IIC_readByte();	   //读取一个字节
void IIC_Ack(bit a);   //发送应答（0为应答）
u8 IIC_WriteData(u8 byAD, u8 byRA,u8 * pData, u8 byCount);//发送一组数据
u8 IIC_ReadData(u8 byAD, u8 * pData, u8 byCount); //读取一组数据


#endif