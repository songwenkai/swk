#include "led.h"
#include "io_cfg.h"
#include "delay.h"
		  
void led_init(void){
	led_off_all();
}
 /******************************************************************
 - 功能描述:关闭或点亮 指定LED亮灭
 - 隶属模块：公开函数模块
 - 函数属性：外部，用户可调用
 - 参数说明：ledx  表示指定的LED  state 表示led状态值  0 或者 1         
 - 返回说明：无
 ******************************************************************/
void led_state(u8 ledx, u8 state) {
	switch(ledx) {
		case 0:led1 = state;break;
		case 1:led2 = state;break;
		case 2:led3 = state;break;
		case 3:led4 = state;break;
		default:break;
	}
}
 /******************************************************************
 - 功能描述:轮流点亮4个led在轮流熄灭
 - 隶属模块：公开函数模块
 - 函数属性：外部，用户可调用
 - 参数说明：无      
 - 返回说明：无
 ******************************************************************/
void led_test(void) {
	char i = 0;
	for(i = 0;i < 4; i++) {
		led_state(i, LED_ON);
		mdelay(1000);
	}
	
	for(i = 3;i > -1; i--) {
		led_state(i, LED_OFF);
		mdelay(1000);
	}
	return;
}



