/*
***********************************************************************************************************************************************
*
*		模块名称 ： 数码管模块、AD转换模块
*		文件名称 ： A_To_D.c
*		版本		 ： v1.0
*		说明		 ： 单片机控制ADC0804进行模式转换，在数码管的前三位以十进制动态显示出A/D转换的数字量
*							 （8位二进制A/D转换量，对应十进制0~255）
*		修改记录 ：
*			  版本号	日期				作者			说明
*				v1.0		2020-01-12	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int

sbit dula=P2^6;//申明锁存器U1的锁存端
sbit wela=P2^7;//申明锁存器U2的锁存端
sbit adwr=P3^6;//申明AD转换器的写入端
sbit adrd=P3^7;//申明AD转换器的读取端
uchar ge,shi,bai;

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

/*显示数码管后三位*/
void display(uchar ge,shi,bai)
{
	/*显示个位*/
	wela=1;
	P0=0x7e;
	wela=0;
	dula=1;
	P0=table[ge];
	dula=0;
	delay_ms(5);
	
	/*显示十位*/
	wela=1;
	P0=0x7d;
	wela=0;
	dula=1;
	P0=table[shi];
	dula=0;
	delay_ms(5);
	
	/*显示百位*/
	wela=1;
	P0=0x7b;
	wela=0;
	dula=1;
	P0=table[bai];
	dula=0;
	delay_ms(5);
	
}

/*进行A/D转换，并传给数码管显示输出量*/
int main()
{
	uchar ADval,A1,A2,A3;//A1 A2 A3为模拟量的三位
	uchar count;
	wela=1;
	P0=0x7f;//使片选端置为低电平
	wela=0;
	while(1)
	{
		/*进行A/D转换*/
		adwr=1;
		_nop_();//_nop_()相当于一个机器周期的延迟，作为<intrins.h>的库函数
		adwr=0;
		_nop_();
		adwr=1;
		
		/*用数码管显示"000"来延长转换时间*/
		for(count=100;count>0;count--)
		{
			display(A1,A2,A3);
		}
		
		/*进行数据输出*/
		P1=0xff;//先使数码管不显示任何数字
		adrd=1;
		_nop_();
		adrd=0;
		_nop_();
		ADval=P1;
		adrd=1;
		
		A1=ADval/10;//个位
		A2=ADval%100/10;//十位
		A3=ADval%100;//百位
		display(A1,A2,A3);
	}
	return 0;
}	
	
