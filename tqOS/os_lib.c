#include "os_core.h"
#include "os_lib.h"
#include "string.h"

//�ַ����ȽϺ���
//s1��s2ΪԴ�ַ�����Ŀ���ַ����ĵ�ַ
//lenΪ��Ҫ�Ƚ϶೤���ַ�
//����0��ʾ�ȽϽ����ͬ
//����1��ʾ�ַ�������ͬ
//����2��ʾ����
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

//��һ���ַ����Ĵ�д��ĸȫ��ת��Сд��ĸ
unsigned char* strlwr(unsigned char *buff)
{
	unsigned char *temp = buff;
	//�ַ������򷵻�
	if(buff == null)
		return null;
	//��д��ĸ����Ӧ��Сд��ĸС32��0x20
	//���ô�λ���ɽ���д��ĸתΪСд��ĸ
	while(*buff != '\0')
	{
		*buff |= (1 << 5);
		buff++;
	}
	return temp;
}

//ȥ���ַ�������β�ո�ͻ��з��Ȳ�����ʾ�ַ�
unsigned char* strtrim(unsigned char *buff)
{
	unsigned char *temp = buff;
	//�ַ������򷵻�
	if(buff == null)
		return null;
	//����ֻʵ��ȥ����β�ո�
	//ѭ��ȥ���ײ��ո�
	while(*buff != '\0')
	{
		if(*buff == ' ')
			temp++;	//�����ص��ַ���ָ�������һλ
		else
			break;	//��ǰ�ַ����ǿո����ʾ�Ѿ����ײ����пո�ȥ��
		buff++;
	}
	//ѭ��ȥ��β���ո�
	while(*buff != '\0')	//��ָ��ָ�����һ������
		buff++;
	buff--;	//ָ����һλ
	while(*buff == ' ')
		buff--;
	*(++buff) = '\0';	//�����ֹͣ��
	return temp;
}

//�����ַ����е��ʵĸ���
//����Ĭ����Ҫ������βû�пո���ַ���
unsigned int strcntw(unsigned char *buff,unsigned char divide)
{
	unsigned int cnt = 0;
	while(*buff == ' ')	//�����ײ��ո�
		buff++;
	while(*buff != '\0')
	{
		if(*buff == ' ')
		{
			cnt++;
			while(*buff == ' ')	//�����ո�
				buff++;
			continue;
		}
		buff++;
	}
	return ++cnt;	//��ΪԴ�ַ�������β�ո�ȥ����,���һ�����ʺ���û�пո���
					//���Լ�ⲻ�����һ������,�����Ҫ��1֮����ܵõ���ȷ������.
}


//����һ������,�Թ̶�����Ϊ�ֽ��.����Ĭ����Ҫ������βû�пո���ַ���
//buffΪԴ�ַ���
//divideΪ�ֽ��
//numΪ��Ҫ��ȡ�ڼ�������,��1��ʼ����
//����ֵΪ��ȡ���ĵ���λ�õ�ָ��,���û����ȡ���򷵻�null
unsigned char* strgetw(unsigned char *buff,unsigned char divide,unsigned int num,unsigned int *len)
{
	unsigned char *temp = buff,cnt = 0;
	//�ַ������򷵻�
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
			while(*buff == ' ')	//�����ո�
				buff++;
			*len = 0;
			temp = buff;
			continue;
		}
		buff++;
		(*len)++;
	}
	//�����Ҫ��ȡ�����ݺͼ���ֵ��һ��,��ʾ�����˽�����,Ҫ��ȡ�����������һ������,����û�пո���
	if(num == cnt + 1)
		return temp;
// 	if(cnt == 0 && num == cnt + 1)
// 		return temp;
// 	if(*buff == '\0' && num == cnt + 1)
// 		return temp;
	return null; 
}

//���ַ���ת��IP��ַ����
//Ĭ�ϴ���ǰ��û�пո��ҽ�β����ֹͣ�����ַ���
//û�д��󷵻�false,������true
bool IPAddressParse(unsigned char *str,unsigned char *dst)
{
	int i,j = 0,k = 0,m = 0,d;
	//��һ�������ݴ��������������,��ֹ���������޷��ָ�֮ǰ��IP��ַ
	unsigned char ip[4];
	for(i=0;i<4;i++)
	{
		while(str[j] != '.' && str[j++] != '\0');
		//������ֵ�������ݱ��浽Ŀ������
		//sscanf���������Կո�ȷ������ַ�Ϊ�ֽ��������ֵ
		//��������0��ʾ���ִ���,����1��ʾû�д���
		if(!sscanf((char*)&str[k],"%d",(int*)&d))
			return true;
// 		printf("%d ",d);
		if(d > 255 || d < 0)
			return true;
		ip[m++] = d;
		k = ++j;	//�����ַ�'.'ͬʱkָ����һ���ַ�����ʼ
	}
	//���û�г��ִ�����IP��ַ���µ�Ŀ������
	for(i=0;i<4;i++)
		dst[i] = ip[i];
	return false;
}

//16λ�ۼ�У��ͺ���
//bufΪ����������
//lenΪ��Ҫ��͵��ֽ���
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

//UDP���ײ�����α�ײ�����ʵ�ײ�,����У���ʱ��Ҫ���������ײ�
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

