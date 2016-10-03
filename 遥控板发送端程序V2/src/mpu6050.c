#include <stc12c5a60s2.h> 
#include "delay.h"
#include "iic.h"
#include "mpu6050.h"
#include "math.h"
#include "uart.h"

#ifdef kalman
xdata volatile float angle, angle_dot; 		//kalman��ĽǶȺͽ��ٶȡ�
#endif

void Single_WriteI2C(u8 REG_Address,u8 REG_data)
{
    IIC_start();                  //��ʼ�ź�
    IIC_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    IIC_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ��
    IIC_SendByte(REG_data);       //�ڲ��Ĵ������ݣ�
    IIC_stop();                   //����ֹͣ�ź�
}
u8 Single_ReadI2C(u8 REG_Address)
{
	u8 xdata REG_data;
	IIC_start();                   //��ʼ�ź�
	IIC_SendByte(SlaveAddress);    //�����豸��ַ+д�ź�
	IIC_SendByte(REG_Address);     //���ʹ洢��Ԫ��ַ����0��ʼ	
	IIC_start();                   //��ʼ�ź�
	IIC_SendByte(SlaveAddress+1);  //�����豸��ַ+���ź�
	REG_data=IIC_readByte();       //�����Ĵ�������
	IIC_Ack(1);                //����Ӧ���ź�
	IIC_stop();                    //ֹͣ�ź�
	return REG_data;
}
//**************************************
//��ʼ��MPU6050
//**************************************
void mpu_init(void)
{
	Single_WriteI2C(PWR_MGMT_1, 0x00);	//�������״̬
	Single_WriteI2C(SMPLRT_DIV, 0x07);	//���������ǲ���Ƶ�ʣ�f=(1or8)/1+ֵ��KHz����������ͨʱΪ1���رմ�ͨʱΪ8,����ֵ��0x07
	Single_WriteI2C(CONFIG_M, 0x06);		//���ô�ͨ�˲���ͨƵ��
	Single_WriteI2C(GYRO_CONFIG, 0x10);	//����������0-1000��ÿ�룬���Լ��
	Single_WriteI2C(ACCEL_CONFIG, 0x01);//���ٶȼ��˲�5Hz������2g�����Լ��
}
//**************************************
//�ϳ�����
//**************************************
int GetData(u8 REG_Address)
{
	u8 xdata H=0,L=0;
	H=Single_ReadI2C(REG_Address);
	L=Single_ReadI2C(REG_Address+1);
	return (H<<8)+L;   //�ϳ�����
}

float mpu_A(u8 chunnel)		 //��������Ϊ��
{
	float xdata dat=0.0;

	switch(chunnel)
	{
		case 0:{
					dat=GetData(ACCEL_XOUT_H);
					if(dat>=35000){dat=0xffff-dat;dat-=Ax_zero;dat=-dat;}
					else dat-=Ax_zero;
					break;
				}
		case 2:{
					dat=GetData(ACCEL_YOUT_H);
					if(dat>=35000){dat=0xffff-dat;dat-=Ay_zero;dat=-dat;}
					else dat-=Ay_zero;
					break;
				}
		case 4:{
					dat=GetData(ACCEL_ZOUT_H);
					if(dat>=35000){dat=0xffff-dat;dat-=Az_zero;dat=-dat;}
					else dat-=Az_zero;
					break;
				}
	
	}
	

	//UART_Put_Inf("g:",(u16)dat);
	dat=dat/16384.0;
	dat=asin(dat)*57.29578;
	if(dat>90)return 90;
	else if(dat<-90)return -90;
	else return dat;
}

u8 mpu_T()//���ص�λΪ0.1 ���϶�
{
	return (u8)(350+(GetData(TEMP_OUT_H) + 13200) / 28); // ������¶�

}
float mpu_V(u8 chunnel)		 //��������Ϊ��
{
	float xdata dat=0.0;

	switch(chunnel)
	{
		case 0:{
					dat=GetData(GYRO_XOUT_H);
					if(dat>=35000){dat=0xffff-dat;dat-=Vx_zero;dat=-dat;}
					else dat-=Vx_zero;
					break;
				}
		case 2:{
					dat=GetData(GYRO_YOUT_H);
					if(dat>=35000){dat=0xffff-dat;dat-=Vy_zero;dat=-dat;}
					else dat-=Vy_zero;
					dat=-dat;//Ϊ����Ƕ�ƥ�䣬ȡ��
					break;
				}
		case 4:{
					dat=GetData(GYRO_ZOUT_H);
					if(dat>=35000){dat=0xffff-dat;dat-=Vz_zero;dat=-dat;}
					else dat-=Vz_zero;
					break;
				}
	
	}
	

//	UART_Put_Inf("g:",(u16)dat);
	dat=dat/32.75;
	
	return dat;  //Ϊ����Ƕ�ƥ�䣬ȡ��
}

