#ifndef __GLOBAL_VAR_H__
#define __GLOBAL_VAR_H__

typedef enum {
	CAR_LEFT = 1,
	CAR_BACKWARD,
	CAR_FORWARD,
	CAR_RIGHT,
	CAR_END,
}car_dir_t;

typedef enum {
	CAR_TYPE_RS_ONE_MOTOR = 1,
	CAR_TYPE_RS_TWO_MOTOR,
	CAR_TYPE_TT_TWO_MOTOR,
	CAR_TYPE_TT_FOUR_MOTOR
}car_type_t;

typedef struct {
	car_type_t car_type;	//车类型
	int car_speed;	//速度，最高位表示方向 1：前 0：后
	int car_angle;	//角度，最高位表示方向 1：右 0：左
	unsigned int car_verify;		// 校验
	/***按键控制**/		
	int Angle_left;
	int Speed_right;
	int key_left_value;
    int key_right_value;
	/************/
	/*陀螺仪控制*/
	int X_angle;
	int Y_angle;

}nrf_pkt_t;

typedef enum {
	NRF_TX = 1,
	NRF_RX,
	NRF_DIR_END,	
}nrf_dir_t;

#define PI 3.14
#define CAR_WHEEL_RADIUS 33 	//车轮半径 单位厘米
#define CAR_CYCLE_PULSE  30		//码盘线数
#define CAR_SPEED_TIMEUP 1000	//1000ms

#define DJ_MIDDLE   1500
extern unsigned int  duoji_pulse;

extern unsigned char car_cesu_start;
extern unsigned int  car_speed_left_count, car_speed_right_count;	//左右轮脉冲计数
extern unsigned int  car_speed_left, car_speed_right; 		//厘米/秒
extern unsigned int  car_speed_time;							//时间计数

extern unsigned char pt_value, key_value;
extern unsigned char dis_buf[15];

extern unsigned char pwm0, pwm1;
extern unsigned char debug_5110;


#endif