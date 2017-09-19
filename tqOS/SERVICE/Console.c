#include "Console.h"
#include "string.h"
#include "usart.h"

//将consle控制台做好
//做成注册式的，通过向控制台服务器注册命令来实现命令的添加和删除

TCB						*t_console;			//控制台任务
Queue					*SerialQueue;		//串口输入数据缓冲区

//命令是指整条控制信息包括指令和参数两大部分
//指令是描述本条命令的操作类型的标识符是命令的第一个单词
static unsigned char	*CmdBuff;			//命令缓冲区分配空间
static char				*InsBuff;			//指令缓冲区分配空间
static char				*ParaBuff;			//参数分配空间
#define InstructionCount	(sizeof(InstructionSet) / sizeof(Instruction))
Instruction InstructionSet[] = 
{
	{"st",Ins_st},						//1、系统状态
	{"ipconfig",null},					//2、IP配置
	{"mem",null},						//3、内存查看
	{"udp",null},						//4、发送UDP数据报
	{"new",null},						//5、分配内存单元
	{"free",null},						//6、删除内存单元
	{"exti",null},						//7、退出
	{"ss",null},						//13、通过串口接收上位机发送来的文件
};



/**
*
* USART1的中断处理函数
* 
*/
extern TCB *t_1,*t_2,*t_3;
void USART1_IRQHandler(void)
{
	if(USART1->SR & (1<<5)) 	//SR寄存器(status register)中的RXNE标志位为1
	{
		USART1->SR &= ~(1<<5); 	//清除RXNE标志位
// 		UartSendByte(USART1,(u8)USART1->DR);
		if(!EnQueue(SerialQueue,(u8)USART1->DR))
			printf("串口缓冲区已满!\r\n");
	}
	OS_Resume(t_2);
}

void p_console(void)
{
	unsigned char data;
	unsigned char ptr = 0;
	
	CmdBuff	= new(MemPool,200);				//命令缓冲区分配空间
	InsBuff = new(MemPool,20);				//指令分配空间
	ParaBuff = new(MemPool,200);			//参数分配空间
	SerialQueue = newQueue(200);			//新建FIFO缓冲队列
	
	
	printf("\r\n>");	
	while(1)
	{
		if(DeQueue(SerialQueue,&data))
		{
			if(data == Enter)	//回车键
			{
				CmdBuff[ptr] = '\0';	//将最后一个字符换成停止符
				Cmd_Analyse(CmdBuff);	//解析数据报
				ptr = 0;
			}
			else if(data == BackSpace)	//退格键
			{
				if(ptr > 0)
					ptr--;
			}
			else
			{
				//backspace/del等不可显示的字符也会被录入
				//字符过滤器只将有效数据加入到缓冲区
				//有效字符从' '(32)到'~'(126)之间
				if(data >= 32 && data <= 126)
					CmdBuff[ptr++] = data;
			}
		}
		OS_Dly(10);
	}
}

//计算指令的参数个数
//参数个数即命令的单词数减去指令即减去1
static int CmdParameterCount(unsigned char *buff)
{
	return strcntw(buff,' ') - 1;
}

//获取指定编号的参数,复制参数到一个字符串而不是返回源字符串指针
//如果编号id是0的话则表示要获取的是指令,大于等于1表示要获取的是参数
//如果没有找到该参数则返回false,否则返回true
static bool CmdGetParameter(unsigned char *buff,char *para,int id)
{
	unsigned int len;
	unsigned char *temp;
	if((temp = strgetw(buff,' ',id + 1,&len)) != null)
	{
		para[len] = '\0';	//结尾加上停止符
		while(len--)
			para[len] = temp[len];
		return true;
	}
	return false;
}

//指令出错报错函数
//id为错误编号如下
typedef enum{
	INSTRUCTION_UNDEFINED,
	PARAMETER_ERROR,
	PATH_ERROR,
}CMD_ERROR;
static void CmdError(CMD_ERROR id)
{
	switch(id)
	{
		case INSTRUCTION_UNDEFINED	:printf("未定义指令");break;
		case PARAMETER_ERROR		:printf("参数错误");break;
		case PATH_ERROR				:printf("路径错误");break;
		default						:printf("其它错误");break;
	}
}


//解析指令
void Cmd_Analyse(unsigned char *buff)
{
	unsigned int i;
	if(strlen((const char*)buff) != 0)
	{
// 		printf("\r\n");
		//将大写字母换为小写字母,将字符串首尾空格去掉
		buff = strtrim(strlwr(buff));
// 		for(i=0;buff[i]!='\0';i++)
// 			UartSendByte(USART1,USART1,buff[i]);
		
		//识别第一个单词作为指令,后面的作为参数
		//第二个单词作为第一个参数以此类推
		if(CmdGetParameter(buff,InsBuff,0))
		{
// 			printf("%s",InsBuff);
			for(i=0;i<InstructionCount;i++)
			{
				if(strcmp(InsBuff,InstructionSet[i].content))
					continue;
// 				printf("编号%d:\r\n",i);
				if(InstructionSet[i].process != null)
				{
					InstructionSet[i].process(buff);
				}
				break;
			}
			if(i == InstructionCount)
				CmdError(INSTRUCTION_UNDEFINED);
		}
		printf("\r\n>");
	}
	else
		printf(">");
}


void Ins_st(unsigned char *buff)
{
	if(CmdParameterCount(buff) <= 1)
	{
		if(CmdGetParameter(buff,ParaBuff,1))
		{
			if(strlen(ParaBuff) == 1)
			{
				if(ParaBuff[0] == 'c')
				{
					printf("cpu usage:%d%%",(int)OS_CPU_Usage);
					return;
				}
				else if(ParaBuff[0] == 'm')
				{
					OS_MemUsage(MemPool);
					printf("ram usage:%d%%\r\n",OS_MEM_Usage);			
					OS_MemPrint(MemPool);
					return;
				}
				else if(ParaBuff[0] == 't')
				{
					printfTCBList(Idle);
					return;
				}
			}
		}
		else
		{
			OS_MemUsage(MemPool);
			printf("cpu usage:%d%%, ram usage:%d%%",(int)OS_CPU_Usage,(int)OS_MEM_Usage);			
			return;
		}
	}
	else
		CmdError(PARAMETER_ERROR);
}

void Ins_screenshot(unsigned char *buff)
{
}


