#ifndef __UART_H__
#define __UART_H__
void uart_init(void);
void UART1_SendOneChar(unsigned char val);  
void UART1_SendString(unsigned char *str);
void u32tostr(unsigned long dat,char *str); 
void UART_Put_Num(unsigned long dat);
void UART_Put_Inf(char *inf,unsigned long dat);
#endif