	EXTERN  Idle
	EXTERN  Curr
	EXTERN  Next
	;ARM芯片在调用子函数时并没有使用堆栈而是使用了连接寄存器R14(LR) 
	
	EXPORT	PendSVInit
	EXPORT	PendSV_Handler
	EXPORT	TrigPendSV
	EXPORT	Xiugai
	
NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register.
NVIC_SYSPRI14   EQU     0xE000ED22                              ; System priority register (priority 14).
NVIC_PENDSV_PRI EQU           0xFF                              ; PendSV priority value (lowest).
NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception.

	PRESERVE8													;表示8字节对齐,也可以使用require8伪指令,这样编译器会将汇编程序进行8字节对齐 
	AREA OS_CORE, CODE, READONLY

PendSVInit
	;初始化PendSV并设置中断优先级为最低
	LDR     R0, =NVIC_SYSPRI14                                  ; Set the PendSV exception priority
	LDR     R1, =NVIC_PENDSV_PRI
	STRB    R1, [R0]
	BX      LR


;触发PendSV中断
TrigPendSV
	LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
	LDR     R1, =NVIC_PENDSVSET
	STR     R1, [R0]
	BX      LR


;修改寄存器的值用于测试
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
	

;PendSV中断处理函数
PendSV_Handler
									;异常产生时会将的PC, PSR, R0-3, R12, LR从PSP中入栈
    CPSID   I						;关闭所有中断,防止切换时被打断
	LDR     R1, =Curr				;获取当前任务TCB指针的地址
	LDR     R2, [R1]				;获取当前任务TCB的地址
	CBZ		R2, skip				;判断当前任务是否为空,若为空表示是初始化阶段,略过下面的R4-R11寄存器的保存
	
	;先保存被打断的线程的剩余的R4-R11寄存器,注意要使用PSP栈指针
	MRS     R0, PSP					;获取当前被打断的任务的栈指针
    SUBS    R0, R0, #0x20			;预留64字节空间给R4-R11
    STM     R0, {R4-R11}			;保存R4-R11
	
	;此时当前任务的环境以及由中断将PC, PSR, R0-3, R12, LR以及手动将R4-R11全部保存起来
	
	STR		R0, [R2]				;保存当前任务的栈指针,任务栈指针为结构体的的第一个元素,其地址就是任务TCB的地址

skip
	;将Next赋给Curr
	LDR     R0, =Next
	LDR     R0, [R0]
	LDR     R1, =Curr
	STR     R0, [R1]
	
	;加载新的任务的栈指针
	LDR     R1, =Next				;获取下一个任务TCB指针的地址
    LDR     R0, [R1]				;获取下一个任务TCB的地址
    LDR     R0, [R0]				;获取下一个任务的栈地址
	
    LDM     R0, {R4-R11}			;加载R4-R11寄存器组
    ADDS    R0, R0, #0x20			;栈地址更新
    MSR     PSP, R0					;栈地址写入PSP
    ORR     LR, LR, #0x04			;确保返回后使用PSP作为栈空间,这是一种方法，还有一种方法是向CONTROL寄存器的bit 2中写1,权威指南英文版P36
									; In the Cortex-M3, the CONTROL[1] bit is always 0 in handler mode. However, in the Thread
									; or base level, it can be either 0 or 1.
									; This bit is writable only when the core is in Thread mode and privileged. In the user state or
									; handler mode, writing to this bit is not allowed. Aside from writing to this register, another
									; way to change this bit is to change bit 2 of the LR when in exception return.
    CPSIE   I						;打开中断
    BX      LR						;异常返回时会将剩余的PC, PSR, R0-3, R12, LR从PSP中出栈
  






	NOP
	END
	