/*
***********************************************************************************************************************************************
*
*		模块名称 ： 液晶模块12232
*		文件名称 ： 12232液晶动态显示.c
*		版本		 ： v1.0
*		说明		 ： 串行操作方式，实现在12232液晶，从第一行右侧移入"I LOVE WZH,GET!"，同时在第二行右侧移入"我爱傻猪！"
*		修改记录 ：
*			  版本号		日期				作者			说明
*				v1.0		2020-01-15	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
	
sbit LCD_CS=P1^0;
sbit LCD_SID=P1^1;
sbit LCD_SCLK=P1^2;

uchar code str1[]="I LOVE WZH，GET!";
uchar code str2[]="我爱傻猪！";
uchar count;

/*延时函数，延时n毫秒*/
void delay_ms(uint n)
{
	uint i,j;
	for(i=n;i>0;i--)
		for(j=110;j>0;j--);
}

/*发送命令*/
void LCD_send_cmd(uchar cmd_val)
{
	uchar i;
	uchar temp_data;
	
	/******发送命令分三步，第一步发送指令，后两步都是发送8位数据*****/
	/*第一步，发送“写命令”的指令*/
	temp_data=0xf8;
	LCD_CS=1;
	LCD_SCLK=0;
	for(i=0;i<8;i++)
	{
		temp_data=(bit)temp_data&0x80;//取出最高位
		LCD_SCLK=1;
		LCD_SCLK=0;
		temp_data=temp_data<<1;
	}
	
	/*第二步，发送8位指令数据*/
	temp_data=cmd_val;
	temp_data&=0xf0;//取高四位
	for(i=0;i<8;i++)
	{
		temp_data=(bit)temp_data&0x80;//取出最高位
		LCD_SCLK=1;
		LCD_SCLK=0;
		temp_data=temp_data<<1;
	}
	
	/*第三步，发送8位指令数据*/
	temp_data=cmd_val;
	temp_data=temp_data<<4;//取低四位
	for(i=0;i<8;i++)
	{
		temp_data=(bit)temp_data&0x80;//取出最高位
		LCD_SCLK=1;
		LCD_SCLK=0;
		temp_data=temp_data<<1;
	}
	
	LCD_CS=0;
	delay_ms(10);
}

/*发送字符串数据*/
void LCD_send_data(uchar data_val)
{
	uchar i;
	uchar temp_data;
	
	/******发送字符串数据分三步，第一步发送指令，后两步都是发送8位数据*****/
	/*第一步，发送“写数据”的指令*/
	temp_data=0xfa;
	LCD_CS=1;
	LCD_SCLK=0;
	for(i=0;i<8;i++)
	{
		temp_data=(bit)temp_data&0x80;//取出最高位
		LCD_SCLK=1;
		LCD_SCLK=0;
		temp_data=temp_data<<1;
	}
	
	/*第二步，发送8位字符串数据*/
	temp_data=data_val;
	temp_data&=0xf0;//取高四位
	for(i=0;i<8;i++)
	{
		temp_data=(bit)temp_data&0x80;//取出最高位
		LCD_SCLK=1;
		LCD_SCLK=0;
		temp_data=temp_data<<1;
	}
	
	/*第三步，发送8位字符串数据*/
	temp_data=data_val;
	temp_data=temp_data<<4;//取低四位
	for(i=0;i<8;i++)
	{
		temp_data=(bit)temp_data&0x80;//取出最高位
		LCD_SCLK=1;
		LCD_SCLK=0;
		temp_data=temp_data<<1;
	}
	
	LCD_CS=0;
	delay_ms(10);
}

/*初始化12232液晶*/
void Init()
{
	delay_ms(100);
	LCD_send_cmd(0x30);//设置8位数据接口，基本指令模式
	LCD_send_cmd(0x02);//清DDRAM
	LCD_send_cmd(0x06);//游标及显示右移一位
	LCD_send_cmd(0x0c);//整体显示开，游标关，反白关
	LCD_send_cmd(0x30);//写入空格清屏幕
	LCD_send_cmd(0x30);//设定首次显示位置

}

/*显示字符串str1*/
void display_str1(uchar pos)
{
	uchar count;
	LCD_send_cmd(0x88-pos);
	for(count=0;count<15;count++)
	{
		LCD_send_data(str1[count]);
	}

}

/*显示字符串str2*/
void display_str2(uchar pos)
{
	uchar count;
	LCD_send_cmd(0x98-pos);
	for(count=0;count<15;count++)
	{
		LCD_send_data(str2[count]);
	}

}

void main()
{
	uchar position;
	Init();
	for(position=0;position<9;position++)//由于12232液晶没有专门的移屏指令，因此使用for循环来实现移屏效果：															//实际上是重复向不同的地方写入显示字符来实现这种效果
	{																		//实际上是重复向不同的地方写入显示字符
		display_str1(position);
		display_str2(position);
	}
	while(1);

}