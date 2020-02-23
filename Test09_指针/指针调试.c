/**
	*说明：为了进一步了解指针使用仿真器查看各变量和存储器的值
	*作者：Yang
	*时间：2020/1/30
	*/
	
#include <reg52.h>

#define uchar unsigned char
#define uint unsigned int

/*定义16个随机数据*/
uchar code temp[]=
{
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27
};

void main()
{
	uchar i;
	uint j;
	uchar code *finger;//定义基于code区的指针
	do
	{
		finger=&temp[0];//取得数组首地址
		for(i=0;i<16;i++)
		{
			for(j=0;j<3000;j++);//延长一段时间
			P1=*finger;
			finger++;
		}
	}while(1);
}
