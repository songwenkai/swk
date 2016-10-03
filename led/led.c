#include<reg51.h>
sbit led=P2^0;
void delay()
{
	char i,j;
	for(i=0;i<100;i++)
	for(j=0;j<100;j++);
}
void main()
{
	while(1)
	{
		led=0;
		delay();
		led=1;
		delay();
	}
}