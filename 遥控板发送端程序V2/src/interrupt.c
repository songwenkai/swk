#include <stdio.h>
#include "interrupt.h"
#include "global_var.h"
#include "stc12c5a60s2.h"
#include "io_cfg.h"
#include "key_scan.h"
#include "nokia_5110.h"
/**************************************************************************
 - 功能描述：51单片机的定时器0初始化
 - 隶属模块：STC51定时器
 - 函数属性：外部，使用户使用
 - 参数说明：无
 - 返回说明：无
 **************************************************************************/
void time0_init(void) {
	TMOD |=0X01;				/* 16位定时/计数器	*/
	TH0=(65535-duoji_pulse)>>8;		/* 重新赋值 高位	*/
    TL0=(65535-duoji_pulse)&0xFF;		/* 重新赋值	低位	*/
	TR0=0;						/* 关闭定时器开关	*/
 	ET0=0;						/* 关闭定时器中断	*/
	return;
}
/**************************************************************************
 - 功能描述：51单片机的定时器1初始化
 - 隶属模块：STC51定时器
 - 函数属性：外部，使用户使用
 - 参数说明：无
 - 返回说明：无
 **************************************************************************/
void time1_init(void) {
	TMOD |=0X10;				/* 16位定时/计数器	*/
	TH1=(65535-20000)>>8;		/* 重新赋值 高位	*/
    TL1=(65535-20000)&0xFF;		/* 重新赋值	低位	*/
	TR1=0;						/* 关闭定时器开关	*/
 	ET1=0;						/* 关闭定时器中断	*/
	return;	
}

/**************************************************************************
 - 功能描述：51单片机的外部中断0初始化
 - 隶属模块：STC51中断
 - 函数属性：外部，使用户使用
 - 参数说明：无
 - 返回说明：无
 **************************************************************************/
void ext0_init(void) {
	IT0 = 1;                    /*下降沿触发*/
	EX0 = 0;					/*disable ext0*/
	return;
}
/**************************************************************************
 - 功能描述：51单片机的外部中断1初始化
 - 隶属模块：STC51中断
 - 函数属性：外部，使用户使用
 - 参数说明：无
 - 返回说明：无
 **************************************************************************/
void ext1_init(void) {
	IT1 = 1;                    /*下降沿触发*/
	EX1 = 0;					/*disable ext1*/
	return;
}
/**************************************************************************
 - 功能描述：51单片机的外部中断0子程序
 - 隶属模块：STC51中断
 - 函数属性：外部，使用户使用
 - 参数说明：无
 - 返回说明：无
 **************************************************************************/
void ext0_interrupt() interrupt 0 
{
	car_speed_left_count ++;
	return;	
}
 /**************************************************************************
 - 功能描述：51单片机的定时器中断1子程序
 - 隶属模块：STC51中断
 - 函数属性：外部，使用户使用
 - 参数说明：无
 - 返回说明：无
 **************************************************************************/
void ext1_interrupt() interrupt 2 
{
	car_speed_right_count ++;
	return;	
}
 /**************************************************************************
 - 功能描述：51单片机的定时器0中断子程序
 - 隶属模块：STC51中断
 - 函数属性：外部，使用户使用
 - 参数说明：无
 - 返回说明：无
 **************************************************************************/
void timer0_interrupt() interrupt 1 //定时器0中断子程序
{
	static unsigned char flag = 0;
	if(1 == flag){
		flag = 0;
		TH0=(65535-(20000-duoji_pulse))>>8;		/* 重新赋值 高位	*/
	    TL0=(65535-(20000-duoji_pulse))&0xFF;	/* 重新赋值	低位	*/
		led1 =~ led1;
	}else{
		flag = 1;
		TH0=(65535-duoji_pulse)>>8;			/* 重新赋值 高位	*/
	    TL0=(65535-duoji_pulse)&0xFF;		/* 重新赋值	低位	*/	
	}
	return;
}

#if 0
 /**************************************************************************
 - 功能描述：51单片机的定时器1中断子程序
 - 隶属模块：STC51中断
 - 函数属性：外部，使用户使用
 - 参数说明：无
 - 返回说明：无
 **************************************************************************/
void timer1_interrupt() interrupt 3 //定时器1中断子程序
{
	TH1=(65535-20000)>>8;		/* 重新赋值 高位	*/
	TL1=(65535-20000)&0xFF;		/* 重新赋值	低位	*/	
	
	//测速 每1S计算一次
	if(car_cesu_start==1) {
		car_speed_time++ ; 
		if(car_speed_time > 50) { 				/* 计时20msx50=1S */
			EX0 = 0;
			EX1 = 0;
			car_speed_left =  2*PI*CAR_WHEEL_RADIUS*(car_speed_left_count*1.00/CAR_CYCLE_PULSE);  //
			car_speed_right = 2*PI*CAR_WHEEL_RADIUS*(car_speed_right_count*1.00/CAR_CYCLE_PULSE);	//
			car_speed_time = 0;
			car_speed_left_count = 0;
			car_speed_right_count = 0;
			EX0 = 1;
			EX1 = 1;
		}
	}
	
	if(0 != (key_value = key_scan())) {
		if(3 == key_value) {
			//debug_5110 = ~debug_5110;
			//led4 = debug_5110;
		}
	}
	

	return;
}
#endif