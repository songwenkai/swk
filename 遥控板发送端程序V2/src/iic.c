#include <stc12c5a60s2.h>
#include <intrins.h>
#include "iic.h"
#include "uart.h"
#include"iic.h"
#include"delay.h"
#include"uart.h"


/*******************************************************************
�����ߺ���               

����ԭ��: void   Start_I2c();  
����:      ����I2C����,������I2C��ʼ����.  
********************************************************************/
void IIC_start()
{
   IIC_dat=1;     /*������ʼ�����������ź�*/
   delay_us();
   IIC_clk=1;	   /*��ʼ��������ʱ�����4.7us,��ʱ*/
   delay_us();
   delay_us();
   delay_us();
   delay_us();
   delay_us();      
    
   IIC_dat=0;     /*������ʼ�ź�*/
   delay_us();	  /* ��ʼ��������ʱ�����4��s*/
   delay_us();
   delay_us();
   delay_us();
   delay_us();      
        
   IIC_clk=0;     /*ǯסI2C���ߣ�׼�����ͻ�������� */
   delay_us();
   delay_us();
}

/*******************************************************************
�������ߺ���               
����ԭ��: void   Stop_I2c();  
����:      ����I2C����,������I2C��������.  
********************************************************************/
void IIC_stop()
{
    IIC_dat=0;           /*���ͽ��������������ź�*/
    delay_us();     /*���ͽ���������ʱ���ź�*/
    IIC_clk=1;           /*������������ʱ�����4us*/ 
   delay_us();
   delay_us();
   delay_us();
   delay_us();
   delay_us();

    IIC_dat=1;           /*����I2C���߽����ź�*/
      delay_us();
   delay_us();
   delay_us();
   delay_us();

    // jxy add : 2010-01-17
    // ����ЩIICģ���У��������õ�����2��
    //SCL=0;           /*������������ʱ�����4us*/ 
    //Delay1us(2);
}

/*******************************************************************
�ֽ����ݷ��ͺ���
����ԭ��: void   SendByte(uchar c);
����:      ������c���ͳ�ȥ,�����ǵ�ַ,Ҳ����������,�����ȴ�Ӧ��,����
           ��״̬λ���в���.(��Ӧ����Ӧ��ʹack=0)     
           ��������������ack=1; ack=0��ʾ��������Ӧ����𻵡�
********************************************************************/
bit IIC_SendByte(u8 c)
{
u8 xdata BitCnt=0,i=0;
bit ack;

    for(BitCnt=0;BitCnt<8;BitCnt++)   /*Ҫ���͵����ݳ���Ϊ8λ*/
     {
      if((c<<BitCnt)&0x80)IIC_dat=1;    /*�жϷ���λ*/
        else   IIC_dat=0;                
      delay_us();
      IIC_clk=1;                /*��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ*/
      
      delay_us();              /*��֤ʱ�Ӹߵ�ƽ���ڴ���4��s*/
      delay_us();
	  delay_us();
	  delay_us();
	  delay_us();
	         
      IIC_clk=0; 

     }
    
     delay_us();
	 delay_us();
     IIC_dat=1;                 /*8λ��������ͷ������ߣ�׼������Ӧ��λ*/
     delay_us();
	 delay_us();   
     IIC_clk=1;
     delay_us();
	 delay_us();
     while((IIC_dat==1)&&(i<250))i++;
	 if(i>=248)ack=0;
	 else ack=1;         /*�ж��Ƿ���յ�Ӧ���ź�*/
	 i=0;	 
     IIC_clk=0;
     delay_us();
	 delay_us();
	 return ack;
}

