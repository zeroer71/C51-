/*
***********************************************************************************************************************************************
*
*		模块名称 ： 数码管模块、AD转换模块、串口模块
*		文件名称 ： 串口打印AD转换值.c
*		版本		 ： v1.0
*		说明		 ： 上位机控制单片机进行AD转换，在数码管的前三位以十进制动态显示出A/D转换的数字量，同时在上位机将这个数值打印出来
*							 （8位二进制A/D转换量，对应十进制0~255）
*		修改记录 ：
*			  版本号	日期				作者			说明
*				v1.0		2020-01-13	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#define uchar unsigned char
#define uint unsigned int

sbit dula=P2^6;//申明锁存器U1的锁存端
sbit wela=P2^7;//申明锁存器U2的锁存端
sbit adwr=P3^6;//申明AD转换器的写入端
sbit adrd=P3^7;//申明AD转换器的读取端

uchar FlagTime,FlagUart,FlagOn,ADval,T0_Time;
float ADo;

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
void display(uchar num)
{
	uchar ge,shi,bai;
	ge=num%10;
	shi=num%100/10;
	bai=num/100;
	
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

/*初始化函数*/
void Init()
{
	/*定时器0为工作方式1，定时器1为工作方式2*/
	TMOD=0x21;
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	TH1=0xfd;
	TL1=0xfd;
	ET0=1;
	TR1=1;
	
	/*串口为工作方式1*/
	SCON=0x50;//比单独对寄存器的位初始化更保险，因为单独对位时需要注意顺序：SM0=0,SM1=1,REN=1,否则结果会出错不稳定
	
	/*开启中断*/
	EA=1;
	ES=1;

}

/*进行AD转换并获取该值*/
uchar GetAD()
{
	uchar ad_val;
	/*AD转换*/
	adwr=1;
	_nop_();
	adwr=0;
	_nop_();
	adwr=1;
	
	P1=0xff;
	
	/*读取转换值*/
	adrd=1;
	_nop_();
	adrd=0;
	_nop_();
	ad_val=P1;
	adrd=1;
	
	return ad_val;
}

/*主函数*/
int main()
{
	/*初始化*/
	Init();
	
	/*AD转换，设置U2存储器的输出最高位为零*/
	wela=1;
	P0=0x7f;
	wela=0;
	
	while(1)
	{
		/*串口接收完毕，判断接收的命令*/
		if(FlagUart==1)
		{
			FlagUart=0;
			ES=0;
			TI=1;
			switch(FlagOn)
			{
				case 0:
					puts("Turn on AD!\n");
					TR0=1;
					break;
				
				case 1:
					printf("Turn off AD！\n");
					TR0=0;
					break;
				
				case 2:
					printf("Error!\n");
					break;
			}
			while(!TI);
			TI=0;
			ES=1;
		}
		
		/*间隔1s发送AD转换值*/
		if(FlagTime==1)
		{
			FlagTime=0;
			ADval=GetAD();
			ADo=(float)ADval*5.0/256.0;
			ES=0;
			TI=1;
			printf("The voltage is %fV\n",ADo);
			while(!TI);
			TI=0;
			ES=1;
		}
		
		/*数码管显示AD转换值*/
		display(ADval);
		
	}
	
	return 0;
}

/*定时器0中断服务函数*/
void T0_time() interrupt 1
{
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	T0_Time++;
	if(T0_Time==20)//到1s
	{
		T0_Time=0;
		FlagTime=1;
	}

}

/*串口中断服务函数*/
void Ser_RxMsg() interrupt 4
{
	uchar temp;
	temp=SBUF;
	RI=0;
	FlagUart=1;
	if(temp==0)
		FlagOn=0;
	else if(temp==1)
		FlagOn=1;
	else
		FlagOn=2;
}