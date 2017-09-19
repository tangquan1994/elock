#include "os_core.h"

/*****************************************************************
内存池空间的首地址为4字节对齐,每次分配以字为最小分配单位,
使用链表结构组织内存池空间,每一个内存块都有一个对应的内存头
内存头占用一个字的空间,且每个内存头的地址也都是4字节对齐的
因此内存头地址的低两位固定为0,为了节省内存空间,将内存头地址
的低两位作为两个标志位,用于标识内存块是否满,寻址时需要和
0xFFFFFFFC进行与运算过滤掉低两位的数值.内存池的最后一个字
指向本身,以便在遍历时可以有停止的条件.

待优化:
进行内存分配时根据内存池的内存块组成的链表进行遍历,选取空闲
内存块与目的需求的字节总数大小最接近的且大于需求字节数,尽量
避免分割大块内存区域.

设想:
再用一个字的空间反向指向被分配数据的指针,这样在进行内存
空间优化和整合的时候可以进行内存块移动扩展来反向修改被分配
指针的指向地址,这样可以在内存使用率较高的时候或者出现过多的
内存碎片的时候进行内存整理释放内存空间.
函数原型设想: void* new(int size,void **ptr);
ptr为null时表示该内存块不能被内存整理所移动.

*****************************************************************/

unsigned int OS_MEM_Usage;			//内存使用率

//四字节对齐的内存空间,size的单位是字节
void OS_MemInit(unsigned int *mempool,unsigned int size)
{
	//如果长度为0,或者内存池不是四字节对齐的(低两位不为0)则报错
	while(size == 0 || (((unsigned int)mempool & 0x00000003) != 0))
		Debug("mem init error");
		
	//初始化内存池使得首字指向最后一个字
	*mempool = (unsigned int)mempool + (size - 4);
	
	//最后一个字指向自身代表内存池的结束位置,且最后一个字为满
	*((unsigned int *)*mempool) = *mempool | ~OS_MEM_FLG;
	
	//将内存池初始化为空闲
	*mempool &= OS_MEM_FLG;
	
// 	printf("内存池起始地址:");
// 	printf("%X",*mempool);
// 	printf(",大小:%dByte\r\n",size);
// 	OS_MemPrint(mempool);
}

void OS_MemPrint(unsigned int *mempool)
{
	unsigned int *temp = mempool;
	if(mempool == null)
		return;
	printf("start->");
	//如果自身没有指向自身说明没有到达内存池的尾部
	while((*temp & OS_MEM_FLG) != (unsigned int)temp)
	{
		printf("%X",(unsigned int)temp);
		printf("(%d,", (*temp & OS_MEM_FLG) - (unsigned int)temp - 4);
		if(*temp & ~OS_MEM_FLG)
			printf("满) -> ");
		else
			printf("空) -> ");
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

//mempool为内存池地址,lenth为申请的字节数
void *new(unsigned int *mempool,unsigned int lenth)
{
	unsigned int *temp = mempool,temp2;
	if(lenth == 0)
		return null;
	while((*temp & OS_MEM_FLG) != (unsigned int)temp)
	{
		if((*temp & ~OS_MEM_FLG) == 0)	//块空闲
		{
			if((*temp & OS_MEM_FLG) - (unsigned int)temp - 4 > lenth)	//空间足够
			{
				temp2 = *temp;
				*temp = (unsigned int)temp + 4 * (1 + (lenth + 3) / 4);	//如果lenth不是4的整倍数那么将lenth扩大至最接近的4的整倍数
				*temp |= ~OS_MEM_FLG;	//设置为满状态
				*(unsigned int *)(*temp & OS_MEM_FLG) = temp2;
				return temp+1;
			}
		}
		temp = (unsigned int *)(*temp & OS_MEM_FLG);
	}
	return null;
}

//内存回收寒素
//回收内存讲究先分配的后回收/后分配的先会后
//即遵循堆栈式的先进后出原则,这样可以保证将连续的
//被回收的空内存块合并起来,因为顾忌内存回收速度
//合并不是从头开始的,而是从被回收的内存块的位置
//向后开始回收的,所以最后回收最先分配的内存块可以
//将所分配的内存块全部合并了.
//设想:
//可以考虑在回收函数中不添加内存合并函数
//等待所有内存块全部回收完毕之后在对内存池进行一次
//完整的合并,这样可能更好.
void free(void *addr)
{
	unsigned int *temp;
	if(addr == null)
		return;
	temp = (unsigned int *)((unsigned int)addr - 4);
	*temp &= OS_MEM_FLG;
	//合并该内存块和其后面的内存块
	OS_MemMerge(temp);
}

//内存合并,合并指定内存块和下一个空内存块,并递归
//如果block为内存池的首地址则将整个内存池空块合并
void OS_MemMerge(unsigned int *block)
{
	unsigned int *temp = block,*temp1;
	if(block == null)
		return;

	//递归地合并,直到搜索到内存池的结尾为止
	while((*temp & OS_MEM_FLG) != (unsigned int)temp)
	{
		if((*temp & ~OS_MEM_FLG) == 0)	//块空闲
		{
			temp1 = (unsigned int *)(*temp & OS_MEM_FLG);
			if((*temp1 & ~OS_MEM_FLG) == 0)	//如果下一块也空闲那就合并这两块
			{
				*temp = *temp1;
				continue;	//使用continue,不将temp指向下一块内存,判断temp的下一块是否为空
			}
		}
		temp = (unsigned int *)(*temp & OS_MEM_FLG);
	}
}


//还需要有空块合并、内存整理等功能


















