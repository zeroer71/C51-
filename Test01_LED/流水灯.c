/*
*************************************************************
*
*		模块名称 ： 流水灯模块
*		文件名称 ： 流水灯.c
*		版本		 ： v1.0
*		说明		 ： 循环依次点亮1，3，5，7灯作为流水灯
*		修改记录 ：
*			  版本号	日期				作者			说明
*				v1.0		2020-01-10	Yang			v1.0版本进行测试
*
*************************************************************
*/

#include <reg52.h>
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char

//延时n毫秒
void delay_ms(uint n)
{
	uint i,j;
	for(i=n;i>0;i--)
			for(j=110;j>0;j--);
}

int main()
{	
	uchar led=0xfe;
	while(1)
	{
		P1=led;
		delay_ms(500);
		led=_crol_(led,2);//将led向左循环移动2位
						//C51库函数：循环左移函数_crol_,循环右移函数_cror_
	}
	return 0;
}