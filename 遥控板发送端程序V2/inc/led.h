#ifndef __LED_H__
#define __LED_H__

#include "type.h"

#define LED_ON  0
#define LED_OFF 1
#define led_on_all() {led1 = led2 = led3 = led4 = 0;}
#define led_off_all() {led1 = led2 = led3 = led4 = 1;}

void led_init(void);
void led_state(u8 ledx, u8 state);
void led_test(void);




#endif