/*
***********************************************************************************************************************************************
6
*		模块名称 ： 串口模块
*		文件名称 ： 从机程序.c
*		版本		 ： v1.0
*		说明		 ： 单片机多机通信，约定一次传送的数据为16B,从机地址为02H
*		修改记录 ：
*			  版本号		日期				作者			说明
*				v1.0		2020-01-27	Yang			v1.0版本进行测试
*				
*				
*
***********************************************************************************************************************************************
*/

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
	
#define SLAVE 0x02
#define LENGTH 16
uchar rbuf[16];
uchar tbuf[16];

bit tReady;
bit rReady;

void TX();
void RX();

void main()
{
	/*开启定时器1，工作方式2*/
	TMOD=0x20;
	TH1=0xfd;
	TL1=0xfd;
	TR1=1;
	
	PCON=0x00;//功率控制寄存器
	SCON=0xf8;//串行口工作方式3
	
	/*开启串口中断*/
	EA=1;
	ES=1;
	
	/*从机随时准备发送接收*/
	while(1)
	{
		tReady=1;
		rReady=1;
	}
}

/**函数:SerialIO()
	*功能：从机串口中断服务函数，从机接收主机传送过来的命令，根据命令的功能判断为发送数据或接收数据
	*输入：无
	*输出：无
	*/
void SerialIO() interrupt 4
{
	uchar temp;
	RI=0;
	ES=0;//关闭串口中断
	if(SBUF!=SLAVE)//收到的地址与从机地址不符，继续监听，即重新接收地址
	{
		ES=1;
		goto reti;
	}
	SM2=0;//地址接收正确，取消监听
	
	SBUF=SLAVE;//给主机返回从机地址
	while(TI!=1);
	TI=0;
	
	/****开始接收命令****/
	while(RI!=1);
	RI=0;
	if(RB8==1)//命令传送结束，重新打开串口中断，进行监听
	{
		SM2=1;
		ES=1;
		goto reti;
	}
	temp=SBUF;//接收命令
	
	/****命令为发送数据****/
	if(temp==0x01)
	{
		if(tReady==1)
			SBUF=0x01;//从机做好发送就绪
		else
			SBUF=0x00;//否则返回0
		while(TI!=1);
		TI=0;
		
		/*开始接收主机的应答*/
		while(RI!=1);
		RI=0;
		if(RB8==1)
		{
			SM2=1;
			ES=1;
			goto reti;
		}
		
		/*开始发送数据*/
		TX();
	}
	
	/****命令为接收数据****/
	else if(temp==0x02)
	{
		if(tReady==1)
			SBUF=0x02;
		else
			SBUF=0x00;
		while(TI!=1);
		TI=0;
		
		/*开始接收主机的应答*/
		while(RI!=1);
		RI=0;
		if(RB8==1)
		{
			SM2=1;
			ES=1;
			goto reti;
		}
		
		/*开始接收数据*/
		RX();
	}
	
	/****命令非法****/
	else
	{
		SBUF=0x80;
		while(TI!=1);
		TI=0;
		SM2=1;
		ES=1;//重新监听
	}
	
	reti:;//中断返回
}

/**函数:TX()
	*功能：从机发送数据给主机
	*输入：无
	*输出：无
	*/
void TX()
{
	uchar i,pbuf;
	tReady=0;//将发送就绪位清零
	do
	{
		pbuf=0;
		for(i=0;i<LENGTH;i++)
		{
			SBUF=tbuf[i];
			pbuf+=tbuf[i];
			while(TI!=1);
			TI=0;
		}
		
		/*发送校验和*/
		while(TI!=1);
		TI=0;
		
		/*接收应答*/
		while(RI!=1);
		RI=0;
	}while(SBUF!=0);
	
	/*发送数据完毕，重新监听*/
	SM2=1;
	ES=1;
}

/**函数:RX()
	*功能：从机接收主机的数据
	*输入：无
	*输出：无
	*/
void RX()
{
	uchar i,pbuf;
	rReady=0;
	while(1)
	{
		pbuf=0;
		for(i=0;i<LENGTH;i++)
		{
			while(RI!=1);
			RI=0;
			rbuf[i]=SBUF;
			pbuf+=rbuf[i];//先将要校验的数据存好
		}
		
		/*接收校验和,并比较*/
		if(SBUF==pbuf)
		{
			SBUF=0x00;
			break;
		}
		else
		{
			SBUF=0xff;
			while(TI!=1);
			TI=0;
		}
	}
	
	/*接收数据完毕，重新监听*/
	SM2=1;
	ES=1;
}