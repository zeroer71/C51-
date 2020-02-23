/*
***********************************************************************************************************************************************
*
*		模块名称：  液晶模块1602，E2PROM模块ATC02,矩阵按键模块
*		文件名称：  按键控制时钟显示.c
*		版本：			v1.0
*		说明：			时间显示在1602液晶上，并按秒实时更新；
*						使用板上的按键随时调节时钟的秒、分、时，S9为功能键，S13为增加键，S17为减少键；
*						断电时AT24C02自动保存显示数据，下次上电时会接着上次的显示继续运行
*	
*						
*		修改记录：
*			    版本			修改时间				作者			修改说明
*				v1.0		2020-02-02			Yang		v1.0版本测试
*				v1.1		2020-02-02			Yang		增加修改日期的功能，按键S8为功能按键，S12为日期增加按键，S16为日期减少按键
*				v1.2		2020-02-03			Yang		增加修改星期的功能，增加温控系统，可以显示当前温度，并且到设定的报警值后蜂鸣器会报警、灯闪烁		
*															第一行显示内容：日期年月日、星期
*															第二行显示内容：时间时分秒、温度
***********************************************************************************************************************************************
*/

#include <reg52.h>
#include "24c02.h"
#define uchar unsigned char
#define uint unsigned int

/****定义液晶显示标志****/
#define FLAG_TIME 0	//时间标志
#define FLAG_DATE 1	//日期标志
 
/****初始化锁存器的锁存端口****/
sbit dula=P2^6;
sbit wela=P2^7;

/****初始化液晶端口****/
sbit lcden=P3^4;
sbit rs=P3^5;

/****初始化矩阵按键端口****/
sbit key_func=P3^0;
sbit key_add=P3^1;
sbit key_sub=P3^2;
sbit key_rd=P3^7; 

/****初始化蜂鸣器端口****/
sbit beep=P2^3;

/****声明温度传感器端口****/
sbit ds=P2^2;//温度传感器信号线

/****声明发光二极管****/
sbit led0=P1^0;
sbit led1=P1^1;
sbit led2=P1^2;
sbit led3=P1^3;

/****声明全局变量****/
uchar count,KeyFuncNum,WeekNum;
char miao,fen,shi;
char day,month,year_fr,year_re;

/****定义温度相关变量****/
uint temp_int;//整型的温度数据
float temp_float;//浮点型的温度数据
uint warn_l1=200;//温度下限值，是温度值乘于10后的结果
uint warn_l2=180;//温度下限值
uint warn_h1=300;//温度上限值
uint warn_h2=320;//温度上限值

/****初始化液晶显示状态****/
uchar code table[]="SUN";

/****延时函数，延时n毫秒****/
void delay_ms(uint n)
{
	uint i,j;
	for(i=n;i>0;i--)
		for(j=110;j>0;j--);
}

