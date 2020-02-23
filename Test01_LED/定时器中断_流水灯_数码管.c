/*
***********************************************************************************************************************************************
*
*		模块名称 ： 发光灯模块、数码管模块
*		文件名称 ： 定时器中断_发光灯_数码管.c
*		版本		 ： v1.0
*		说明		 ： 定时器0的方式1实现第一个发光灯以200ms间隔闪烁，定时器1的方式1实现数码管前两位59s循环计时
*		修改记录 ：
*			  版本号	日期				作者			说明
*				v1.0		2020-01-11	Yang			v1.0版本进行测试
*				v1.1		2020-01-11  Yang			改成可以显示秒分时的24小时制时钟,发光灯改为间隔1s闪烁
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int

sbit dula=P2^6;
sbit wela=P2^7;
sbit led=P1^0;
uint ge,shi,bai,qian,wan,shiWan;
uint NumT0,NumT1_1,NumT1_2,NumT1_3,NumT1_4;

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

/*显示数码管前六位*/
void display(uint ge,uint shi,bai,qian,wan,shiWan)
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
	
	/*显示百位*/
	wela=1;
	P0=0xf7;
	wela=0;
	dula=1;
	P0=table[bai];
	dula=0;
	delay_ms(5);
	
	/*显示千位*/
	wela=1;
	P0=0xfb;
	wela=0;
	dula=1;
	P0=table[qian];
	dula=0;
	delay_ms(5);
	
	/*显示万位*/
	wela=1;
	P0=0xfd;
	wela=0;
	dula=1;
	P0=table[wan];
	dula=0;
	delay_ms(5);
	
	/*显示十万位*/
	wela=1;
	P0=0xfe;
	wela=0;
	dula=1;
	P0=table[shiWan];
	dula=0;
	delay_ms(5);
	
}


int main()
{
	TMOD=0x11;
	/*设置定时器0为工作方式1，50ms*/
	TH0=(65536-45872)/256;
	TL0=(65536-45872)%256;
	EA=1;
	ET0=1;
	TR0=1;
	
	/*设置定时器1为工作方式1,50ms*/
	TH1=(65536-45872)/256;
	TL1=(65536-45872)%256;
	EA=1;
	ET1=1;
	TR1=1;
	
	while(1)
	{
		display(ge,shi,bai,qian,wan,shiWan);
	}
	
}	

/*定时器0中断，实现发光灯间隔1s闪烁*/
void T0_Time() interrupt 1
{
	/*重装初值,50ms*/
	TH0=(65536-48572)/256;
	TH1=(65536-48572)%256;
	
	NumT0++;
	if(NumT0==20)//到1s
	{
		NumT0=0;
		led=~led;
	}
	
}

/*定时器1中断，实现数码管前六位24小时制时钟循环*/
void T1_Time() interrupt 3
{
	/*重装初值，50ms*/
	TH1=(65536-48572)/256;
	TL1=(65536-48572)%256;
	
	NumT1_1++;
	if(NumT1_1==20)//到1秒
	{
		NumT1_1=0;
		NumT1_2++;//记录秒数
		if(NumT1_2==60)
	  {
			NumT1_2=0;
			NumT1_3++;//记录分钟
			if(NumT1_3==60)
			{
				NumT1_3=0;
				NumT1_4++;//记录小时
				if(NumT1_4==24)
				{
					NumT1_4=0;
				}
			}
		}
		ge=NumT1_2%10;
		shi=NumT1_2/10;
		bai=NumT1_3%10;
		qian=NumT1_3/10;
		wan=NumT1_4/10;
		shiWan=NumT1_4%10;
	}
	
}