	EXTERN  Idle
	EXTERN  Curr
	EXTERN  Next
	;ARMоƬ�ڵ����Ӻ���ʱ��û��ʹ�ö�ջ����ʹ�������ӼĴ���R14(LR) 
	
	EXPORT	PendSVInit
	EXPORT	PendSV_Handler
	EXPORT	TrigPendSV
	EXPORT	Xiugai
	
NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register.
NVIC_SYSPRI14   EQU     0xE000ED22                              ; System priority register (priority 14).
NVIC_PENDSV_PRI EQU           0xFF                              ; PendSV priority value (lowest).
NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception.

	PRESERVE8													;��ʾ8�ֽڶ���,Ҳ����ʹ��require8αָ��,�����������Ὣ���������8�ֽڶ��� 
	AREA OS_CORE, CODE, READONLY

PendSVInit
	;��ʼ��PendSV�������ж����ȼ�Ϊ���
	LDR     R0, =NVIC_SYSPRI14                                  ; Set the PendSV exception priority
	LDR     R1, =NVIC_PENDSV_PRI
	STRB    R1, [R0]
	BX      LR


;����PendSV�ж�
TrigPendSV
	LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
	LDR     R1, =NVIC_PENDSVSET
	STR     R1, [R0]
	BX      LR


;�޸ļĴ�����ֵ���ڲ���
Xiugai
	LDR		R0 ,=0x00000000
	LDR		R1 ,=0x11111111
	LDR		R2 ,=0x22222222
	LDR		R3 ,=0x33333333

	; MOV		R4 ,#0
	; MOV		R5 ,#0
	; MOV		R6 ,#0
	; MOV		R7 ,#0
	; MOV		R8 ,#0
	; MOV		R9 ,#0
	; MOV		R10,#0
	; MOV		R11,#0
	BX      LR
	

;PendSV�жϴ�����
PendSV_Handler
									;�쳣����ʱ�Ὣ��PC, PSR, R0-3, R12, LR��PSP����ջ
    CPSID   I						;�ر������ж�,��ֹ�л�ʱ�����
	LDR     R1, =Curr				;��ȡ��ǰ����TCBָ��ĵ�ַ
	LDR     R2, [R1]				;��ȡ��ǰ����TCB�ĵ�ַ
	CBZ		R2, skip				;�жϵ�ǰ�����Ƿ�Ϊ��,��Ϊ�ձ�ʾ�ǳ�ʼ���׶�,�Թ������R4-R11�Ĵ����ı���
	
	;�ȱ��汻��ϵ��̵߳�ʣ���R4-R11�Ĵ���,ע��Ҫʹ��PSPջָ��
	MRS     R0, PSP					;��ȡ��ǰ����ϵ������ջָ��
    SUBS    R0, R0, #0x20			;Ԥ��64�ֽڿռ��R4-R11
    STM     R0, {R4-R11}			;����R4-R11
	
	;��ʱ��ǰ����Ļ����Լ����жϽ�PC, PSR, R0-3, R12, LR�Լ��ֶ���R4-R11ȫ����������
	
	STR		R0, [R2]				;���浱ǰ�����ջָ��,����ջָ��Ϊ�ṹ��ĵĵ�һ��Ԫ��,���ַ��������TCB�ĵ�ַ

skip
	;��Next����Curr
	LDR     R0, =Next
	LDR     R0, [R0]
	LDR     R1, =Curr
	STR     R0, [R1]
	
	;�����µ������ջָ��
	LDR     R1, =Next				;��ȡ��һ������TCBָ��ĵ�ַ
    LDR     R0, [R1]				;��ȡ��һ������TCB�ĵ�ַ
    LDR     R0, [R0]				;��ȡ��һ�������ջ��ַ
	
    LDM     R0, {R4-R11}			;����R4-R11�Ĵ�����
    ADDS    R0, R0, #0x20			;ջ��ַ����
    MSR     PSP, R0					;ջ��ַд��PSP
    ORR     LR, LR, #0x04			;ȷ�����غ�ʹ��PSP��Ϊջ�ռ�,����һ�ַ���������һ�ַ�������CONTROL�Ĵ�����bit 2��д1,Ȩ��ָ��Ӣ�İ�P36
									; In the Cortex-M3, the CONTROL[1] bit is always 0 in handler mode. However, in the Thread
									; or base level, it can be either 0 or 1.
									; This bit is writable only when the core is in Thread mode and privileged. In the user state or
									; handler mode, writing to this bit is not allowed. Aside from writing to this register, another
									; way to change this bit is to change bit 2 of the LR when in exception return.
    CPSIE   I						;���ж�
    BX      LR						;�쳣����ʱ�Ὣʣ���PC, PSR, R0-3, R12, LR��PSP�г�ջ
  






	NOP
	END
	