/****蜂鸣器报警函数****/
void beep_func()
{
	beep=0;//报警
	delay_ms(100);
	beep=1;//熄灭
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

/****液晶显示时间函数****/
void lcd_display_time(uchar flag,uchar add,uchar date)
{
	uchar ge,shi;
	ge=date%10;
	shi=date/10;
	if (flag==0)//flag==0，传递时间参数
	{
		lcd_write_cmd(0x80+0x40+add);//设置显示位置
		lcd_write_data(0x30+shi);//送去液晶显示十位
		lcd_write_data(0x30+ge);//送去液晶显示个位
	}

	else if (flag==1)//flag==1,传递日期参数
	{
		lcd_write_cmd(0x80+add);//设置显示位置
		lcd_write_data(0x30+shi);//送去液晶显示个位
		lcd_write_data(0x30+ge);//送去液晶显示十位
	}
	
}

/****液晶显示温度函数****/
void lcd_display_temp(uchar add,uint temp_int)
{
	uchar ge,shi,xiao;
	shi=temp_int/100;
	ge=temp_int%100/10;
	xiao=temp_int%100%10;
	
	lcd_write_cmd(0x80+0x40+add);//设置显示位置
	lcd_write_data(0x30+shi);//送去液晶显示十位
	lcd_write_data(0x30+ge);//送去液晶显示个位
	lcd_write_cmd(0x80+0x40+add+3);
	lcd_write_data(0x30+xiao);//送去液晶显示小数部分
}


/****写星期函数****/
void lcd_write_week(uchar week)
{
	lcd_write_cmd(0x80+13);

	switch(week)
	{
		case 1:	lcd_write_data('M');delay_ms(5);
				lcd_write_data('O');delay_ms(5);
				lcd_write_data('N');delay_ms(5);
				break;

		case 2:	lcd_write_data('T');delay_ms(5);
				lcd_write_data('U');delay_ms(5);
				lcd_write_data('E');delay_ms(5);
				break;

		case 3:	lcd_write_data('W');delay_ms(5);
				lcd_write_data('E');delay_ms(5);
				lcd_write_data('N');delay_ms(5);
				break;

		case 4:	lcd_write_data('T');delay_ms(5);
				lcd_write_data('H');delay_ms(5);
				lcd_write_data('U');delay_ms(5);
				break;

		case 5:	lcd_write_data('F');delay_ms(5);
				lcd_write_data('R');delay_ms(5);
				lcd_write_data('I');delay_ms(5);
				break;

		case 6:	lcd_write_data('S');delay_ms(5);
				lcd_write_data('A');delay_ms(5);
				lcd_write_data('T');delay_ms(5);
				break;

		case 7:	lcd_write_data('S');delay_ms(5);
				lcd_write_data('U');delay_ms(5);
				lcd_write_data('N');delay_ms(5);
				break;
	}
}

/****液晶初始化函数****/
void LcdInit()
{
	uchar i;
	
	/*初始化全局变量*/
	count=0;
	KeyFuncNum=0;
	WeekNum=0;
	miao=0;
	fen=0;
	shi=0;

	/*初始化矩阵按键*/
	key_rd=0;//软件将矩阵第4列一端置低用以分解出独立按键

	/*初始化液晶参数*/
	wela=0;
	dula=0;//关闭锁存器的锁存端，防止对液晶显示产生干扰
	lcden=0;
	I2C_init();//初始化E2PROM模块AT24C02,开辟内存
	lcd_write_cmd(0x38);
	lcd_write_cmd(0x0c);
	lcd_write_cmd(0x06);
	lcd_write_cmd(0x01);
	lcd_write_cmd(0x80+13);//设置显示初始坐标
	for(i=0;i<3;i++)//写入液晶上电显示的初始数据
	{
		lcd_write_data(table[i]);
		delay_ms(5);
	}
	
	lcd_write_cmd(0x80+6);//设置液晶第一行的显示位置
	lcd_write_data('-');
	delay_ms(5);
	lcd_write_cmd(0x80+9);
	lcd_write_data('-');
	delay_ms(5);

	lcd_write_cmd(0x80+0x40+4);//设置液晶第二行的显示位置
	lcd_write_data(':');
	delay_ms(5);
	lcd_write_cmd(0x80+0x40+7);
	lcd_write_data(':');
	lcd_write_cmd(0x80+0x40+14);//第15格显示“.”
	lcd_write_data('.');
	delay_ms(5);
	
	day=I2C_read_data(4);//上电时首先从AT24C02读取日期数据
	month=I2C_read_data(5);
	year_re=I2C_read_data(6);
	year_fr=I2C_read_data(7);
	lcd_display_time(FLAG_DATE,10,day);//将数据送往液晶显示
	lcd_display_time(FLAG_DATE,7,month);
	lcd_display_time(FLAG_DATE,4,year_re);
	lcd_display_time(FLAG_DATE,2,year_fr);

	miao=I2C_read_data(1);//上电时首先从AT24C02读取时间数据
	fen=I2C_read_data(2);
	shi=I2C_read_data(3);
	lcd_display_time(FLAG_TIME,8,miao);//将数据送往液晶显示
	lcd_display_time(FLAG_TIME,5,fen);
	lcd_display_time(FLAG_TIME,2,shi);
	
	/*开启定时器0中断*/
	TMOD=0x01;//设为工作方式1
	TH0=(65536-50000)/256;//装初值，50ms刷新一次
	TL0=(65536-50000)%256;
	TR0=1;
	EA=1;
	ET0=1;
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
		lcd_display_temp(12,GetTemp());//用显示温度这个操作起延时作用
	}

	/*取消报警 灯灭*/
	beep=1;
	P1=0xff;
	i=noice;
	while(i--)
	{
		lcd_display_temp(12,GetTemp());//用显示温度这个操作起延时作用
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
			lcd_display_temp(12,GetTemp());
		}
	}
}