//��⵱ǰ�Ƿ��������ٶȸ���
bit mpu_Atest()
{
	float xdata x=0.0,y=0.0,z=0.0;
	float xdata add=0;
	x=GetData(ACCEL_XOUT_H);
	if(x>=35000){x=0xffff-x;x-=Ax_zero;x=-x;}
	else x-=Ax_zero;
	x=x/16384.0;
	y=GetData(ACCEL_YOUT_H);
	if(y>=35000){y=0xffff-y;y-=Ay_zero;y=-y;}
	else y-=Ay_zero;
	y=y/16384.0;
	z=GetData(ACCEL_ZOUT_H);
	if(z>=35000){z=0xffff-z;z-=Az_zero;z=-z;}
	else z-=Az_zero;
	z=z/16384.0;
	add=x*x+y*y+z*z-1;
	if((add<mpu_test_num)&&(add>(-mpu_test_num)))return 0;
	else return 1;

}
#ifdef kalman

void kalman_init()
{
	TMOD|=0x10;				 //��ʱ��1������ʽ1
	TH0=(65536-36840)/256;	 //22.1184M����20MS��ʱ����
	TL0=(65536-36840)%256;
	EA=1;					 //�����ж�
	ET1=1;					 //����ʱ��1�ж�
	TR1=1;					 //������ʱ��1

}

void T1_time(void) interrupt 3
{
	TH0=(65536-36840)/256;	 //22.1184M����1MS��ʱ������������0.12us
	TL0=(65536-36840)%256;
	switch(kalman_chunnel)
	{
		case chunnel_X :  Kalman_Filter(mpu_A(chunnel_X),mpu_V(chunnel_Y));break;
		case chunnel_Y :  Kalman_Filter(mpu_A(chunnel_Y),mpu_V(chunnel_X));break;
		default : break;
	}
   	
}


//-------------------------------------------------------
//Kalman�˲���8MHz�Ĵ���ʱ��Լ1.8ms��
//-------------------------------------------------------

//-------------------------------------------------------
//static xdata volatile float angle, angle_dot; 		//�ⲿ��Ҫ���õı���

//Q��һ��2 x2����,����������Э���������������������,���������Ƕ�ô���Ų�б�������������
//R�������Э��������R = E(WT)���ڱ�����,����һ��1 x1����,˵,����Ԥ��10 ras�����Ӳ�б�ǡ�����һ��1 x1���������������V = 0.1
//ע�⣺dt��ȡֵΪkalman�˲�������ʱ��;
//const float Q_angle=0.001, Q_gyro=0.0015, R_angle=0.1, dt=0.02;
//Q_angle:������ٶȵ�Ԥ��������ֵԽ��ͱ�ʾԽ��������
//Q_gyro�������ǵ�Ԥ��������ֵԽ��ͱ�ʾԽ��������
//R_angle������������ֵԽ��ͱ�ʾ��Խ������������������ݣ���Խ������Ԥ���ֵ��
//dt������ʱ��
#define Q_angle 0.003
#define Q_gyro 0.0015
#define R_angle 0.5	
#define dt 0.02
#define C_0 1
static xdata volatile float kalman_P[2][2] = {{ 1, 0 },{ 0, 1 }};		
static xdata volatile float Pdot[4] ={0,0,0,0};	


static xdata volatile float q_bias, angle_err, PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;
//-------------------------------------------------------
void Kalman_Filter(float angle_m,float gyro_m)			//gyro_m:gyro_measure
{
	
	angle+=(gyro_m-q_bias) * dt;
	
	Pdot[0]=Q_angle - kalman_P[0][1] - kalman_P[1][0];
	Pdot[1]=- kalman_P[1][1];
	Pdot[2]=- kalman_P[1][1];
	Pdot[3]=Q_gyro;
	
	kalman_P[0][0] += Pdot[0] * dt;
	kalman_P[0][1] += Pdot[1] * dt;
	kalman_P[1][0] += Pdot[2] * dt;
	kalman_P[1][1] += Pdot[3] * dt;
	
	
	angle_err = angle_m - angle;
	
	
	PCt_0 = C_0 * kalman_P[0][0];
	PCt_1 = C_0 * kalman_P[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * kalman_P[0][1];

	kalman_P[0][0] -= K_0 * t_0;
	kalman_P[0][1] -= K_0 * t_1;
	kalman_P[1][0] -= K_1 * t_0;
	kalman_P[1][1] -= K_1 * t_1;
	
	
	angle	+= K_0 * angle_err;
	q_bias	+= K_1 * angle_err;
	angle_dot = gyro_m-q_bias;	  
}


#endif

