/*
***********************************************************************************************************************************************
*
*		模块名称 ： 数码管模块、矩阵键盘模块
*		文件名称 ： 矩阵键盘检测.c
*		版本		 ： v1.0
*		说明		 ： 4X4矩阵键盘控制数码管显示"0~F"
*		修改记录 ：
*			  版本号	日期				作者			说明
*				v1.0		2020-01-12	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int

sbit dula=P2^6;
sbit wela=P2^7;

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

/*显示数码管前一位*/
void display(uchar num)
{
	/*显示段数据*/
	P0=table[num];
	dula=1;
	dula=0;
}

/*矩阵键盘检测*/
void MatrixKeyScan()
{
	uchar key,temp;
	/*temp=P3为扫描P3端口*/
	
	/*检测第一行*/
	P3=0xfe;//第一行为低电平
	temp=P3;
	temp=temp&0xf0;//转换为列电平
	if(temp!=0xf0)//第一次检测按键是否按下
	{
		delay_ms(10);//延时消抖
		temp=P3;
		temp=temp&0xf0;
		if(temp!=0xf0)//第二次检测
		{
			temp=P3;
			switch(temp)
			{
				case 0xee:
					key=0;
					break;
				
				case 0xde:
					key=1;
					break;
				
				case 0xbe:
					key=2;
					break;
				
				case 0x7e:
					key=3;
					break;
			}
			while(temp!=0xf0)//检测按键是否松开，没松开则一直检测
			{
				temp=P3;
				temp=temp&0xf0;
			}
			display(key);//等按键松开后，再显示数字
		}
	}
	
	/*检测第二行*/
	P3=0xfd;//第二行为低电平
	temp=P3;
	temp=temp&0xf0;//转换为列电平
	if(temp!=0xf0)//第一次检测按键是否按下
	{
		delay_ms(10);//延时消抖
		temp=P3;
		temp=temp&0xf0;
		if(temp!=0xf0)//第二次检测
		{
			temp=P3;
			switch(temp)
			{
				case 0xed:
					key=4;
					break;
				
				case 0xdd:
					key=5;
					break;
				
				case 0xbd:
					key=6;
					break;
				
				case 0x7d:
					key=7;
					break;
			}
			while(temp!=0xf0)//检测按键是否松开，没松开则一直检测
			{
				temp=P3;
				temp=temp&0xf0;
			}
			display(key);//等按键松开后，再显示数字
		}
	}
	
	/*检测第三行*/
	P3=0xfb;//第三行为低电平
	temp=P3;
	temp=temp&0xf0;//转换为列电平
	if(temp!=0xf0)//第一次检测按键是否按下
	{
		delay_ms(10);//延时消抖
		temp=P3;
		temp=temp&0xf0;
		if(temp!=0xf0)//第二次检测
		{
			temp=P3;
			switch(temp)
			{
				case 0xeb:
					key=8;
					break;
				
				case 0xdb:
					key=9;
					break;
				
				case 0xbb:
					key=10;
					break;
				
				case 0x7b:
					key=11;
					break;
			}
			while(temp!=0xf0)//检测按键是否松开，没松开则一直检测
			{
				temp=P3;
				temp=temp&0xf0;
			}
			display(key);//等按键松开后，再显示数字
		}
	}
	
	/*检测第四行*/
	P3=0xf7;//第四行为低电平
	temp=P3;
	temp=temp&0xf0;//转换为列电平
	if(temp!=0xf0)//第一次检测按键是否按下
	{
		delay_ms(10);//延时消抖
		temp=P3;
		temp=temp&0xf0;
		if(temp!=0xf0)//第二次检测
		{
			temp=P3;
			switch(temp)
			{
				case 0xe7:
					key=12;
					break;
				
				case 0xd7:
					key=13;
					break;
				
				case 0xb7:
					key=14;
					break;
				
				case 0x77:
					key=15;
					break;
			}
			while(temp!=0xf0)//检测按键是否松开，没松开则一直检测
			{
				temp=P3;
				temp=temp&0xf0;
			}
			display(key);//等按键松开后，再显示数字
		}
	}
}

/*主函数*/
int main()
{
	P0=0;//先关闭数码管的所有段选
	dula=1;
	dula=0;
	
	P0=0xc0;//位选中所有数码管
	wela=1;
	wela=0;
	while(1)
	{
		MatrixKeyScan();
	}
	return 0;
}
