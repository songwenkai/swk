#include <stc12c5a60s2.h> 
#include "delay.h"
#include "iic.h"
#include "mpu6050.h"
#include "math.h"
#include "uart.h"

#ifdef kalman
xdata volatile float angle, angle_dot; 		//kalman后的角度和角速度。
#endif

void Single_WriteI2C(u8 REG_Address,u8 REG_data)
{
    IIC_start();                  //起始信号
    IIC_SendByte(SlaveAddress);   //发送设备地址+写信号
    IIC_SendByte(REG_Address);    //内部寄存器地址，
    IIC_SendByte(REG_data);       //内部寄存器数据，
    IIC_stop();                   //发送停止信号
}
u8 Single_ReadI2C(u8 REG_Address)
{
	u8 xdata REG_data;
	IIC_start();                   //起始信号
	IIC_SendByte(SlaveAddress);    //发送设备地址+写信号
	IIC_SendByte(REG_Address);     //发送存储单元地址，从0开始	
	IIC_start();                   //起始信号
	IIC_SendByte(SlaveAddress+1);  //发送设备地址+读信号
	REG_data=IIC_readByte();       //读出寄存器数据
	IIC_Ack(1);                //接收应答信号
	IIC_stop();                    //停止信号
	return REG_data;
}
//**************************************
//初始化MPU6050
//**************************************
void mpu_init(void)
{
	Single_WriteI2C(PWR_MGMT_1, 0x00);	//解除休眠状态
	Single_WriteI2C(SMPLRT_DIV, 0x07);	//设置陀螺仪采样频率：f=(1or8)/1+值（KHz），开启带通时为1，关闭带通时为8,典型值：0x07
	Single_WriteI2C(CONFIG_M, 0x06);		//设置带通滤波器通频带
	Single_WriteI2C(GYRO_CONFIG, 0x10);	//陀螺仪量程0-1000度每秒，不自检测
	Single_WriteI2C(ACCEL_CONFIG, 0x01);//加速度计滤波5Hz，量程2g，不自检测
}
//**************************************
//合成数据
//**************************************
int GetData(u8 REG_Address)
{
	u8 xdata H=0,L=0;
	H=Single_ReadI2C(REG_Address);
	L=Single_ReadI2C(REG_Address+1);
	return (H<<8)+L;   //合成数据
}

float mpu_A(u8 chunnel)		 //仰角向上为正
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

u8 mpu_T()//返回单位为0.1 摄氏度
{
	return (u8)(350+(GetData(TEMP_OUT_H) + 13200) / 28); // 计算出温度

}
float mpu_V(u8 chunnel)		 //仰角向上为正
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
					dat=-dat;//为了与角度匹配，取反
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
	
	return dat;  //为了与角度匹配，取反
}

//监测当前是否有外界加速度干扰
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
	TMOD|=0x10;				 //定时器1工作方式1
	TH0=(65536-36840)/256;	 //22.1184M晶振20MS计时常数
	TL0=(65536-36840)%256;
	EA=1;					 //开总中断
	ET1=1;					 //开定时器1中断
	TR1=1;					 //启动定时器1

}

void T1_time(void) interrupt 3
{
	TH0=(65536-36840)/256;	 //22.1184M晶振1MS计时常数，最大误差0.12us
	TL0=(65536-36840)%256;
	switch(kalman_chunnel)
	{
		case chunnel_X :  Kalman_Filter(mpu_A(chunnel_X),mpu_V(chunnel_Y));break;
		case chunnel_Y :  Kalman_Filter(mpu_A(chunnel_Y),mpu_V(chunnel_X));break;
		default : break;
	}
   	
}


//-------------------------------------------------------
//Kalman滤波，8MHz的处理时间约1.8ms；
//-------------------------------------------------------

//-------------------------------------------------------
//static xdata volatile float angle, angle_dot; 		//外部需要引用的变量

//Q是一个2 x2矩阵,代表了流程协方差噪声。在这种情况下,它表明我们多么相信测斜仪相对于陀螺仪
//R代表测量协方差噪声R = E(WT)。在本例中,它是一个1 x1矩阵,说,我们预计10 ras抖动从测斜仪。对于一个1 x1矩阵在这种情况下V = 0.1
//注意：dt的取值为kalman滤波器采样时间;
//const float Q_angle=0.001, Q_gyro=0.0015, R_angle=0.1, dt=0.02;
//Q_angle:三轴加速度的预测噪声，值越大就表示越不相信它
//Q_gyro：陀螺仪的预测噪声，值越大就表示越不相信它
//R_angle：测量噪声，值越大就表示你越不相信你测量到的数据，而越相信你预测的值。
//dt：采样时间
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

