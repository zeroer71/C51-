/*
***********************************************************************************************************************************************
*
*		模块名称 ： 数码管模块、独立按键模块
*		文件名称 ： 独立按键检测.c
*		版本		 ： v1.0
*		说明		 ： 按键控制数码管的显示，S2使数字加一，S3使数字减一，S4使数字复位，S5控制定时器自动显示数字
*		修改记录 ：
*			  版本号	日期				作者			说明
*				v1.0		2020-01-11	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int

sbit dula=P2^6;
sbit wela=P2^7;
sbit KEY_S2=P3^4;
sbit KEY_S3=P3^5;
sbit KEY_S4=P3^6;
sbit KEY_S5=P3^7;
uint ge,shi;
uint NumTotal,NumT0;

uchar code table[]={
	0x3f,0x06,0x5b,0x4f,
	0x66,0x6d,0x7d,0x07,
	0x7f,0x6f,0x77,0x7c,
	0x39,0x5e,0x79,0x71
};

/*延时函数，延时n毫秒*/
void delay_ms(uint n)
{
	uint i,j;
	for(i=n;i>0;i--)
		for(j=110;j>0;j--);
}

/*显示数码管前两位*/
void display(uint Num)
{
	/*显示个位*/
	ge=Num%10;
	wela=1;
	P0=0xdf;
	wela=0;
	dula=1;
	P0=table[ge];
	dula=0;
	delay_ms(5);
	
	/*显示十位*/
	shi=Num/10;
	wela=1;
	P0=0xef;
	wela=0;
	dula=1;
	P0=table[shi];
	dula=0;
	delay_ms(5);
	
}

/*初始化定时器0，工作方式1，16位定时器*/
void Init()
{
	TMOD=0x01;
	/*装初值，50ms*/
	TH0=(65536-48572)/256;
	TL0=(65536-48572)%256;
	EA=1;
	ET0=1;
}


/*定时器0中断函数*/
void T0_Time() interrupt 1
{
	/*装初值，50ms*/
	TH0=(65536-48572)/256;
	TL0=(65536-48572)%256;
	
	NumT0++;
	if(NumT0==20)//到1s
	{
		NumT0=0;
		NumTotal++;
		if(NumTotal==60) NumTotal=0;
	}
}

/*扫描独立按键*/
void KeyScan()
{
	if(KEY_S2==0)
	{
		delay_ms(10);
		if(KEY_S2==0)
		{
			NumTotal++;
			if(NumTotal==60) NumTotal=0;
			while(!KEY_S2);//等待按键松开
		}
	}
	
	if(KEY_S3==0)
	{
		delay_ms(10);
		if(KEY_S3==0)
		{
			if(NumTotal==0) NumTotal=60;
			NumTotal--;
			while(!KEY_S3);
		}
	}
	
	if(KEY_S4==0)
	{
		delay_ms(10);
		if(KEY_S4==0)
		{
			NumTotal=0;
			while(!KEY_S4);
		}
	}
	
	if(KEY_S5==0)
	{
		delay_ms(10);
		if(KEY_S5==0)
		{
			while(!KEY_S5);
			TR0=~TR0;
		}
	}
}

int main()
{
	Init();
	
	while(1)
	{
		KeyScan();
		display(NumTotal);
	}
	return 0;
}