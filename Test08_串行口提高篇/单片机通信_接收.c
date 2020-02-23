/*
***********************************************************************************************************************************************
*
*		模块名称 ： 数码管模块
*		文件名称 ： 单片机通信_发送.c
*		版本		 ： v1.0
*		说明		 ： 接收另一个单片机发送过来的键值，将对应键值以0~F方式显示在数码管上
*		修改记录 ：
*			  版本号		日期				作者			说明
*				v1.0		2020-01-23	Yang			v1.0版本进行测试
*				v1.1		2020-01-23	Yang			增加发送数据的功能
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int

//#define ONLY_RX
#define TX_RX

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

/*发送数据函数*/
void SendData(uchar key_num)
{
	SBUF=key_num;
	while(!TI);//等待发送完成
	TI=0;//发送完成后将TI重新置零，预备下一轮发送
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
			SendData(key);//等按键松开后，再显示数字
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
			SendData(key);//等按键松开后，再显示数字
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
			SendData(key);//等按键松开后，再显示数字
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
			SendData(key);//等按键松开后，再显示数字
		}
	}
}

/*****************仅接收数据**********************/
#ifdef ONLY_RX

/*主函数*/
void main()
{
	/*开启定时器1，设为工作方式2，与发送方一致*/
	TMOD=0x20;
	TL1=0xfe;
	TH1=0xfe;
	TR1=1;
	
	/*设串口工作方式为工作方式1*/
	REN=1;
	SM0=1;
	SM1=0;
	
	/*开启中断*/
	EA=1;
	ES=1;
	
	/*开启数码管的位选*/
	P0=0xc0;//选中所有数码管，6位
	wela=1;
	wela=0;
	
	while(1);//此处不断查询中断
}

/*串口中断服务函数*/
void Serial() interrupt 5
{
	uchar temp;
	RI=0;
	temp=SBUF;
	display(temp);
}

/*****************发送与接收数据**********************/
#ifdef TX_RX

/*主函数*/
void main()
{
	/*开启定时器1，设为工作方式2，可自动重装初值*/
	TMOD=0x20;
	TL1=0xfe;
	TH1=0xfe;
	TR1=1;
	
	/*串口设为工作方式1*/
	SM0=0;
	SM1=1;
	EA=1;
	ES=1;
	REN=1;
	
	while(1)
	{
		ES=0;
		delay_ms(5);
		MatrixKeyScan();
		ES=1;
	}

}

/*串口中断服务函数*/
void Serial() interrupt 5
{
	uchar temp;
	RI=0;
	temp=SBUF;
	display(temp);
}

#endif