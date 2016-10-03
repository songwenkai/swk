#include <stc12c5a60s2.h>
#include <intrins.h>
#include "iic.h"
#include "uart.h"
#include"iic.h"
#include"delay.h"
#include"uart.h"


/*******************************************************************
起动总线函数               

函数原型: void   Start_I2c();  
功能:      启动I2C总线,即发送I2C起始条件.  
********************************************************************/
void IIC_start()
{
   IIC_dat=1;     /*发送起始条件的数据信号*/
   delay_us();
   IIC_clk=1;	   /*起始条件建立时间大于4.7us,延时*/
   delay_us();
   delay_us();
   delay_us();
   delay_us();
   delay_us();      
    
   IIC_dat=0;     /*发送起始信号*/
   delay_us();	  /* 起始条件锁定时间大于4μs*/
   delay_us();
   delay_us();
   delay_us();
   delay_us();      
        
   IIC_clk=0;     /*钳住I2C总线，准备发送或接收数据 */
   delay_us();
   delay_us();
}

/*******************************************************************
结束总线函数               
函数原型: void   Stop_I2c();  
功能:      结束I2C总线,即发送I2C结束条件.  
********************************************************************/
void IIC_stop()
{
    IIC_dat=0;           /*发送结束条件的数据信号*/
    delay_us();     /*发送结束条件的时钟信号*/
    IIC_clk=1;           /*结束条件建立时间大于4us*/ 
   delay_us();
   delay_us();
   delay_us();
   delay_us();
   delay_us();

    IIC_dat=1;           /*发送I2C总线结束信号*/
      delay_us();
   delay_us();
   delay_us();
   delay_us();

    // jxy add : 2010-01-17
    // 在有些IIC模拟中，发现有用到下面2句
    //SCL=0;           /*结束条件建立时间大于4us*/ 
    //Delay1us(2);
}

/*******************************************************************
字节数据发送函数
函数原型: void   SendByte(uchar c);
功能:      将数据c发送出去,可以是地址,也可以是数据,发完后等待应答,并对
           此状态位进行操作.(不应答或非应答都使ack=0)     
           发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
********************************************************************/
bit IIC_SendByte(u8 c)
{
u8 xdata BitCnt=0,i=0;
bit ack;

    for(BitCnt=0;BitCnt<8;BitCnt++)   /*要传送的数据长度为8位*/
     {
      if((c<<BitCnt)&0x80)IIC_dat=1;    /*判断发送位*/
        else   IIC_dat=0;                
      delay_us();
      IIC_clk=1;                /*置时钟线为高，通知被控器开始接收数据位*/
      
      delay_us();              /*保证时钟高电平周期大于4μs*/
      delay_us();
	  delay_us();
	  delay_us();
	  delay_us();
	         
      IIC_clk=0; 

     }
    
     delay_us();
	 delay_us();
     IIC_dat=1;                 /*8位发送完后释放数据线，准备接收应答位*/
     delay_us();
	 delay_us();   
     IIC_clk=1;
     delay_us();
	 delay_us();
     while((IIC_dat==1)&&(i<250))i++;
	 if(i>=248)ack=0;
	 else ack=1;         /*判断是否接收到应答信号*/
	 i=0;	 
     IIC_clk=0;
     delay_us();
	 delay_us();
	 return ack;
}

/*******************************************************************
                  字节数据接收函数               
函数原型: uchar   RcvByte();
功能:      用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
           发完后请用应答函数应答从机。  
********************************************************************/ 
u8 IIC_readByte()
{
   u8 retc;
   u8 xdata BitCnt=0;
  
   retc=0; 
   IIC_dat=1;                /*置数据线为输入方式*/
   for(BitCnt=0;BitCnt<8;BitCnt++)
       {
         delay_us();   
         IIC_clk=0;                   /*置时钟线为低，准备接收数据位*/
        
         delay_us();                  /*时钟低电平周期大于4.7μs*/
		 delay_us();
		 delay_us();
		 delay_us();
		 delay_us();
       
         IIC_clk=1;                   /*置时钟线为高使数据线上数据有效*/
         delay_us();
		 delay_us();
		 delay_us();
         retc=retc<<1;
         if(IIC_dat==1)retc=retc+1;   /*读数据位,接收的数据位放入retc中 */
         delay_us();
		 delay_us(); 
       }
   	IIC_clk=0;    
  	 delay_us();
	delay_us();
   return(retc);
}

/********************************************************************
应答子函数
函数原型:   void Ack_I2c(bit a);
功能:       主控器进行应答信号(可以是应答或非应答信号，由位参数a决定)
				0为应答。
********************************************************************/
void IIC_Ack(bit a)
{
  
   if(a==0)IIC_dat=0;            /*在此发出应答或非应答信号 */
         else IIC_dat=1;
   delay_us();
   delay_us();
   delay_us();      
   IIC_clk=1;
  
   delay_us();                     /*时钟低电平周期大于4μs*/
   delay_us();
   delay_us();
   delay_us();
   delay_us();
    
   IIC_clk=0;                      /*清时钟线，钳住I2C总线以便继续接收*/
   delay_us();
   delay_us();   
}


/*******************************************************************
向有子地址器件发送多字节数据函数
函数原型: bit   ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
功能:      从启动总线到发送地址，子地址,数据，结束总线的全过程,从器件
           地址sla，子地址suba，发送内容是s指向的内容，发送no个字节。
           如果返回1表示操作成功，否则操作有误。
注意：     使用前必须已结束总线。
输入: 器件地址，器件子地址，数据，数据个数
********************************************************************/
unsigned char IIC_WriteData(unsigned char byAD, unsigned char byRA,
                     unsigned char * pData, unsigned char byCount)
{
    u8 xdata i=0;
	bit ack=1;   
    IIC_start();                  /*启动总线*/
    ack=IIC_SendByte(byAD);              /*发送器件地址*/
    if(ack==0)
    {	
			UART1_SendString("木有应答！");
        return(0);
    }
    ack=IIC_SendByte(byRA);             /*发送器件子地址*/
    if(ack==0)
    {	
        return(0);
     }

    for(i = 0; i < byCount; i++)
    {   
      ack=IIC_SendByte(*pData);             /*发送数据*/
      if(ack==0)
      {	 
          return(0);  
       }
      pData++;
    } 
    IIC_stop();                 /*结束总线*/ 

    return(1);
}

/*******************************************************************
向有子地址器件读取多字节数据函数               
函数原型: bit   ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
功能:      从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件
           地址sla，子地址suba，读出的内容放入s指向的存储区，读no个字节。
            如果返回1表示操作成功，否则操作有误。
注意：     使用前必须已结束总线。
输入: 器件地址，器件子地址，数据，数据个数
********************************************************************/
unsigned char IIC_ReadData(unsigned char byAD, unsigned char * pData, 
                                unsigned char byCount)
{
    u8 xdata i;
	bit ack;

   IIC_start();                  /*启动总线*/

    ack=IIC_SendByte(byAD);                 /*发送器件地址*/
    

    if(ack==0)
    {
        return(0);
    }

    for(i = 0; i < (byCount-1); i++)
    {
        *pData=IIC_readByte();                /*接收数据*/
        IIC_Ack(0);                      /*发送应答位*/  
        pData++;
    }

    *pData=IIC_readByte();
    IIC_Ack(1);                    /*发送非应答*/
    IIC_stop();                    /*结束总线*/ 
    return(1);
}
