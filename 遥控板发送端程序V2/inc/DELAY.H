#ifndef __Delay_h__
#define __Delay_h__

#include "type.h"

void delay(unsigned int t);//基本的延时
void delay_ms(unsigned int t);//根据微妙确定毫秒
void delay_us(void); 
#define mdelay(t) delay_ms(t)
#define udelay(t) delay_us(t)

#endif