/****按键扫描函数****/
void KeyScan()
{
	if (key_func==0)//确认功能键按下
	{
		delay_ms(5);//延时消抖
		if (key_func==0)//再次确认功能键按下
		{
			KeyFuncNum++;
			while(!key_func);//等待按键释放
			beep_func();//每当按键按下释放蜂鸣器，发出滴声

			/*==============控制时间===============*/
			if (KeyFuncNum==1)//第一次按下定位到秒位置
			{
				TR0=0;//关闭定时器0
				lcd_write_cmd(0x80+0x40+8);
				lcd_write_cmd(0x0f);//开启光标闪烁
			}

			if (KeyFuncNum==2)//第二次按下光标闪烁定位到分钟位置
			{
				lcd_write_cmd(0x80+0x40+5);
			}

			if (KeyFuncNum==3)//第三次按下光标闪烁定位到时钟位置
			{
				lcd_write_cmd(0x80+0x40+2);
			}

			/*==============控制日期===============*/
			if (KeyFuncNum==4)//第四次按下定位到“日”位置
			{
				lcd_write_cmd(0x80+10);
			}

			if (KeyFuncNum==5)//第五次按下光标闪烁定位到"月"位置
			{
				lcd_write_cmd(0x80+7);
			}

			if (KeyFuncNum==6)//第六次按下光标闪烁定位到"年"的后半位置
			{
				lcd_write_cmd(0x80+4);
			}

			if (KeyFuncNum==7)//第七次按下光标闪烁定位到"年"的前半位置
			{
				lcd_write_cmd(0x80+2);
			}

			if (KeyFuncNum==8)//第八次按下光标闪烁定位到“星期”位置
			{
				lcd_write_cmd(0x80+13);
			}
			if (KeyFuncNum==9)//第九次按下
			{
				KeyFuncNum=0;
				lcd_write_cmd(0x0c);//取消光标闪烁
				TR0=1;
			}
		}		
	}

	if (KeyFuncNum!=0)//只有功能键被按下，增加键与减少键才有效
	{
		if (key_add==0)//确认增加键被按下
		{
			delay_ms(5);//延时消抖
			if (key_add==0)//再次确认键被按下
			{
				while(!key_add);//等待按键释放
				beep_func();
				
				/*==============控制时间===============*/
				if (KeyFuncNum==1)//功能键被按下一次，光标停在秒的位置
				{
					miao++;
					if (miao==60)//满60后清0
					{
						miao=0;
					}
					lcd_display_time(FLAG_TIME,8,miao);//每调节一次送液晶显示一次
					lcd_write_cmd(0x80+0x40+8);//光标重新回到秒的显示位置
					I2C_write_data(1,miao);//数据改变立即存入E2PROM
				}

				if (KeyFuncNum==2)//功能键被按下两次，光标停在分钟的位置
				{
					fen++;
					if (fen==60)
					{
						fen=0;
					}
					lcd_display_time(FLAG_TIME,5,fen);
					lcd_write_cmd(0x80+0x40+5);//光标重新回到分钟的显示位置
					I2C_write_data(2,fen);//数据改变立即存入E2PROM
				}

				if (KeyFuncNum==3)//功能键被按下三次，光标停在时钟的位置
				{
					shi++;
					if (shi==24)
					{
						shi=0;
					}
					lcd_display_time(FLAG_TIME,2,shi);
					lcd_write_cmd(0x80+0x40+2);//光标重新回到时钟的显示位置
					I2C_write_data(3,shi);//数据改变立即存入E2PROM
				}

				/*==============控制日期===============*/
				if (KeyFuncNum==4)//功能键被按下四次，光标停在“日”的位置
				{
					day++;
					if (day==29)//29后清0,2020年2月为闰月。此处需要根据实际日期来调整，将来可以增加“智能调整”功能
					{
						day=0;
					}
					lcd_display_time(FLAG_DATE,10,day);//每调节一次送液晶显示一次
					lcd_write_cmd(0x80+10);//光标重新回到"日"的显示位置
					I2C_write_data(4,day);//数据改变立即存入E2PROM
				}

				if (KeyFuncNum==5)//功能键被按下五次，光标停在“月”的位置
				{
					month++;
					if (month==12)
					{
						month=0;
					}
					lcd_display_time(FLAG_DATE,7,month);
					lcd_write_cmd(0x80+7);//光标重新回到“月的显示位置
					I2C_write_data(5,month);//数据改变立即存入E2PROM
				}

				if (KeyFuncNum==6)//功能键被按下六次，光标停在“年”后半的位置
				{
					year_re++;
					if (year_re==99)
					{
						year_re=0;
					}
					lcd_display_time(FLAG_DATE,4,year_re);
					lcd_write_cmd(0x80+4);//光标重新回到“年”的后半显示位置
					I2C_write_data(6,year_re);//数据改变立即存入E2PROM
				}

				if (KeyFuncNum==7)//功能键被按下七次，光标停在“年”前半的位置
				{
					year_fr++;
					if (year_fr==99)
					{
						year_fr=0;
					}
					lcd_display_time(FLAG_DATE,2,year_fr);
					lcd_write_cmd(0x80+2);//光标重新回到“年”的前半显示位置
					I2C_write_data(7,year_fr);//数据改变立即存入E2PROM
				}

				if (KeyFuncNum==8)//功能键被按下八次，光标停在“星期”的位置
				{
					WeekNum++;
					if (WeekNum==8)
					{
						WeekNum=1;
					}
					lcd_write_week(WeekNum);
					lcd_write_cmd(0x80+13);//光标重新回到“星期”的显示位置
					I2C_write_data(8,WeekNum);//数据改变立即存入E2PROM
				}
			}
		}

		if (key_sub==0)//减少键被按下
		{
			delay_ms(5);//延时消抖
			if (key_sub==0)//再次确认按键被按下
			{
				while(!key_sub);//等待按键释放
				beep_func();

				/*==============控制时间===============*/
				if (KeyFuncNum==1)//功能键被按下一次，光标停在秒的位置
				{
					miao--;
					if (miao==-1)
					{
						miao=59;
					}
					lcd_display_time(FLAG_TIME,8,miao);
					lcd_write_cmd(0x80+0x40+8);
					I2C_write_data(1,miao);
				}

				if (KeyFuncNum==2)//功能键被按下两次，光标停在分钟的位置
				{
					fen--;
					if (fen==-1)
					{
						fen=59;
					}
					lcd_display_time(FLAG_TIME,5,fen);
					lcd_write_cmd(0x80+0x40+7);
					I2C_write_data(2,fen);
				}

				if (KeyFuncNum==3)//功能键被按下三次，光标停在时钟的位置
				{
					shi--;
					if (shi==-1)
					{
						shi=23;
					}
					lcd_display_time(FLAG_TIME,2,shi);
					lcd_write_cmd(0x80+0x40+2);
					I2C_write_data(3,shi);
				}

				/*==============控制日期===============*/
				if (KeyFuncNum==4)//功能键被按下四次，光标停在“日”的位置
				{
					day++;
					if (day==29)//29后清0,2020年2月为闰月。此处需要根据实际日期来调整，将来可以增加“智能调整”功能
					{
						day=0;
					}
					lcd_display_time(FLAG_DATE,10,day);//每调节一次送液晶显示一次
					lcd_write_cmd(0x80+10);//光标重新回到"日"的显示位置
					I2C_write_data(4,day);//数据改变立即存入E2PROM
				}

				if (KeyFuncNum==5)//功能键被按下五次，光标停在“月”的位置
				{
					month++;
					if (month==12)
					{
						month=0;
					}
					lcd_display_time(FLAG_DATE,7,month);
					lcd_write_cmd(0x80+7);//光标重新回到“月的显示位置
					I2C_write_data(5,month);//数据改变立即存入E2PROM
				}

				if (KeyFuncNum==6)//功能键被按下六次，光标停在“年”后半的位置
				{
					year_re++;
					if (year_re==99)
					{
						year_re=0;
					}
					lcd_display_time(FLAG_DATE,4,year_re);
					lcd_write_cmd(0x80+4);//光标重新回到“年”的后半显示位置
					I2C_write_data(6,year_re);//数据改变立即存入E2PROM
				}

				if (KeyFuncNum==7)//功能键被按下七次，光标停在“年”前半的位置
				{
					year_fr++;
					if (year_fr==99)
					{
						year_fr=0;
					}
					lcd_display_time(FLAG_DATE,2,year_fr);
					lcd_write_cmd(0x80+2);//光标重新回到“年”的前半显示位置
					I2C_write_data(7,year_fr);//数据改变立即存入E2PROM
				}

				if (KeyFuncNum==8)//功能键被按下八次，光标停在“星期”的位置
				{
					WeekNum--;
					if (WeekNum==0)
					{
						WeekNum=7;
					}
					lcd_write_week(WeekNum);
					lcd_write_cmd(0x80+13);//光标重新回到“星期”的显示位置
					I2C_write_data(8,WeekNum);//数据改变立即存入E2PROM
				}
			}
		}

	}
}

