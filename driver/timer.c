#include  "TIM.h"

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
void Timer_Init(TIM_TypeDef* Timer,u16 arr,u16 psc)
{
	psc -= 1;			//��Ϊ CK_CNT=fCK_PSC/(PSC[15:0]+1);
	arr -= 1;			//��Ϊһֱ���¼�����0,arr�μ����Ļ�ARR�踳ֵarr-1
	RCC_SW(APB1_TIM2,On);	//�򿪶�ʱ��3/4��ʱ��
	Timer->ARR 		= 	arr;      		//�趨�������Զ���װֵ
	Timer->CR1 		|=	1<<4;    			//���¼���
	Timer->PSC 		= 	psc;      		//Ԥ��Ƶ��
	Timer->DIER		|=	1<<0;     		//��������ж�	  
	Timer_Switch(Timer,Off);				//�رն�ʱ��
// 	iNVIC_Init(TIM3_IRQChannel,2,2,Enable);	//��ΪPID�����ж�Դ
}
