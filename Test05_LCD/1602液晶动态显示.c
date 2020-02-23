/*
***********************************************************************************************************************************************
*
*		模块名称 ： 液晶模块1602
*		文件名称 ： 1602液晶动态显示.c
*		版本		 ： v1.0
*		说明		 ： 并行操作方式，实现在1602液晶，从第一行右侧移入"I LOVE WZH,GET!"，同时在第二行右侧移入"WWW.SYY.COM"
*		修改记录 ：
*			  版本号		日期				作者			说明
*				v1.0		2020-01-14	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
	
sbit LCD_EN=P3^4;
sbit LCD_RS=P3^5;
sbit dula=P2^6;
sbit wela=P2^7;

uchar code str1[]="I LOVE WZH!";
uchar code str2[]="LOVE YOU,BABY!";
uchar count;

/*延时函数，延时n毫秒*/
void delay_ms(uint n)
{
	uint i,j;
	for(i=n;i>0;i--)
		for(j=110;j>0;j--);
}

/*写命令函数*/
void LCD_write_cmd(uchar cmd_val)
{
	LCD_RS=0;//选择写命令
	P0=cmd_val;//将要写的命令字送到数据总线上
	delay_ms(5);//稍作延时以待数据稳定
	LCD_EN=1;//使能端给一高脉冲，因为初始化函数已经将LCD_EN=0
	delay_ms(5);//稍作延时
	LCD_EN=0;//将使能端置零，完成高脉冲
}

/*写数据函数*/
void LCD_write_data(uchar data_val)
{
	LCD_RS=1;//选择写数据
	P0=data_val;//将要写的数据送到数据总线上
	delay_ms(5);//稍作延时以待数据稳定
	LCD_EN=1;//使能端给一高脉冲，因为初始化函数已经将LCD_EN=0
	delay_ms(5);//稍作延时
	LCD_EN=0;//将使能端置零，完成高脉冲
}

/*初始化LCD*/
void Init()
{
	dula=0;
	wela=0;//关闭段选、位选
	LCD_EN=0;//先关闭液晶使能端
	LCD_write_cmd(0x38);//设置16 X 2显示，5 X 7点阵，8位数据接口
	LCD_write_cmd(0x0c);//开显示，不显示光标，不闪烁
	LCD_write_cmd(0x06);//写一个字符后指针加1
	LCD_write_cmd(0x01);//显示清0，数据指针清0
}

void main()
{
	/*初始化LCD*/
	Init();
	
	/*先写第一行的数据指针，再写入第一行数据*/
	LCD_write_cmd(0x80+0x10);//将数据写入非显示区域地址处
	
	for(count=0;count<11;count++)
	{
		LCD_write_data(str1[count]);
		delay_ms(5);
	}
	
	/*先写第二行的数据指针，再写入第二行数据*/
	LCD_write_cmd(0x80+0x50);
	
	for(count=0;count<14;count++)
	{
		LCD_write_data(str2[count]);
		delay_ms(5);
	}
	
	for(count=0;count<16;count++)
	{
		LCD_write_cmd(0x18);//将屏幕左移，将数据移入要显示的区域
		delay_ms(200);//间隔200ms左移一个地址
	}
	while(1);
}
