/*
***********************************************************************************************************************************************
*
*		模块名称 ： 数码管模块,E2PROM模块
*		文件名称 ： STC单片机E2PROM应用.c
*		版本		 ： v1.0
*		说明		 ：操作STC单片机自带的E2PROM，存储一组按秒递增的两位数据，并且在数码管上显示，数据每显示一次就写入一次
*							 当关闭实验板电源再开启时，从E2PROM读取数据并显示在数码管上
*	
*						
*		修改记录 ：
*			  版本号		日期				作者			说明
*				v1.0		2020-01-31	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int

/****定义ISP的操作命令****/
#define RdCommand 0x01//读命令
#define PrgCommand 0x02//编程命令
#define EraseCommand 0x03//擦写命令

/****定义常规指示****/
#define Error 1
#define Ok 0
#define WaitTime 0x01//定义CPU的等待时间

/****定义ISP相关寄存器****/
sfr  ISP_DATA=0xe2;
sfr  ISP_ADDRH=0xe3;
sfr  ISP_ADDRL=0xe4;
sfr  ISP_CMD=0xe5;
sfr  ISP_TRIG=0xe6;
sfr  ISP_CONTR=0xe7;

/****定义数码管相关操作****/
sbit dula=P2^6;
sbit wela=P2^7;
uchar code table[]={
	0x3f,0x06,0x5b,0x4f,
	0x66,0x6d,0x7d,0x07,
	0x7f,0x6f,0x77,0x7c,
	0x39,0x5e,0x79,0x71
};

uchar num_ms;

/****延时函数，延时n毫秒****/
void delay_ms(uint n)
{
	uint i,j;
	for(i=n;i>0;i--)
		for(j=110;j>0;j--);
}

/****显示数码管前两位****/
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

/****打开ISP,IAP功能****/
void ISP_IAP_enable()
{
	EA=0;//关中断
	ISP_CONTR=ISP_CONTR&0x18;//书上有这步，不太明白这步的意思
	ISP_CONTR=ISP_CONTR|WaitTime;
	ISP_CONTR=0x80;//ISPEN=1
}

/****关闭ISP,IAP功能****/
void ISP_IAP_disable()
{
	ISP_CONTR=0x00;//ISPEN=0
	ISP_TRIG=0x00;
	EA=1;//开中断
}

/****公用函数****/
void ISP_public()
{
	ISP_IAP_enable();
	ISP_TRIG=0x46;//触发ISP_IAP命令字节1
	ISP_TRIG=0xb9;//触发ISP_IAP命令字节2
	_nop_();
}

/****字节读取****/
uchar byte_read(uint byte_addr)
{
	ISP_ADDRH=(uchar)(byte_addr>>8);//存入地址的8位
	ISP_ADDRL=(uchar)(byte_addr&0x00ff);//存入地址的低8位
	ISP_CMD=ISP_CMD&0xf8;//清除低3位
	ISP_CMD=ISP_CMD|RdCommand;//写入读命令
	ISP_public();//执行命令
	ISP_IAP_disable();
	return ISP_DATA;
}

/****扇区擦除****/
void SectorErase(uint sector_addr)
{
	uint iSectorAddr;
	iSectorAddr=sector_addr&0xfe00;//取扇区地址
	ISP_ADDRH=(uchar)(iSectorAddr>>8);//存入地址的8位
	ISP_ADDRL=0x00;//存入地址的低8位
	ISP_CMD=ISP_CMD&0xf8;//清除低3位
	ISP_CMD=ISP_CMD|EraseCommand;//写入擦除命令
	ISP_public();//执行命令
	ISP_IAP_disable();
}

/****字节写****/
void byte_write(uint byte_addr,uchar byte_data)
{
	ISP_ADDRH=(uchar)(byte_addr>>8);//存入地址的8位
	ISP_ADDRL=(uchar)(byte_addr&0x00ff);//存入地址的低8位
	ISP_CMD=ISP_CMD&0xf8;//清除低3位
	ISP_CMD=ISP_CMD|PrgCommand;//写入字节写命令
	ISP_DATA=byte_data;//写入数据
	ISP_public();//执行命令
	ISP_IAP_disable();
}

/****主函数****/
void main()
{
	uchar num_s;
	
	/*开启定时器0，设为工作方式1*/
	TMOD=0x01;
	TH0=(65536-50000)/256;//装初值，50ms一次
	TL0=(65536-50000)%256;
	TR0=1;
	
	/*开启中断*/
	EA=1;//总中断
	ET0=1;//定时器0中断
	
	/*程序开始时，从E2PROM读取数据*/
	num_s=byte_read(0x2000);
	
	if(num_s>=60)//防止首次上电读取错误
		num_s=0;
	while(1)
	{
		if(num_ms>=20)
		{
			num_ms=0;
			num_s++;
			SectorErase(0x2000);//擦除扇区
			byte_write(0x2000,num_s);//往E2PROM写入数据
			if(num_s==60)
			{
				num_s=0;
			}
		}
		display(num_s%10,num_s/10);
	}
}

/****定时器0中断服务函数****/
void T0_time() interrupt 1
{
	TH0=(65536-50000)/256;//装初值，50ms一次
	TL0=(65536-50000)%256;
	num_ms++;
}