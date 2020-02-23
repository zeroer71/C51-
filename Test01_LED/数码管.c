/*
*************************************************************
*
*		模块名称 ： 数码管模块
*		文件名称 ： 数码管.c
*		版本		 ： v1.0
*		说明		 ： 同时点亮实验板上的6个数码管
*		修改记录 ：
*			  版本号	日期				作者			说明
*				v1.0		2020-01-11	Yang			v1.0版本进行测试
*
*************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
sbit dula=P2^6;
sbit wela=P2^7;
uchar num;
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

int main()
{
	/*先设置位选，选择前6个数码管*/
	wela=1;
	P0=0xc0;
	wela=0;
	while(1)
	{
		for(num=0;num<16;num++)//用16个数循环显示
		{
			dula=1;
			P0=table[num];
			dula=0;
			delay_ms(1000);//延时1000毫秒
		}
	}
	return 0;
}