/****主函数****/
void main()
{
	uchar i;
	
	/*初始化液晶1602*/
	LcdInit();

	/*主循环*/
	while(1)
	{
		/*扫描按键*/
		KeyScan();

		/*温度转换，数码管、液晶显示温度*/		//出现了乱码问题，仍未解决
		//TempChange();
		//for (i = 0; i < 10; ++i)
		//{
		// 	lcd_display_temp(12,GetTemp());
		// }

		/*处理温度值*/
		//TempDeal(temp_int);

		/*温度显示以延时*/
		// for (i = 0; i < 10; ++i)
		// {
		// 	lcd_display_temp(12,GetTemp());
		// }
	}
}

/****定时器0中断服务函数****/
void T0_time() interrupt 1
{
	TH0=(65536-50000)/256;//再次装初值
	TL0=(65536-50000)%256;
	count++;
	if (count==20)//到1s
	{
		count=0;
		miao++;
		if (miao==60)
		{
			miao=0;
			fen++;
			if (fen==60)
			{
				fen=0;
				shi++;
				if (shi==24)
				{
					shi=0;
					day++;
					if (day==29)
					{
						day=0;
						month++;
						if (month==12)
						{
							month=0;
							year_re++;
							if (year_re==99)
							{
								year_re=0;
								year_fr++;
								if (year_fr==99)
								{
									year_fr=0;
								}
								lcd_display_time(FLAG_DATE,2,year_fr);
								I2C_write_data(7,year_fr);
							}
							lcd_display_time(FLAG_DATE,4,year_re);
							I2C_write_data(6,year_re);
						}
						lcd_display_time(FLAG_DATE,7,month);
						I2C_write_data(5,month);
					}
					lcd_display_time(FLAG_DATE,10,day);
					I2C_write_data(4,day);
				}
				lcd_display_time(FLAG_TIME,2,shi);//小时若变化则送给液晶显示
				I2C_write_data(3,shi);//数据改变则写入E2PROM
			}
			lcd_display_time(FLAG_TIME,5,fen);
			I2C_write_data(2,fen);
		}
		lcd_display_time(FLAG_TIME,8,miao);
		I2C_write_data(1,miao);
	}

}