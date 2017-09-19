#include "os_core.h"
#include "os_lib.h"
#include "string.h"

//字符串比较函数
//s1和s2为源字符串和目的字符串的地址
//len为需要比较多长的字符
//返回0表示比较结果相同
//返回1表示字符串不相同
//返回2表示出错
int strcmp2(unsigned char *s1,unsigned char *s2,int len)
{
	int i = len;
	if(s1 == null || s2 == null)
		return 2;
	while(i--)
	{
		if(s1[i] != s2[i])
			return 1;
	}
	return 0;
}

//将一个字符串的大写字母全部转成小写字母
unsigned char* strlwr(unsigned char *buff)
{
	unsigned char *temp = buff;
	//字符串空则返回
	if(buff == null)
		return null;
	//大写字母比相应的小写字母小32即0x20
	//设置此位即可将大写字母转为小写字母
	while(*buff != '\0')
	{
		*buff |= (1 << 5);
		buff++;
	}
	return temp;
}

//去除字符串的首尾空格和换行符等不可显示字符
unsigned char* strtrim(unsigned char *buff)
{
	unsigned char *temp = buff;
	//字符串空则返回
	if(buff == null)
		return null;
	//现在只实现去掉首尾空格
	//循环去掉首部空格
	while(*buff != '\0')
	{
		if(*buff == ' ')
			temp++;	//将返回的字符串指针向后移一位
		else
			break;	//当前字符不是空格则表示已经将首部所有空格去掉
		buff++;
	}
	//循环去掉尾部空格
	while(*buff != '\0')	//将指针指向最后一个数据
		buff++;
	buff--;	//指针退一位
	while(*buff == ' ')
		buff--;
	*(++buff) = '\0';	//最后补上停止符
	return temp;
}

//返回字符串中单词的个数
//函数默认是要处理首尾没有空格的字符串
unsigned int strcntw(unsigned char *buff,unsigned char divide)
{
	unsigned int cnt = 0;
	while(*buff == ' ')	//过掉首部空格
		buff++;
	while(*buff != '\0')
	{
		if(*buff == ' ')
		{
			cnt++;
			while(*buff == ' ')	//过掉空格
				buff++;
			continue;
		}
		buff++;
	}
	return ++cnt;	//因为源字符串的首尾空格被去掉了,最后一个单词后面没有空格了
					//所以检测不到最后一个单词,因此需要加1之后才能得到正确单词数.
}


//返回一个单词,以固定符号为分界符.函数默认是要处理首尾没有空格的字符串
//buff为源字符串
//divide为分界符
//num为需要提取第几个单词,从1开始计数
//返回值为提取到的单词位置的指针,如果没有提取到则返回null
unsigned char* strgetw(unsigned char *buff,unsigned char divide,unsigned int num,unsigned int *len)
{
	unsigned char *temp = buff,cnt = 0;
	//字符串空则返回
	if(buff == null || *buff == '\0')
		return null;
	*len = 0;
	while(*buff != '\0')
	{
		if(*buff == ' ')
		{
			cnt++;
			if(cnt == num)
				return temp;
			while(*buff == ' ')	//过掉空格
				buff++;
			*len = 0;
			temp = buff;
			continue;
		}
		buff++;
		(*len)++;
	}
	//如果需要提取的数据和计数值差一个,表示遇到了结束符,要提取的数据是最后一个数据,后面没有空格了
	if(num == cnt + 1)
		return temp;
// 	if(cnt == 0 && num == cnt + 1)
// 		return temp;
// 	if(*buff == '\0' && num == cnt + 1)
// 		return temp;
	return null; 
}

//将字符串转成IP地址数组
//默认处理前面没有空格且结尾是有停止符的字符串
//没有错误返回false,出错返回true
bool IPAddressParse(unsigned char *str,unsigned char *dst)
{
	int i,j = 0,k = 0,m = 0,d;
	//用一个数组暂存解析出来的数据,防止解析出错无法恢复之前的IP地址
	unsigned char ip[4];
	for(i=0;i<4;i++)
	{
		while(str[j] != '.' && str[j++] != '\0');
		//解析数值并将数据保存到目的数组
		//sscanf函数可以以空格等非数子字符为分界符解析数值
		//函数返回0表示出现错误,返回1表示没有错误
		if(!sscanf((char*)&str[k],"%d",(int*)&d))
			return true;
// 		printf("%d ",d);
		if(d > 255 || d < 0)
			return true;
		ip[m++] = d;
		k = ++j;	//跳过字符'.'同时k指向下一个字符的起始
	}
	//如果没有出现错误则将IP地址跟新到目的数组
	for(i=0;i<4;i++)
		dst[i] = ip[i];
	return false;
}

//16位累加校验和函数
//buf为缓冲区数组
//len为需要求和的字节数
unsigned short CheckSum(unsigned char *buf,unsigned int len)
{
	unsigned int sum = 0;

	// build the sum of 16bit words
	while(len >1){
		sum += 0xFFFF & (*buf<<8|*(buf+1));
		buf+=2;
		len-=2;
	}
	// if there is a byte left then add it (padded with zero)
	if (len)
	{
		sum += (0xFF & *buf)<<8;
	}
	// now calculate the sum over the bytes in the sum
	// until the result is only 16bit long
	while (sum>>16)
	{
		sum = (sum & 0xFFFF)+(sum >> 16);
	}
	// build 1's complement:
	return( (unsigned short) sum ^ 0xFFFF);
}

//UDP的首部包括伪首部和真实首部,计算校验和时需要包括两个首部
unsigned short UDP_CheckSum(unsigned char *buf,unsigned int len)
{
	unsigned int sum = 0;

	sum += 17;
	sum += len-8;

	while(len >1)
	{
		sum += 0xFFFF & (*buf<<8|*(buf+1));
		buf+=2;
		len-=2;
	}
	// if there is a byte left then add it (padded with zero)
	if (len)
	{
		sum += (0xFF & *buf)<<8;
	}
	// now calculate the sum over the bytes in the sum
	// until the result is only 16bit long
	while (sum>>16)
	{
		sum = (sum & 0xFFFF)+(sum >> 16);
	}
	// build 1's complement:
	return( (unsigned short) sum ^ 0xFFFF);
}

