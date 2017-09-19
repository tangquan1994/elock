#include "Console.h"
#include "string.h"
#include "usart.h"

//��consle����̨����
//����ע��ʽ�ģ�ͨ�������̨������ע��������ʵ���������Ӻ�ɾ��

TCB						*t_console;			//����̨����
Queue					*SerialQueue;		//�����������ݻ�����

//������ָ����������Ϣ����ָ��Ͳ������󲿷�
//ָ����������������Ĳ������͵ı�ʶ��������ĵ�һ������
static unsigned char	*CmdBuff;			//�����������ռ�
static char				*InsBuff;			//ָ���������ռ�
static char				*ParaBuff;			//��������ռ�
#define InstructionCount	(sizeof(InstructionSet) / sizeof(Instruction))
Instruction InstructionSet[] = 
{
	{"st",Ins_st},						//1��ϵͳ״̬
	{"ipconfig",null},					//2��IP����
	{"mem",null},						//3���ڴ�鿴
	{"udp",null},						//4������UDP���ݱ�
	{"new",null},						//5�������ڴ浥Ԫ
	{"free",null},						//6��ɾ���ڴ浥Ԫ
	{"exti",null},						//7���˳�
	{"ss",null},						//13��ͨ�����ڽ�����λ�����������ļ�
};



/**
*
* USART1���жϴ�����
* 
*/
extern TCB *t_1,*t_2,*t_3;
void USART1_IRQHandler(void)
{
	if(USART1->SR & (1<<5)) 	//SR�Ĵ���(status register)�е�RXNE��־λΪ1
	{
		USART1->SR &= ~(1<<5); 	//���RXNE��־λ
// 		UartSendByte(USART1,(u8)USART1->DR);
		if(!EnQueue(SerialQueue,(u8)USART1->DR))
			printf("���ڻ���������!\r\n");
	}
	OS_Resume(t_2);
}

void p_console(void)
{
	unsigned char data;
	unsigned char ptr = 0;
	
	CmdBuff	= new(MemPool,200);				//�����������ռ�
	InsBuff = new(MemPool,20);				//ָ�����ռ�
	ParaBuff = new(MemPool,200);			//��������ռ�
	SerialQueue = newQueue(200);			//�½�FIFO�������
	
	
	printf("\r\n>");	
	while(1)
	{
		if(DeQueue(SerialQueue,&data))
		{
			if(data == Enter)	//�س���
			{
				CmdBuff[ptr] = '\0';	//�����һ���ַ�����ֹͣ��
				Cmd_Analyse(CmdBuff);	//�������ݱ�
				ptr = 0;
			}
			else if(data == BackSpace)	//�˸��
			{
				if(ptr > 0)
					ptr--;
			}
			else
			{
				//backspace/del�Ȳ�����ʾ���ַ�Ҳ�ᱻ¼��
				//�ַ�������ֻ����Ч���ݼ��뵽������
				//��Ч�ַ���' '(32)��'~'(126)֮��
				if(data >= 32 && data <= 126)
					CmdBuff[ptr++] = data;
			}
		}
		OS_Dly(10);
	}
}

//����ָ��Ĳ�������
//��������������ĵ�������ȥָ���ȥ1
static int CmdParameterCount(unsigned char *buff)
{
	return strcntw(buff,' ') - 1;
}

//��ȡָ����ŵĲ���,���Ʋ�����һ���ַ��������Ƿ���Դ�ַ���ָ��
//������id��0�Ļ����ʾҪ��ȡ����ָ��,���ڵ���1��ʾҪ��ȡ���ǲ���
//���û���ҵ��ò����򷵻�false,���򷵻�true
static bool CmdGetParameter(unsigned char *buff,char *para,int id)
{
	unsigned int len;
	unsigned char *temp;
	if((temp = strgetw(buff,' ',id + 1,&len)) != null)
	{
		para[len] = '\0';	//��β����ֹͣ��
		while(len--)
			para[len] = temp[len];
		return true;
	}
	return false;
}

//ָ���������
//idΪ����������
typedef enum{
	INSTRUCTION_UNDEFINED,
	PARAMETER_ERROR,
	PATH_ERROR,
}CMD_ERROR;
static void CmdError(CMD_ERROR id)
{
	switch(id)
	{
		case INSTRUCTION_UNDEFINED	:printf("δ����ָ��");break;
		case PARAMETER_ERROR		:printf("��������");break;
		case PATH_ERROR				:printf("·������");break;
		default						:printf("��������");break;
	}
}


//����ָ��
void Cmd_Analyse(unsigned char *buff)
{
	unsigned int i;
	if(strlen((const char*)buff) != 0)
	{
// 		printf("\r\n");
		//����д��ĸ��ΪСд��ĸ,���ַ�����β�ո�ȥ��
		buff = strtrim(strlwr(buff));
// 		for(i=0;buff[i]!='\0';i++)
// 			UartSendByte(USART1,USART1,buff[i]);
		
		//ʶ���һ��������Ϊָ��,�������Ϊ����
		//�ڶ���������Ϊ��һ�������Դ�����
		if(CmdGetParameter(buff,InsBuff,0))
		{
// 			printf("%s",InsBuff);
			for(i=0;i<InstructionCount;i++)
			{
				if(strcmp(InsBuff,InstructionSet[i].content))
					continue;
// 				printf("���%d:\r\n",i);
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


