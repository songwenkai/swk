#ifndef __CONFIG_H__
#define __CONFIG_H__

#define STC12
//#define STC89

//#define HAVE_NRF

#ifdef HAVE_NRF
//#define NRF_RX_MODE
//#define NRF_TX_MODE
#endif

#define LCD5110

//#define HAVE_DJ
//#define HAVE_CS
//#define HAVE_UART

#define XJ_CTL
//#define PT2272_CTL
//#define NRF_CTL
//#define BZ_CTL

#define TIME_INTERRUPT
//#define EXT_INTERRUPT

//PWM module
#ifdef XJ_CTL
#define USE_PCA0
#define USE_PCA1
#endif

//car type
//#define TT_MOTOR2_NO_DJ
//#define TT_MOTOR_AND_DJ
//#define TT_MOTOR2_AND_DJ
//#define RS_MOTOR_AND_DJ
//#define RS_MOTOR2_AND_DJ

#define NRF_TT2_CTL 	0X01
#define NRF_TT2_DJ1_CLT	0X02
#define NRF_RS1_DJ1_CTL 0X03
#define NRF_RS2_DJ2_CTL 0X04
#endif