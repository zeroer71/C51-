/*
***********************************************************************************************************************************************
*
*		模块名称 ： 数码管模块，温度传感器模块DS18B20,蜂鸣器模块，发光二极管模块，液晶1602模块
*		文件名称 ： DS18B20温度传感器温控系统.c
*		版本		 ： v1.0
*		说明		 ： 在前三个数码管显示当前采集到的环境温度（0~99.9°C）；
*					温度低于20°C时，蜂鸣器以慢“滴”声报警，P1^0口发光二极管闪烁；
*					温度低于18°C时，蜂鸣器以快“滴”声报警，P1^0、P1^1口两个发光二极管闪烁；
*					温度高于30°C时，蜂鸣器以慢“滴”声报警，P1^2口发光二极管闪烁；
*					温度高于32°C时，蜂鸣器以快“滴”声报警，P1^2、P1^3口两个发光二极管闪烁；
*	
*						
*		修改记录 ：
*			  版本号		日期				作者			说明
*				v1.0		2020-02-03	Yang			v1.0版本进行测试
*				v1.1		2020-02-03	Yang			增加液晶显示的功能，在液晶上显示室内温度值
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#include <stdio.h>
#include "24c02.h"
#define uchar unsigned char
#define uint unsigned int

/****声明温度传感器端口****/
sbit ds=P2^2;//温度传感器信号线

/****声明数码管端口****/
sbit dula=P2^6;//段选
sbit wela=P2^7;//位选

/****声明蜂鸣器端口****/
sbit beep=P2^3;

/****声明发光二极管****/
sbit led0=P1^0;
sbit led1=P1^1;
sbit led2=P1^2;
sbit led3=P1^3;

/****初始化液晶端口****/
sbit lcden=P3^4;
sbit rs=P3^5;

/****初始化液晶显示状态****/
uchar code table_1[]="Temperature:";

/****定义温度相关变量****/
uint temp_int;//整型的温度数据
float temp_float;//浮点型的温度数据
uint warn_l1=200;//温度下限值，是温度值乘于10后的结果
uint warn_l2=180;//温度下限值
uint warn_h1=300;//温度上限值
uint warn_h2=320;//温度上限值

/****定义数码管显示数值****/
uchar code table[]={
	0x3f,0x06,0x5b,0x4f,
	0x66,0x6d,0x7d,0x07,
	0x7f,0x6f,			//不带小数点的0~9编码
	0xbf,0x86,0xdb,0xcf,
	0xe6,0xed,0xfd,0x87,
	0xff,0xef			//带小数点的0~9编码
};

/****延时函数，延时n毫秒****/
void delay_ms(uint n)
{
	uint i,j;
	for(i=n;i>0;i--)
		for(j=110;j>0;j--);
}

/****液晶写命令函数****/
void lcd_write_cmd(uchar cmd)
{
	rs=0;
	lcden=0;
	P0=cmd;
	delay_ms(5);
	lcden=1;
	delay_ms(5);
	lcden=0;
}

/****液晶写数据函数****/
void lcd_write_data(uchar data_val)
{
	rs=1;
	lcden=0;
	P0=data_val;
	delay_ms(5);
	lcden=1;
	delay_ms(5);
	lcden=0;
}

/****液晶显示函数****/
void lcd_display_temp(uchar add,uint temp_int)
{
	uchar ge,shi,xiao;
	shi=temp_int/100;
	ge=temp_int%100/10;
	xiao=temp_int%100%10;
	
	lcd_write_cmd(0x80+add);//设置显示位置
	lcd_write_data(0x30+shi);//送去液晶显示十位
	lcd_write_data(0x30+ge);//送去液晶显示个位
	lcd_write_cmd(0x80+add+3);
	lcd_write_data(0x30+xiao);//送去液晶显示小数部分
}

/****液晶1602初始化函数****/
void LcdInit()
{
	uchar i;

	/*初始化液晶参数*/
	wela=0;
	dula=0;//关闭锁存器的锁存端，防止对液晶显示产生干扰
	lcden=0;
	I2C_init();//初始化E2PROM模块AT24C02,开辟内存
	lcd_write_cmd(0x38);
	lcd_write_cmd(0x0c);
	lcd_write_cmd(0x06);
	lcd_write_cmd(0x01);

	/*写入液晶上电显示的初始内容*/
	lcd_write_cmd(0x80);//第一行第1格显示“Temperature:”
	for(i=0;i<12;i++)
	{
		lcd_write_data(table_1[i]);
		delay_ms(5);
	}

	lcd_write_cmd(0x80+14);//第一行第15格显示“.”
	lcd_write_data('.');
	delay_ms(5);
}

/****DS18B20复位，初始化函数****/
void DsReset()
{
	uint i;
	ds=0;//数据线拉低
	i=103;
	while(i>0)i--;//延时720us左右
	ds=1;//数据线拉高
	i=4;
	while(i>0)i--;//延时30us左右
}

/****读1位数据函数****/
uchar TempReadBit()
{
	uint i;
	bit data_val;
	ds=0;
	i++;//起延时作用
	ds=1;
	i++;
	i++;
	data_val=ds;//从数据线获取数据
	i=8;
	while(i>0)i--;//延时起缓冲作用
	return data_val;
}

/****读1个字节数据函数****/
uchar TempReadByte()
{
	uint i,j,data_val;
	data_val=0;
	for (i = 0; i < 8; ++i)
	{
		j=TempReadBit();
		data_val=(j<<7)|(data_val>>1);
	}
	return data_val;
}

