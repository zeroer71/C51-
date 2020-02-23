/**
	*功能：运用指针，对10个整数进行选择排序,从小到大
	*作者：Yang
	*时间：2020/1/28
	*/

#include <stdio.h>

/*交换两个数据*/
void swap(int *p1,int *p2)
{
    int temp;
    temp=*p1;
    *p1=*p2;
    *p2=temp;
}

/*选择排序，从小到大*/
void SelectionSort(int *p,int n)
{
	int i,j,k,temp;

	for(i=0;i<n-1;i++)
	{
		k=i;
		for(j=i+1;j<n;j++)
		{
			if(*(p+j)<*(p+k))
			{
				k=j;
			}
			if(k!=i)
			{
                swap(p+k,p+i);
			}
		}
	}
}

void main()
{
	int i,*p,array[10];
	p=array;

	/*输入数组*/
	printf("Please input 10 numbers:\n");
	for(i=0;i<10;i++)
	{
		scanf("%d",p++);
	}

	/*排序*/
	p=array;
	SelectionSort(p,10);

	/*打印数组*/
	printf("After sort:\n");
	p=array;
	for(i=0;i<10;i++)
	{
		printf("%4d",*(p++));
	}
	printf("\n");

}
