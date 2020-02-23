/*
***********************************************************************************************************************************************
*
*		模块名称 ： E2PROM模块AT24C02
*		文件名称 ： 90秒制的秒表.c
*		版本		 ： v1.0
*		说明		 ： 利用定时器产生一个0~99秒变化的秒表，显示在数码管上，并将变化写入AT24C02；
*								当关闭实验板电源，并再次打开电源时，单片机先从AT24C02读取原来写入的数字，接着次数继续变化并显示在数码管上
*		修改记录 ：
*			  版本号		 日期				作者			说明
*				v1.0		2020-01-17	Yang			v1.0版本进行测试
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int

bit  write=0;
sbit SDA=P2^0;
sbit SCL=P2^1;
sbit dula=P2^6;
sbit wela=P2^7;

uchar second,count;//second：秒
/*数码管数字表*/
uchar code table[]={
0x3f,0x06,0x5b,0x4f,
	0x66,0x6d,0x7d,0x07,
	0x7f,0x6f,0x77,0x7c,
	0x39,0x5e,0x79,0x71
};

/*硬件延时函数，延时4~5微秒*/
void delay_us_hard()
{;;}

/*软件延时函数，延时n毫秒*/
void delay_ms_soft(uint n)
{
	uint i,j;
	for(i=n;i>0;i--)
		for(j=110;j>0;j--);
}

/*开始信号,SDA下降沿触发*/
void I2C_start()
{
	SDA=1;
	delay_us_hard();
	SCL=1;
	delay_us_hard();
	SDA=0;
	delay_us_hard();
}

/*停止信号，SDA上升沿触发*/
void I2C_stop()
{
	SDA=0;
	delay_us_hard();
	SCL=1;
	delay_us_hard();
	SDA=1;
	delay_us_hard();
}

/*应答信号，SDA触发上升沿，再触发下降沿*/
void I2C_respond()
{
	uchar i;
	SCL=1;
	delay_us_hard();
	while(SDA==1&&i<250)i++;//等待低电平来，或者一直等待,等待一定次数若无应答则退出
	SCL=0;
	delay_us_hard();
}

/*初始化,SDA=1,SCL=1*/
void I2C_init()
{
	SDA=1;
	delay_us_hard();
	SCL=1;
	delay_us_hard();
}

/*写一个字节*/
void I2C_write_byte(uchar data_val)
{
	uchar i,temp;
	temp=data_val;
	for(i=0;i<8;i++)
	{
		temp=temp<<1;//数据左移一位，最高位移入CY寄存器
		SCL=0;
		delay_us_hard();
		SDA=CY;//将CY寄存器的内容传给数据总线
		delay_us_hard();
		SCL=1;//SCL拉高，启动写数据
		delay_us_hard();
	}
	/*回归原始状态*/
	SCL=0;
	delay_us_hard();
	SDA=1;
	delay_us_hard();
}

/*读一个字节*/
uchar I2C_read_byte()
{
	uchar i,j;
	SCL=0;//先把时钟拉低
	delay_us_hard();
	SDA=1;//把数据拉高
	delay_us_hard();
	for(i=0;i<8;i++)
	{
		SCL=1;//再把时钟拉高，启动读“位”
		delay_us_hard();
		j=(j<<1)|SDA;//j左移一位与SDA进行或运算，依次将8个独立的位赋给j来完成接收
		SCL=0;//最后把时钟拉低，停止读“位”
		delay_us_hard();
	}
	return j;
}

/*指定地址写数据*/
void I2C_write_data(uchar address,uchar data_val)
{
	I2C_start();
	I2C_write_byte(0xa0);//写含片选信号的控制字,进行写操作
	I2C_respond();
	I2C_write_byte(address);//写芯片内的储存器单元子地址
	I2C_respond();
	I2C_write_byte(data_val);
	I2C_respond();
	I2C_stop();
}

/*指定地址读数据*/
uchar I2C_read_data(uchar address)
{
	uchar temp;
	I2C_start();
	I2C_write_byte(0xa0);//写含片选信号的控制字，进行写操作
	I2C_respond();
	I2C_write_byte(address);//写芯片内的存储器单元子地址
	I2C_respond();
	I2C_start();
	I2C_write_byte(0xa1);//写含片选信号的控制字，进行读操作
	I2C_respond();
	temp=I2C_read_byte();
	I2C_stop();
	
	return temp;
}

/*显示数码管后两位*/
void display(uchar ge,uchar shi)
{
	/*显示个位*/
	wela=1;
	P0=0xef;
	wela=0;
	dula=1;
	P0=table[ge];
	dula=0;
	delay_ms_soft(5);
	
	/*显示十位*/
	wela=1;
	P0=0xdf;
	wela=0;
	dula=1;
	P0=table[shi];
	dula=0;
	delay_ms_soft(5);
}

/*主函数*/
void main()
{
	/*初始化I2C并读取E2PPROM的数据*/
	I2C_init();
	second=I2C_read_data(2);
	if(second>100)//如果乱码，则将second置零
	{
		second=0;
	}
	
	/*开启时钟0进行计时中断*/
	TMOD=0x01;//时钟0，工作方式1
	TH0=(65536-50000)/256;//0.05ms一次
	TL0=(65536-50000)%256;
	TR0=1;
	EA=1;
	ET0=1;
	
	/*反复更新显示数码管,到1s时便把秒数写进E2PPROM*/
	while(1)
	{
		display(second/10,second%10);
	
		if(write==1)//到1s
		{
			write=0;
			I2C_write_data(2,second);
		}
	}
}

/*定时器0中断服务函数*/
void T0_time() interrupt 1
{
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	count++;
	if(count==20)//到1s
	{
		count=0;
		write=1;//1s写入E2PPROM
		second++;//秒数增1
		if(second==100)//定时100s，再从零开始计时
		{
			second=0;
		}
	}
}