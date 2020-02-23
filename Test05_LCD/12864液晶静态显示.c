/*
***********************************************************************************************************************************************
*
*		模块名称 ： 液晶模块12864
*		文件名称 ： 12864液晶静态显示.c
*		版本		 ： v1.0
*		说明		 ： 并行操作方式，实现在12864液晶的第一行显示“0123456789”，并且让每一位数字随机变化，
*								第二行显示“我爱傻猪!”,第三行显示“www.syy.com”,第四行对应第三行显示出下划线
*		修改记录 ：
*			  版本号		日期				作者			说明
*				v1.0		2020-01-15	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#include <stdlib.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int

/*端口定义*/
#define LCD_data=P0;//数据口
sbit LCD_EN=P3^4;//液晶使能控制
sbit LCD_RS=P3^5;//寄存器选择输入，0为输入指令，1为输入显示数据
sbit LCD_RW=P3^6;//液晶读/写控制
sbit LCD_PSB=P3^7;//串/并行方式控制，0为串行，1为并行

uchar str1[10];
uchar code str2[]="我爱傻猪！";
uchar code str3[]={"www.syy.com"};
uchar code str4[]={"------------"};

/*延时函数，延时n毫秒*/
void delay_ms(uint n)
{
	uint i,j;
	for(i=n;i>0;i--)
		for(j=110;j>0;j--);
}

/*发送指令数据到LCD*/
/*LCD_RS=L,LCD_RW=L,LCD_EN=高脉冲，D0-D7=指令码*/
void LCD_write_cmd(uchar cmd_val)
{
	LCD_RS=0;
	LCD_RW=0;
	LCD_EN=0;
	P0=cmd_val;//将指令数据送到数据总线
	delay_ms(5);
	LCD_EN=1;
	delay_ms(5);
	LCD_EN=0;
}

/*发送显示数据到LCD*/
/*LCD_RS=H,LCD_RW=L,LCD_EN=高脉冲，D0-D7=指令码*/
void LCD_write_data(uchar data_val)
{
	LCD_RS=1;
	LCD_RW=0;
	LCD_EN=0;
	P0=data_val;//将指令数据送到数据总线
	delay_ms(5);
	LCD_EN=1;
	delay_ms(5);
	LCD_EN=0;
}

/*确定显示坐标*/
void LCD_SetPos(uchar x,uchar y)
{
	uchar pos;
	if(x==0)
		x=0x80;
	else if(x==1)
		x=0x90;
	else if(x==2)
		x=0x88;
	else if(x==3)
		x=0x98;
	pos=x+y;
	
	LCD_write_cmd(pos);
}

/*生成随机数字*/
void LCD_MakeRand()
{
	uchar num;
	num=rand();
	/*生成高五位*/
	str1[0]=num/10000+0x30;
	str1[1]=num%10000/1000+0x30;
	str1[2]=num%1000/100+0x30;
	str1[3]=num%100/10+0x30;
	str1[4]=num%10+0x30;
	
	num=rand();
	/*生成低五位*/
	str1[5]=num/10000+0x30;
	str1[6]=num%10000/1000+0x30;
	str1[7]=num%1000/100+0x30;
	str1[8]=num%100/10+0x30;
	str1[9]=num%10+0x30;
}

/*初始化液晶参数*/
void LCD_Init()
{
	LCD_PSB=1;//选择并行操作
	LCD_write_cmd(0x30);
	delay_ms(5);
	LCD_write_cmd(0x0c);
	delay_ms(5);
	LCD_write_cmd(0x01);
	delay_ms(5);
}

/*主程序*/
void main()
{
	uchar i;
	delay_ms(5);
	LCD_Init();
	
	/*设置显示位置为第二行的第1个字符*/
	LCD_SetPos(1,0);
	i=0;
	while(str2[i]!='\0')
	{
		LCD_write_data(str2[i]);//显示字符
		i++;
	}
	
	/*设置显示位置为第三行的第1个字符*/
	LCD_SetPos(2,0);
	i=0;
	while(str3[i]!='\0')
	{
		LCD_write_data(str3[i]);//显示字符
		i++;
	}
	
	/*设置显示位置为第四行的第1个字符*/
	LCD_SetPos(3,0);
	i=0;
	while(str4[i]!='\0')
	{
		LCD_write_data(str4[i]);//显示字符
		i++;
	}
	
	/*设置显示位置为第一行的第1个字符*/
	while(1)
	{
		LCD_SetPos(0,0);
		i=0;
		LCD_MakeRand();
		for(i=0;i<10;i++)
		{
			LCD_write_data(str1[i]);
		}
	}
}