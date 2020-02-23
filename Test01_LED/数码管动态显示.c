/*
*****************************************************************************************************************************************
*
*		模块名称 ： 数码管模块
*		文件名称 ： 数码管动态显示.c
*		版本		 ： v1.0
*		说明		 ： 使实验板上的前6个数码管显示“123456”
*		修改记录 ：
*			  版本号	日期				作者			说明
*				v1.0		2020-01-11	Yang			v1.0版本进行测试
*       v1.1    2020-01-11  Yang      加入条件编译，可选择先传输位数据或段数据
*
*****************************************************************************************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
#define WEI_FIRST
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

/*先传送段选数据*/
#ifndef WEI_FIRST
int main()
{
	while(1)
	{
		/*点亮第一个数码管，显示数字1*/
		dula=1;
		P0=table[1];
		dula=0;
		P0=0xff;//消影
		wela=1;
		P0=0xfe;
		wela=0;
		delay_ms(1);//延时1毫秒，变化极快，得到数字显示保持不变的效果
		
		/*点亮第二个数码管，显示数字2*/
		dula=1;
		P0=table[2];
		dula=0;
		P0=0xff;
		wela=1;
		P0=0xfd;
		wela=0;
		delay_ms(1);
		
		/*点亮第三个数码管，显示数字3*/
		dula=1;
		P0=table[3];
		dula=0;
		P0=0xff;
		wela=1;
		P0=0xfb;
		wela=0;
		delay_ms(1);
		
		/*点亮第四个数码管，显示数字4*/
		dula=1;
		P0=table[4];
		dula=0;
		P0=0xff;
		wela=1;
		P0=0xf7;
		wela=0;
		delay_ms(1);
		
		/*点亮第五个数码管，显示数字5*/
		dula=1;
		P0=table[5];
		dula=0;
		P0=0xff;
		wela=1;
		P0=0xef;
		wela=0;
		delay_ms(1);
		
		/*点亮第六个数码管，显示数字6*/
		dula=1;
		P0=table[6];
		dula=0;
		P0=0xff;
		wela=1;
		P0=0xdf;
		wela=0;
		delay_ms(1);
	}
	return 0;
}

/*先传送位选数据*/
#else
int main()
{
	while(1)
	{
		/*点亮第一个数码管，显示数字"1"*/
		wela=1;
		P0=0xfe;
		wela=0;
		dula=1;
		P0=table[1];
		dula=0;
		delay_ms(1);
		
		/*点亮第二个数码管，显示数字"2"*/
		wela=1;
		P0=0xfd;
		wela=0;
		dula=1;
		P0=table[2];
		dula=0;
		delay_ms(1);
		
		/*点亮第三个数码管，显示数字"3"*/
		wela=1;
		P0=0xfb;
		wela=0;
		dula=1;
		P0=table[3];
		dula=0;
		delay_ms(1);
		
		/*点亮第四个数码管，显示数字"4"*/
		wela=1;
		P0=0xf7;
		wela=0;
		dula=1;
		P0=table[4];
		dula=0;
		delay_ms(1);
		
		/*点亮第五个数码管，显示数字"5"*/
		wela=1;
		P0=0xef;
		wela=0;
		dula=1;
		P0=table[5];
		dula=0;
		delay_ms(1);
		
		/*点亮第六个数码管，显示数字"6"*/
		wela=1;
		P0=0xdf;
		wela=0;
		dula=1;
		P0=table[6];
		dula=0;
		delay_ms(1);
		
	}
	return 0;

}
#endif