/*******************************************************************
                  �ֽ����ݽ��պ���               
����ԭ��: uchar   RcvByte();
����:      �������մ���������������,���ж����ߴ���(����Ӧ���ź�)��
           ���������Ӧ����Ӧ��ӻ���  
********************************************************************/ 
u8 IIC_readByte()
{
   u8 retc;
   u8 xdata BitCnt=0;
  
   retc=0; 
   IIC_dat=1;                /*��������Ϊ���뷽ʽ*/
   for(BitCnt=0;BitCnt<8;BitCnt++)
       {
         delay_us();   
         IIC_clk=0;                   /*��ʱ����Ϊ�ͣ�׼����������λ*/
        
         delay_us();                  /*ʱ�ӵ͵�ƽ���ڴ���4.7��s*/
		 delay_us();
		 delay_us();
		 delay_us();
		 delay_us();
       
         IIC_clk=1;                   /*��ʱ����Ϊ��ʹ��������������Ч*/
         delay_us();
		 delay_us();
		 delay_us();
         retc=retc<<1;
         if(IIC_dat==1)retc=retc+1;   /*������λ,���յ�����λ����retc�� */
         delay_us();
		 delay_us(); 
       }
   	IIC_clk=0;    
  	 delay_us();
	delay_us();
   return(retc);
}

/********************************************************************
Ӧ���Ӻ���
����ԭ��:   void Ack_I2c(bit a);
����:       ����������Ӧ���ź�(������Ӧ����Ӧ���źţ���λ����a����)
				0ΪӦ��
********************************************************************/
void IIC_Ack(bit a)
{
  
   if(a==0)IIC_dat=0;            /*�ڴ˷���Ӧ����Ӧ���ź� */
         else IIC_dat=1;
   delay_us();
   delay_us();
   delay_us();      
   IIC_clk=1;
  
   delay_us();                     /*ʱ�ӵ͵�ƽ���ڴ���4��s*/
   delay_us();
   delay_us();
   delay_us();
   delay_us();
    
   IIC_clk=0;                      /*��ʱ���ߣ�ǯסI2C�����Ա��������*/
   delay_us();
   delay_us();   
}


/*******************************************************************
�����ӵ�ַ�������Ͷ��ֽ����ݺ���
����ԭ��: bit   ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
����:      ���������ߵ����͵�ַ���ӵ�ַ,���ݣ��������ߵ�ȫ����,������
           ��ַsla���ӵ�ַsuba������������sָ������ݣ�����no���ֽڡ�
           �������1��ʾ�����ɹ��������������
ע�⣺     ʹ��ǰ�����ѽ������ߡ�
����: ������ַ�������ӵ�ַ�����ݣ����ݸ���
********************************************************************/
unsigned char IIC_WriteData(unsigned char byAD, unsigned char byRA,
                     unsigned char * pData, unsigned char byCount)
{
    u8 xdata i=0;
	bit ack=1;   
    IIC_start();                  /*��������*/
    ack=IIC_SendByte(byAD);              /*����������ַ*/
    if(ack==0)
    {	
			UART1_SendString("ľ��Ӧ��");
        return(0);
    }
    ack=IIC_SendByte(byRA);             /*���������ӵ�ַ*/
    if(ack==0)
    {	
        return(0);
     }

    for(i = 0; i < byCount; i++)
    {   
      ack=IIC_SendByte(*pData);             /*��������*/
      if(ack==0)
      {	 
          return(0);  
       }
      pData++;
    } 
    IIC_stop();                 /*��������*/ 

    return(1);
}

/*******************************************************************
�����ӵ�ַ������ȡ���ֽ����ݺ���               
����ԭ��: bit   ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
����:      ���������ߵ����͵�ַ���ӵ�ַ,�����ݣ��������ߵ�ȫ����,������
           ��ַsla���ӵ�ַsuba�����������ݷ���sָ��Ĵ洢������no���ֽڡ�
            �������1��ʾ�����ɹ��������������
ע�⣺     ʹ��ǰ�����ѽ������ߡ�
����: ������ַ�������ӵ�ַ�����ݣ����ݸ���
********************************************************************/
unsigned char IIC_ReadData(unsigned char byAD, unsigned char * pData, 
                                unsigned char byCount)
{
    u8 xdata i;
	bit ack;

   IIC_start();                  /*��������*/

    ack=IIC_SendByte(byAD);                 /*����������ַ*/
    

    if(ack==0)
    {
        return(0);
    }

    for(i = 0; i < (byCount-1); i++)
    {
        *pData=IIC_readByte();                /*��������*/
        IIC_Ack(0);                      /*����Ӧ��λ*/  
        pData++;
    }

    *pData=IIC_readByte();
    IIC_Ack(1);                    /*���ͷ�Ӧ��*/
    IIC_stop();                    /*��������*/ 
    return(1);
}
