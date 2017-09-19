#ifndef __CONSOLE_H__
#define __CONSOLE_H__
	#include "os_core.h"
	#include "os_lib.h"
	
	extern TCB *t_console;
	
// 	#define BackSpace	127		//退格按键使用的是Delete按键的键值
	#define BackSpace	8
	#define Enter		13
	
	
	//传输控制符
	#define SEND_FILE	1		//1表示发送文件
	#define RECE_FILE	2		//2表示接收文件
	#define SEND_SHOT	3		//3表示发送屏幕截图
	#define P_HEADER	'T'		//T表示帧头
	#define P_TAIL		'Q'		//Q表示帧尾
	#define P_TRANS		'\\'	//转义字符
	
	typedef struct Instruction
	{
		char *content;
		void (*process)(unsigned char *buff);
// 		struct Instruction *next;
	}Instruction;
	
	void p_console(void);
	void Cmd_Analyse(unsigned char *buff);
	void Ins_st(unsigned char *buff);
	void Instruction_ipconfig(unsigned char *buff);
	void Instruction_udp(unsigned char *buff);
	void Instruction_new(unsigned char *buff);
	void Instruction_free(unsigned char *buff);
	void Ins_pwd(unsigned char *buff);
	void Ins_ls(unsigned char *buff);
	void Ins_cd(unsigned char *buff);
	void Ins_cat(unsigned char *buff);
	void Ins_send(unsigned char *buff);
	void Ins_reci(unsigned char *buff);
	void Ins_screenshot(unsigned char *buff);

#endif
