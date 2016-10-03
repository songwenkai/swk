
/*******************************************************

        +-----------------------------------------+
        |�����ǵ�    רע���ܳ������˿���|
        +-----------------------------------------+

  ��Դ���Ȩ�� �����ǵ� ȫȨ���У��������ã�������������֪
        �Ͻ�����������ҵĿ�ģ�Υ�߱ؾ�������Ը�
             ->��Ʒ���� http://hailzd.taobao.com/
             ->��������Ⱥ QQȺ1:372079996  QQȺ2:341730862 			  
********************************************************/
#include "stc12c5a60s2.h"
#include <stdio.h>			/* ��׼IO			     */
#include <string.h>			/* ��׼��				 */
#include "sconfig.h" 		/* ��������     		 */
#include "type.h"			/* �������Ͷ���			 */
#include "io_cfg.h"			/* �˿�����				 */
#include "global_var.h"		/* ȫ�ֱ��������Ͷ��� 	 */
#include "led.h"
#include "delay.h"
#include "key_scan.h"
#include "nokia_5110.h"
#include "nrf24L01.h"
//#include "interrupt.h"
#include "ad_get.h"
#include "uart.h"
#include "iic.h"
#include "mpu6050.h"
#define CAR_SPEED_RIGHT_IO 0     //P10 - P17 ��ͨ��ADC
#define CAR_ANGLE_RIGHT_IO 1    //��ȡP15���IO��adcֵ
#define CAR_SPEED_LEFT_IO  2
#define CAR_ANGLE_LEFT_IO  3
/**********************************************************/
/**********************************************************/
/**********************************************************/
void sys_init(void);
void io_init(void);
void global_var_init(void);
void dis_init(void);
void nrf_init(nrf_dir_t nrf_dir);

void beep_test(void);
void nrf_ctl(void);
void fill_nrf_pkt(nrf_pkt_t *nrf_pkt);

/**********************************************************/
/**********************************************************/
/**********************************************************/
unsigned char dis_buf[15] = "              ";

nrf_pkt_t nrf_pkt;
unsigned char nrf_buf[sizeof(nrf_pkt_t)];
nrf_dir_t nrf_dir_flag;
unsigned char nrf_init_flag = 0;
unsigned char debug_5110;
unsigned int car_speed_left_ad_middle_value,car_speed_right_ad_middle_value;
unsigned int car_angle_left_ad_middle_value,car_angle_right_ad_middle_value;
int i=0;

int main(void) 
{
	sys_init();
	while(1) 
	{			
		nrf_ctl();
	}
	return 0;
}

void sys_init(void) 
{

	io_init();
	global_var_init();
//	led_init();
	nrf_tx_mode_init();
	dis_init();
	//led_test();
	uart_init();
	mpu_init();
  delay_ms(150);	
	beep_test();
	//uart_init();
   //mpu_init();
	delay_ms(250);	
	kalman_init();
}


void io_init(void) 
{
	P4SW |= 0xFF;	//����P4��	
//	P1M0 &= 0Xf0;   //P14 P15 P16 P17����ΪAD�ɼ�ģʽ
//	P1M1 |= ~0Xf0;
	
	return;
}

void global_var_init(void) 
{
		
	car_speed_left_ad_middle_value = read_adc(CAR_SPEED_LEFT_IO);
	car_speed_right_ad_middle_value = read_adc(CAR_SPEED_RIGHT_IO);
	car_angle_left_ad_middle_value = read_adc(CAR_ANGLE_LEFT_IO);
	car_angle_right_ad_middle_value = read_adc(CAR_ANGLE_RIGHT_IO);

	debug_5110 = 0;
	return;
}


void dis_init(void) 
{
	LCD_init(); //��ʼ��Һ��    
	LCD_clear();
	LCD_write_english_string(0,0,"+_+HLKJ CAR+_+");
	LCD_write_english_string(0,1,"Type: RS_ONE  ");
	LCD_write_english_string(0,2,"Sspd: xxx cm/s");
	LCD_write_english_string(0,3,"Angl: xxx     ");
	LCD_write_english_string(0,4,"Gspd: xxx cm/s");
	return;
}


void beep_test(void) 
{
	beep = 0;
	delay_ms(500);
	beep = 1;	
}


