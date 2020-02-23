/*
***********************************************************************************************************************************************
*
*		模块名称 ： 流水灯模块、AD转换模块
*		文件名称 ： D_To_A.c
*		版本		 ： v1.0
*		说明		 ： 单片机控制DAC0832输出电流，让发光二极管D12由灭均匀变到最亮，再由最亮均匀熄灭
*							 
*		修改记录 ：
*			  版本号	  日期				作者			说明
*				v1.0		2020-01-12	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>

#define uchar unsigned char
#define uint unsigned int

sbit dula=P2^6;//申明锁存器U1的锁存端
sbit wela=P2^7;//申明锁存器U2的锁存端
sbit dawr=P3^6;//申明AD转换器的写入端
sbit dacs=P3^2;//申明AD转换器的读取端
sbit beef=P2^3;//申明蜂鸣器端口

/*延时函数，延时n毫秒*/
void delay_ms(uint n)
{
	uint i,j;
	for(i=n;i>0;i--)
		for(j=110;j>0;j--);
}

/*D/A转换*/
int main()
{
	uint flag,DA_Val;
	wela=0;
	dula=0;
	dawr=0;
	dacs=0;
	P0=0;//输入端置零
	
	while(1)
	{
		if(flag==0)
		{
			DA_Val+=5;
			P0=DA_Val;
			if(DA_Val==255)
			{
				flag=1;
				beef=0;//蜂鸣器响
				delay_ms(100);
				beef=1;//蜂鸣器灭
			}
			delay_ms(50);
		}
		else
		{
			DA_Val-=5;
			P0=DA_Val;
			if(DA_Val==0)
			{
				flag=0;
				beef=0;//蜂鸣器响
				delay_ms(100);
				beef=1;//蜂鸣器灭
			}
			delay_ms(50);
		}
	}
	
	return 0;

}