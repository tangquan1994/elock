#include "os_core.h"

/*****************************************************************
�ڴ�ؿռ���׵�ַΪ4�ֽڶ���,ÿ�η�������Ϊ��С���䵥λ,
ʹ������ṹ��֯�ڴ�ؿռ�,ÿһ���ڴ�鶼��һ����Ӧ���ڴ�ͷ
�ڴ�ͷռ��һ���ֵĿռ�,��ÿ���ڴ�ͷ�ĵ�ַҲ����4�ֽڶ����
����ڴ�ͷ��ַ�ĵ���λ�̶�Ϊ0,Ϊ�˽�ʡ�ڴ�ռ�,���ڴ�ͷ��ַ
�ĵ���λ��Ϊ������־λ,���ڱ�ʶ�ڴ���Ƿ���,Ѱַʱ��Ҫ��
0xFFFFFFFC������������˵�����λ����ֵ.�ڴ�ص����һ����
ָ����,�Ա��ڱ���ʱ������ֹͣ������.

���Ż�:
�����ڴ����ʱ�����ڴ�ص��ڴ����ɵ�������б���,ѡȡ����
�ڴ����Ŀ��������ֽ�������С��ӽ����Ҵ��������ֽ���,����
����ָ����ڴ�����.

����:
����һ���ֵĿռ䷴��ָ�򱻷������ݵ�ָ��,�����ڽ����ڴ�
�ռ��Ż������ϵ�ʱ����Խ����ڴ���ƶ���չ�������޸ı�����
ָ���ָ���ַ,�����������ڴ�ʹ���ʽϸߵ�ʱ����߳��ֹ����
�ڴ���Ƭ��ʱ������ڴ������ͷ��ڴ�ռ�.
����ԭ������: void* new(int size,void **ptr);
ptrΪnullʱ��ʾ���ڴ�鲻�ܱ��ڴ��������ƶ�.

*****************************************************************/

unsigned int OS_MEM_Usage;			//�ڴ�ʹ����

//���ֽڶ�����ڴ�ռ�,size�ĵ�λ���ֽ�
void OS_MemInit(unsigned int *mempool,unsigned int size)
{
	//�������Ϊ0,�����ڴ�ز������ֽڶ����(����λ��Ϊ0)�򱨴�
	while(size == 0 || (((unsigned int)mempool & 0x00000003) != 0))
		Debug("mem init error");
		
	//��ʼ���ڴ��ʹ������ָ�����һ����
	*mempool = (unsigned int)mempool + (size - 4);
	
	//���һ����ָ����������ڴ�صĽ���λ��,�����һ����Ϊ��
	*((unsigned int *)*mempool) = *mempool | ~OS_MEM_FLG;
	
	//���ڴ�س�ʼ��Ϊ����
	*mempool &= OS_MEM_FLG;
	
// 	printf("�ڴ����ʼ��ַ:");
// 	printf("%X",*mempool);
// 	printf(",��С:%dByte\r\n",size);
// 	OS_MemPrint(mempool);
}

void OS_MemPrint(unsigned int *mempool)
{
	unsigned int *temp = mempool;
	if(mempool == null)
		return;
	printf("start->");
	//�������û��ָ������˵��û�е����ڴ�ص�β��
	while((*temp & OS_MEM_FLG) != (unsigned int)temp)
	{
		printf("%X",(unsigned int)temp);
		printf("(%d,", (*temp & OS_MEM_FLG) - (unsigned int)temp - 4);
		if(*temp & ~OS_MEM_FLG)
			printf("��) -> ");
		else
			printf("��) -> ");
		temp = (unsigned int *)(*temp & OS_MEM_FLG);
	}
	printf("end\r\n");
}

void OS_MemUsage(unsigned int *mempool)
{
	unsigned int *temp = mempool,free = 0,space = 0,total = 0;
	while((*temp & OS_MEM_FLG) != (unsigned int)temp)
	{
		space = (*temp & OS_MEM_FLG) - (unsigned int)temp - 4;
		total += space;
		if(!(*temp & ~OS_MEM_FLG))
		{
			free += space;
		}
		temp = (unsigned int *)(*temp & OS_MEM_FLG);
	}
// 	printf("%d %d\r\n",total,free);
	OS_MEM_Usage = 100 * (total - free) / total;
}

//mempoolΪ�ڴ�ص�ַ,lenthΪ������ֽ���
void *new(unsigned int *mempool,unsigned int lenth)
{
	unsigned int *temp = mempool,temp2;
	if(lenth == 0)
		return null;
	while((*temp & OS_MEM_FLG) != (unsigned int)temp)
	{
		if((*temp & ~OS_MEM_FLG) == 0)	//�����
		{
			if((*temp & OS_MEM_FLG) - (unsigned int)temp - 4 > lenth)	//�ռ��㹻
			{
				temp2 = *temp;
				*temp = (unsigned int)temp + 4 * (1 + (lenth + 3) / 4);	//���lenth����4����������ô��lenth��������ӽ���4��������
				*temp |= ~OS_MEM_FLG;	//����Ϊ��״̬
				*(unsigned int *)(*temp & OS_MEM_FLG) = temp2;
				return temp+1;
			}
		}
		temp = (unsigned int *)(*temp & OS_MEM_FLG);
	}
	return null;
}

//�ڴ���պ���
//�����ڴ潲���ȷ���ĺ����/�������Ȼ��
//����ѭ��ջʽ���Ƚ����ԭ��,�������Ա�֤��������
//�����յĿ��ڴ��ϲ�����,��Ϊ�˼��ڴ�����ٶ�
//�ϲ����Ǵ�ͷ��ʼ��,���Ǵӱ����յ��ڴ���λ��
//���ʼ���յ�,�������������ȷ�����ڴ�����
//����������ڴ��ȫ���ϲ���.
//����:
//���Կ����ڻ��պ����в�����ڴ�ϲ�����
//�ȴ������ڴ��ȫ���������֮���ڶ��ڴ�ؽ���һ��
//�����ĺϲ�,�������ܸ���.
void free(void *addr)
{
	unsigned int *temp;
	if(addr == null)
		return;
	temp = (unsigned int *)((unsigned int)addr - 4);
	*temp &= OS_MEM_FLG;
	//�ϲ����ڴ����������ڴ��
	OS_MemMerge(temp);
}

//�ڴ�ϲ�,�ϲ�ָ���ڴ�����һ�����ڴ��,���ݹ�
//���blockΪ�ڴ�ص��׵�ַ�������ڴ�ؿտ�ϲ�
void OS_MemMerge(unsigned int *block)
{
	unsigned int *temp = block,*temp1;
	if(block == null)
		return;

	//�ݹ�غϲ�,ֱ���������ڴ�صĽ�βΪֹ
	while((*temp & OS_MEM_FLG) != (unsigned int)temp)
	{
		if((*temp & ~OS_MEM_FLG) == 0)	//�����
		{
			temp1 = (unsigned int *)(*temp & OS_MEM_FLG);
			if((*temp1 & ~OS_MEM_FLG) == 0)	//�����һ��Ҳ�����Ǿͺϲ�������
			{
				*temp = *temp1;
				continue;	//ʹ��continue,����tempָ����һ���ڴ�,�ж�temp����һ���Ƿ�Ϊ��
			}
		}
		temp = (unsigned int *)(*temp & OS_MEM_FLG);
	}
}


//����Ҫ�пտ�ϲ����ڴ�����ȹ���


















