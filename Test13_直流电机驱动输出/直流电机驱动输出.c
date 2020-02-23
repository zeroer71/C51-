/*
***********************************************************************************************************************************************
*
*		模块名称 ： 数码管模块、独立按键模块
*		文件名称 ： 直流电机驱动输出.c
*		版本		 ： v1.0
*		说明		 ： 按键控制直流电机的PWM信号输出，S2按下加速，S3按下减速
*		修改记录 ：
*			  版本号	日期				作者			说明
*				v1.0		2020-02-11	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int

sbit dula=P2^6;
sbit wela=P2^7;
sbit Machine=P1^7;
sbit KEY_S2_AddSpeed=P3^4;//加速按键
sbit KEY_S3_SubSpeed=P3^5;//减速按键

uint num,Num_Show,Num_High,Num_Low;

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

/*显示数码管前四位*/
void display(uint Num)
{
	/*显示个位*/
	dula=0;
	P0=table[0];
	dula=0;
	dula=1;
	P0=0xdf;
	wela=0;
	wela=1;
	delay_ms(5);
	
	/*显示十位*/
	dula=0;
	P0=table[0];
	dula=0;
	dula=1;
	P0=0xdf;
	wela=0;
	wela=1;
	delay_ms(5);
	
	/*显示百位*/
	dula=0;
	P0=table[0];
	dula=0;
	dula=1;
	P0=0xdf;
	wela=0;
	wela=1;
	delay_ms(5);
	
	/*显示千位*/
	dula=0;
	P0=table[Num_Show];
	dula=0;
	dula=1;
	P0=0xdf;
	wela=0;
	wela=1;
	delay_ms(5);
		
}

/*扫描按键函数*/
void KeyScan()
{
	/*检测加速按键是否按下*/
	if (KEY_S2_AddSpeed==0)
	{
		delay_ms(5);//延时消抖
		if (KEY_S2_AddSpeed==0)
		{
			num++;
			/*如果增速超过了最高速3*/
			if (num==4)
			{
				num=3;
			}
			while(KEY_S2_AddSpeed==0);
		}
	}

	/*检测减速按键释放按下*/
	if (KEY_S3_SubSpeed==0)
	{
		delay_ms(5);//延时消抖
		if (KEY_S3_SubSpeed==0)
		{
			if (num!=0)
			{
				num--;
			}

			else
				num=0;
			while(KEY_S3_SubSpeed==0);
		}
	}
}

/*根据速度标志进行数据处理*/
void DisposeSpeed()
{
	switch(num)
	{
		case 0:
				Num_Show=1;
				Num_High=1;
				Num_Show=3;

		case 1:
				Num_Show=2;
				Num_High=2;
				Num_Show=2;

		case 2:
				Num_Show=3;
				Num_High=3;
				Num_Show=1;

		case 3:
				Num_Show=4;
				Num_High=4;
				Num_Show=0;
	}
}


/*驱动直流电机*/
void DriveMachine()
{
	uchar i;

	/*PWM输出低电平*/
	if (Num_Show!=0)
	{
		for (int i = 0; i < Num_Low; ++i)
		{
			Machine=0;	
			display();
		}
	}

	/*PWM输出高电平*/
	for (int i = 0; i < Num_High; ++i)
	{
		Machine=1;
		display();
	}
}


/*主函数*/
void main()
{
	while(1)
	{
		Machine=0;
		KeyScan();
		DisposeSpeed();
		DriveMachine();
	}
}