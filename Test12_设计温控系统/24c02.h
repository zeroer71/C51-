#ifndef __24C02_H
#define __24C02_H

#define uchar unsigned char
#define uint unsigned int

bit  write=0;
sbit SDA=P2^0;
sbit SCL=P2^1;

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

#endif
