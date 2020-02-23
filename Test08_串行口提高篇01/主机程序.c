/*
***********************************************************************************************************************************************
6
*		模块名称 ： 串口模块
*		文件名称 ： 主机程序.c
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
uchar code tbuf[16]={"master transmit"};

/*发送复位信号*/
void error()
{
	SBUF=0xff;
	while(TI!=1);
	TI=0;
}

/**函数:master()
	*功能：主机与从机进行通信，主机传送命令给从机，根据命令的功能判断为发送数据或接收数据
	*输入：从机地址address,命令command
	*输出：0
	*/
uchar master(uchar address,uchar command)
{
	uchar i,temp,pbuf;
	while(1)
	{
		SBUF=SLAVE;//发呼叫地址
		while(TI!=1);//等待主机发送完
		TI=0;
		while(RI!=1);//等待从机应答
		RI=0;
		if(SBUF!=address)//若地址错，则发送复位信号
		{
			error();
		}
		
		else
		{
			TB8=0;//将地址标志清零
			SBUF=command;//发送命令
			while(TI!=1);
			TI=0;
			while(RI!=1);
			RI=0;
			temp=SBUF;
			
			/*从机没收到命令，则RB8=0，复位发送信号*/
			if(temp&&0x08==0x08)
			{
				TB8=1;
				error();
			}
			
			/*从机收到命令，主机开始执行*/
			else
			{
				/*发送数据*/
				if(command==0x01)
				{
					if(temp&&0x01==0x01)//RRDY=1,从机接收就绪
					{
						do
						{
							pbuf=0;//清校验和
							
							/*开始发送16B数据*/
							for(i=0;i<LENGTH;i++)
							{
								SBUF=tbuf[i];//发送一数据
								pbuf+=tbuf[i];
								while(TI!=1);
								TI=0;
							}
							
							/*发送校验和*/
							SBUF=pbuf;
							while(TI!=1);
							TI=0;
							while(RI!=1);
							RI=0;
							
							return 0;
						}while(SBUF!=0);
						
						TB8=1;//发送数据完毕，置地址标志
					}
					return 0;
					
				}
				
				/*接收数据*/
				else if(command==0x02)
				{
					if(temp&&0x02==0x02)//TRDY=1,从机发送就绪
					{
						while(1)
						{
							pbuf=0;//清校验和
							
							for(i=0;i<LENGTH;i++)
							{
								while(RI!=1);
								RI=0;
								rbuf[i]=SBUF;//接收一数据
								pbuf+=rbuf[i];//将用于校验的数据存好
							}
							
							/*接收检验和*/
							while(RI!=1);
							RI=0;
							if(SBUF==pbuf)
							{
								SBUF=0x00;//校验和相同发00
								while(TI!=1);
								TI=0;
								break;
							}
							
							else
							{
								SBUF=0xff;//校验和不同发ff
								while(TI!=1);
								TI=0;
							}
						}
						
						TB8=1;//置地址标志
						return 0;
					}
				}
			
			}
			
		}
	}

}

/*主函数*/
void main()
{
	/*开启定时器1，工作方式2*/
	TMOD=0x20;
	TH1=0xfd;
	TL1=0xfd;
	TR1=1;
	
	PCON=0x00;//功率控制寄存器
	SCON=0xf8;//串行口为方式3
	
	master(SLAVE,0x01);
	master(SLAVE,0x02);
	
	while(1);
}