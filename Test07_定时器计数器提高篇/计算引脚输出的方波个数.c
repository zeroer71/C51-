/*
***********************************************************************************************************************************************
*
*		模块名称 ： 数码管模块
*		文件名称 ： 计算引脚输出的方波个数.c
*		版本		 ： v1.0
*		说明		 ： （v1.0）利用计数器0的方式1，用一根导线连接GND引脚，另一端去接触T0(P3^4)引脚，每接触一下，计数器计一次数
*								将所计的数值显示在数码管的前两位，计满100时清零，再从头计起
*		修改记录 ：
*			  版本号		日期				作者			说明
*				v1.0		2020-01-22	Yang			v1.0版本进行测试,将导线从GND接到P1^0端口
*				v1.1		2020-01-22  Yang			定时器0进行计时，计数器1进行计数，利用中断读取计数器1的数据，将导线端接到T1(P3^5)引脚
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#define T0_COUNT
#define uchar unsigned char
#define uint unsigned int

sbit dula=P2^6;
sbit wela=P2^7;
sbit port_out=P1^0;

uchar count,NumTotal;
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

/*显示数码管后两位*/
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

/***************v1.0版本，T0作计数器*******************/
#ifdef T0_COUNT

/*读取T0(P3^4)引脚的数据*/
uchar T0_read()
{
	uchar high_1,high_2,low;
	uchar value;
	
	while(1)
	{
		/*端口P1^0以2Hz的频率输出高低电平*/
		port_out=1;
		delay_ms(500);
		port_out=0;
		
		/*防止当发生进位时，很有可能读错数据，因此TH0的寄存器需要读取两次,确保读取的时候没有发生进位*/
		high_1=TH0;
		low=TL0;
		high_2=TH0;
		if(high_1==high_2)//前后两次读取相同确保读取正确再退出
			break;
	}
	
	value=256*high_1+low;
	return value;
}

/*主函数*/
void main()
{
	uchar num1,num2;
	uchar result;
	/*开启计数器0*/
	TMOD=0x05;//设置计数器0工作方式1（00000101）
	TH0=0;
	TL0=0;
	TR0=1;
	while(1)
	{
		result=T0_read();
	 	
		/*计满100清零*/
		if(result>=100)
		{
			result=0;
			TH0=0;
			TL0=0;
		}
		num1=result%10;//个位
		num2=result/10;//十位
		display(num1,num2);
	}

}

/***************v1.1版本，T0作定时器，T1作计数器*******************/
#ifdef T1_COUNT

void main()
{
	/*定时器0设为工作方式2，计数器1设为工作方式1*/
	TMOD=0x51;//工作方式01010010
	TH0=(65536-47852)%256;//装初值
	TL0=(65536-47852)/256;
	TR0=1;
	EA=1;
	ET0=1;
	
	TH1=0;
	TL1=0;
	TR1=1;
	
	while(1)
	{
		if(count>=100)
		{
			count=0;
		}
	}
}

/*定时器0中断服务函数*/
void T1_time() interrupt 1
{
	count++;
	uchar high_1,high_2,low;
	uchar value;
	
	while(1)
	{
		/*防止当发生进位时，很有可能读错数据，因此TH1的寄存器需要读取两次,确保读取的时候没有发生进位*/
		high_1=TH1;
		low=TL1;
		high_2=TH1;
		if(high_1==high_2)//前后两次读取相同确保读取正确再退出
		{
			break;
		}
	}
	value=high_1*256+low;
	display(value%100,value/100);
}
#endif