void nrf_ctl(void) 
{
	unsigned char ret;
	fill_nrf_pkt(&nrf_pkt);//������ݰ�
	memcpy(nrf_buf, &nrf_pkt, sizeof(nrf_pkt_t));

	ret = nRF24L01_TxPacket(nrf_buf);
	if(ret) 
	{
		if(debug_5110) 
		{
			//������ʾ
			sprintf(dis_buf, "Type: %s", 
			nrf_pkt.car_type==CAR_TYPE_RS_ONE_MOTOR?"RS_ONE  ":
			nrf_pkt.car_type==CAR_TYPE_RS_TWO_MOTOR?"RS_TWO  ":
			nrf_pkt.car_type==CAR_TYPE_RS_TWO_MOTOR?"TT_TWO  ":
			nrf_pkt.car_type==CAR_TYPE_RS_TWO_MOTOR?"TT_FOUR ":
													"TYPE ERR");
			LCD_write_english_string(0, 1, dis_buf);
			
			//�ٶ���ʾ
			sprintf(dis_buf,"Sspd: xxxxcm/s");
			if(nrf_pkt.car_speed & 0x8000) 
			{
				dis_buf[5] = '+';
				nrf_pkt.car_speed = 1024 - nrf_pkt.car_speed;
			} 
			else 
			{
				dis_buf[5] = '-';
			}		
			dis_buf[6] = (nrf_pkt.car_speed&0x3ff)/1000 + '0';
			dis_buf[7] = (nrf_pkt.car_speed&0x3ff)%1000/100 + '0';
			dis_buf[8] = (nrf_pkt.car_speed&0x3ff)%100/10 + '0';
			dis_buf[9] = (nrf_pkt.car_speed&0x3ff)%10 + '0';
			LCD_write_english_string(0, 2, dis_buf);
			
			//�Ƕ���ʾ
			sprintf(dis_buf, "Angl: xxxx   ");
			if(nrf_pkt.car_angle & 0x8000) 
			{
				dis_buf[5] = '+';
				nrf_pkt.car_angle = 1024 - nrf_pkt.car_angle&0x3ff;
			} 
			else 
			{
				dis_buf[5] = '-';
			}
			dis_buf[6] = (nrf_pkt.car_angle&0x3ff)/1000 + '0';
			dis_buf[7] = (nrf_pkt.car_angle&0x3ff)%1000/100 + '0';
			dis_buf[8] = (nrf_pkt.car_angle&0x3ff)%100/10 + '0';
			dis_buf[9] = (nrf_pkt.car_angle&0x3ff)%10 + '0';
			LCD_write_english_string(0, 3, dis_buf);
			
			LCD_write_english_string(0, 5, "send  success! ");
		}
		led_nrf = ~led_nrf;
	} 
	else 
	{
		if(debug_5110)
		{
			LCD_write_english_string(0, 5, "send  error!   ");
		}
		led_nrf = 1;
	}
	return;
}

void fill_nrf_pkt(nrf_pkt_t *nrf_pkt) 
{
	unsigned int temp;
	memset(nrf_pkt, 0, sizeof(nrf_pkt_t));
	
	//������
	nrf_pkt->car_type = CAR_TYPE_RS_ONE_MOTOR;
	
	//�ٶ�  
	//temp = read_adc(CAR_SPEED_LEFT_IO); 
	//nrf_pkt->car_speed = temp - car_speed_left_ad_middle_value;
	temp = read_adc(CAR_SPEED_RIGHT_IO); 
	nrf_pkt->car_speed = temp - car_speed_right_ad_middle_value;   //��ȡ��ҡ��ADCֵ����ֵ��nrf_pkt->car_speed��Ա
	
    //UART_Put_Inf("speed1:",temp);	
    //	UART_Put_Inf("speed2:",nrf_pkt->car_speed);	
 
	//�Ƕ�
	//temp = read_adc(CAR_SPEED_RIGHT_IO);
	//nrf_pkt->car_angle = temp - car_angle_right_ad_middle_value;
	  temp = read_adc(CAR_ANGLE_LEFT_IO); 
	  nrf_pkt->car_angle = temp - car_angle_left_ad_middle_value;
	/*
	UART_Put_Inf("angle1:",temp);	
	UART_Put_Inf("angle2:",nrf_pkt->car_speed);	
	*/
	//temp = read_adc(CAR_ANGLE_LEFT_IO);
	//nrf_pkt->car_angle = temp - car_angle_left_ad_middle_value;
	nrf_pkt->Angle_left = key_scan_left();
	nrf_pkt->Speed_right = key_scan_right();
	nrf_pkt->key_left_value = key_scan_left();
	nrf_pkt->key_right_value = key_scan_right();
	/******************************/
	// UART1_SendOneChar(nrf_pkt->key_left_value+0x30);
	// UART1_SendOneChar(nrf_pkt->Speed_right+'0');
	/******************************/
	nrf_pkt->X_angle = mpu_A(chunnel_X);
	nrf_pkt->Y_angle = mpu_A(chunnel_Y);
	#if  0
	if((nrf_pkt->X_angle)>0)
	UART_Put_Inf("Ax:",nrf_pkt->X_angle);	//��ʾX��Ƕ�
	else
	{
		nrf_pkt->X_angle=-nrf_pkt->X_angle;
		UART_Put_Inf("Ax:-",nrf_pkt->X_angle);
	}
		
   // UART_Put_Inf("speed:",temp);	//��ʾX��Ƕ�

	
   
	if((nrf_pkt->Y_angle)>0)
	UART_Put_Inf("Ay:",nrf_pkt->Y_angle);	//��ʾX��Ƕ�
	else
	{
		nrf_pkt->Y_angle=-nrf_pkt->Y_angle;
		UART_Put_Inf("Ay:-",nrf_pkt->Y_angle);
	} 
		
  //UART_Put_Inf("speed:",temp);	//��ʾX��Ƕ�
	#endif
	//У��
	nrf_pkt->car_verify = nrf_pkt->car_type + nrf_pkt->car_speed + nrf_pkt->car_angle;
	
	return;
}