/****向DS18B20写一个字节数据函数****/
void TempWriteByte(uchar data_val)
{
	uint i,j;
	bit TempBit;
	for (i = 0; i < 8; ++i)
	{
		/*逐个获取一个字节的位数据*/
		TempBit=data_val&0x01;
		data_val>>=1;

		/*写1*/
		if (TempBit)
		{
			ds=0;
			j++;j++;
			ds=1;
			j=8;
			while(j>0)j--;
		}

		/*写0*/
		else
		{
			ds=0;
			j=8;
			while(j>0)j--;
			ds=1;
			j++;j++;
		}
	}
}

/****DS18B20获取温度并转换****/
void TempChange()
{
	DsReset();
	delay_ms(1);
	TempWriteByte(0xcc);//跳过ROM命令，因为只对一个DS18B20操作，不需要读取ROM编码与匹配ROM编码
	TempWriteByte(0x44);//温度转换命令
}

/****读取寄存器中存储的温度数据****/
uint GetTemp()
{
	uchar LowWord,HighWord;
	DsReset();
	delay_ms(1);
	TempWriteByte(0xcc);
	TempWriteByte(0xbe);//读取暂存器命令
	LowWord=TempReadByte();//低八位
	HighWord=TempReadByte();//高八位

	/*获取整型温度值*/
	temp_int=HighWord;
	temp_int<<=8;
	temp_int=temp_int|LowWord;//两个字节合为一个字

	/*获取浮点型温度值*/
	temp_float=temp_int*0.0625;//温度在寄存器中为12位，分辨率为0.0625°
	temp_int=temp_float*10+0.5;
	temp_float=temp_float+0.05;

	return temp_int;//返回整型温度值
}

/****数码管显示函数****/
void display(uchar num,uchar data_val)	//num为第几个数码管，data_val为温度值
{
	uchar i;
	
	/*开启段选*/
	dula=0;
	P0=table[data_val];
	dula=1;
	dula=0;
	
	/*开启位选*/
	wela=0;
	i=0xff;//消影
	i=i&(~((0x01)<<(num)));//用i来存储位选数据，只有一位为0
	P0=i;
	wela=1;
	wela=0;
	delay_ms(1);
}

/****数码管显示温度值****/
void TempDisplay(uint temp_int)
{
	uchar i;

	/*第一个数码管显示十位*/
	i=temp_int/100;
	display(0,i);

	/*第二个数码管显示个位*/
	i=temp_int%100/10;
	display(1,i+10);//加小数点

	/*第三个数码管显示小数位*/
	i=temp_int%100%10;
	display(2,i);
}

/****蜂鸣器报警，灯闪烁函数****/
void TempWarn(uchar noice,uchar led)	//noice为报警音调，led为二极管
{
	uchar i;
	i=noice;

	/*报警 灯亮*/
	beep=0;//蜂鸣器报警
	P1=~(led);//控制相应的灯亮
	while(i--)
	{
		TempDisplay(GetTemp());//用显示温度这个操作起延时作用
	}

	/*取消报警 灯灭*/
	beep=1;
	P1=0xff;
	i=noice;
	while(i--)
	{
		TempDisplay(GetTemp());//用显示温度这个操作起延时作用
	}
}

/****温度处理函数****/
void TempDeal(uint temp_int)
{
	uchar i;

	/*温度达到报警值，进行报警处理*/
	if ((temp_int>warn_l2)&&(temp_int<=warn_l1))//温度大于18°，小于20°
	{
		TempWarn(40,0x01);//第一个灯亮，蜂鸣器长“滴”
	}

	else if (temp_int<=warn_l2)//温度小于18°
	{
		TempWarn(10,0x03);//第一、二个灯亮，蜂鸣器短“滴”
	}

	else if ((temp_int>warn_h1)&&(temp_int<=warn_h2))//温度大于30°，小于32°
	{
		TempWarn(40,0x04);//第三个灯亮，蜂鸣器长“滴”
	}

	else if (temp_int>=warn_h2)//温度大于32°
	{
		TempWarn(10,0x0c);//第三、四个灯亮，蜂鸣器短“滴”
	}

	/*温度未达到报警值，不作报警处理，正常显示温度*/
	else
	{
		i=40;
		while(i--)
		{
			TempDisplay(GetTemp());
		}
	}
}

/****串口初始化函数****/
void SerialInit()
{
	TMOD=0x20;//设定串口1，工作方式2
	PCON=0x00;
	SCON=0x50;
	TH1=0xfd;
	TL1=0xfd;//波特率9600
	TR1=1;//开启串口
}

/****串口数据发送函数****/
void SerialSendData(uchar *data_ptr)
{
	do
	{
		SBUF=*data_ptr++;
		while(!TI);//等待发送完成
		TI=0;
	}while(*data_ptr);//保持循环直到字符为'\0'
}

/****主函数****/
void main()
{
	uchar buff[4];
	uchar i;

	/*初始化液晶*/
	LcdInit();
	
	/*初始化串口*/
	SerialInit();

	while(1)
	{	
		/*温度转换，数码管、液晶显示温度*/
		TempChange();
		for (i = 0; i < 10; ++i)
		{
			lcd_display_temp(12,GetTemp());//液晶与数码管公用P0口，造成了数码管前两位不断闪烁的现象
			delay_ms(100);
			TempDisplay(GetTemp());
		}

		/*处理温度值*/
		TempDeal(temp_int);

		/*串口发送数据*/
		sprintf(buff,"%f",temp_float);//将浮点型温度格式转换为字符型
		for (i = 0; i < 10; ++i)
		{
			TempDisplay(GetTemp());//温度显示
		}
		SerialSendData(buff);

		/*温度显示以延时*/
		for (i = 0; i < 10; ++i)
		{
			TempDisplay(GetTemp());
		}
	}
	
}
