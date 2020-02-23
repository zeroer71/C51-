/*
***********************************************************************************************************************************************
*
*		模块名称 ： 数码管模块
*		文件名称 ： 软件实现系统复位.c
*		版本		 ： v1.0
*		说明		 ： 在数码管前两位显示以秒增的数，增加到10时，利用STC单片机的软件复位功能让单片机复位
*	
*						
*		修改记录 ：
*			  版本号		日期				作者			说明
*				v1.0		2020-01-31	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int

sfr  ISP_CONTR=0xe7;//声明ISP控制寄存器
sbit dula=P2^6;
sbit wela=P2^7;

uchar code table[]={
	0x3f,0x06,0x5b,0x4f,
	0x66,0x6d,0x7d,0x07,
	0x7f,0x6f,0x77,0x7c,
	0x39,0x5e,0x79,0x71
};
uchar num_s;
uchar num_ms;

/****延时函数，延时n毫秒****/
void delay_ms(uint n)
{
	uint i,j;
	for(i=n;i>0;i--)
		for(j=110;j>0;j--);
}

/****显示数码管前两位****/
void display(uchar ge,uchar shi)
{
	/*显示个位*/
	wela=1;
	P0=0xdf;
	wela=0;
	dula=1;
	P0=table[ge];
	dula=0;
	delay_ms(5);
	
	/*显示十位*/
	wela=1;
	P0=0xef;
	wela=0;
	dula=1;
	P0=table[shi];
	dula=0;
	delay_ms(5);	
}

/****主函数****/
void main()
{
	/*开启定时器0，设为工作方式1*/
	TMOD=0x01;
	TH0=(65536-50000)/256;//装初值，50ms一次
	TL0=(65536-50000)%256;
	TR0=1;
	
	/*开启中断*/
	EA=1;//总中断
	ET0=1;//定时器0中断
	
	/*循环读取数据*/
	while(1)
	{
		if(num_ms==20)
		{
			num_ms=0;
			num_s++;
			if(num_s==10)
			{
				ISP_CONTR=0x20;
			}
		}
		display(num_s%10,num_s/10);
	}
}

/****定时器0中断服务函数****/
void T0_time() interrupt 1
{
	TH0=(65536-50000)/256;//装初值，50ms一次
	TL0=(65536-50000)%256;
	num_ms++;
}