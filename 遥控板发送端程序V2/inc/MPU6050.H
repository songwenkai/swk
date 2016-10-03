#ifndef __MPU6050_H__
#define __MPU6050_H__
#include"type.h"


#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG_M			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I			0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0xD0	//IIC写入时的地址字节数据，+1为读取


#define Ax_zero	578		  //0度X寄存器值
#define Ay_zero	(-896)	  //0度Y寄存器值
#define Az_zero	(-750)	  //0度Z寄存器值

#define Vx_zero	(-92)	  //0速度X寄存器值
#define Vy_zero	(-26)	  //0速度Y寄存器值
#define Vz_zero	(21)	  //0速度Z寄存器值

#define chunnel_X 0
#define chunnel_Y 2
#define chunnel_Z 4

#define mpu_test_num 0.08	//动静监测系数，越小越偏向于“动”。典型值0.1

#define kalman 1
#define kalman_chunnel	chunnel_Y


//MPU6050操作函数
void mpu_init(void);					//初始化MPU6050
u8 Single_ReadI2C(u8 REG_Address);						//读取I2C数据
void  Single_WriteI2C(u8 REG_Address,u8 REG_data);	//向I2C写入数据
int GetData(u8 REG_Address);   //合成数据，内部调用
float mpu_A(u8 chunnel);	   //读取角度，仰角向上为正,Z向下为正。
float mpu_V(u8 chunnel);	   //读取角速度，XY以其垂直轴向仰角方向滚动为正，Z向左滚动为正。	
u8 mpu_T(void);				   //读取温度。
bit mpu_Atest();               //监测当前是否有外界加速度干扰
#ifdef kalman
void Kalman_Filter(float angle_m,float gyro_m);	  //kalman运算函数
void kalman_init(void);		   //卡曼初始化，调用此函数会降低main效率
#endif


#endif
