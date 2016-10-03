#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#define interrupt_open() 	{EA = 1;}
#define interrupt_close() 	{EA = 0;}
#define time0_open()		{TR0 = 1;ET0 = 1;}
#define time0_close()		{TR0 = 0;ET0 = 0;}
#define time1_open()		{TR1 = 1;ET1 = 1;}
#define time1_close()		{TR1 = 0;ET1 = 0;}
#define ext0_open()			{EX0 = 1;}
#define ext0_close()		{EX0 = 0;}
#define ext1_open()			{EX1 = 1;}
#define ext1_close()		{EX1 = 0;}
#define time_close()		{TR0 = 0;ET0 = 0;TR1 = 0;ET1 = 0;}
#define ext_close()			{EX0 = 0;EX1 = 0;};


void time0_init(void);
void time1_init(void);
void ext0_init(void);
void ext1_init(void);

#endif