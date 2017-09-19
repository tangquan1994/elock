#ifndef __CONSOLE_H__
#define __CONSOLE_H__
	#include "os_core.h"
	#include "os_lib.h"
	
	extern TCB *t_console;
	
// 	#define BackSpace	127		//�˸񰴼�ʹ�õ���Delete�����ļ�ֵ
	#define BackSpace	8
	#define Enter		13
	
	
	//������Ʒ�
	#define SEND_FILE	1		//1��ʾ�����ļ�
	#define RECE_FILE	2		//2��ʾ�����ļ�
	#define SEND_SHOT	3		//3��ʾ������Ļ��ͼ
	#define P_HEADER	'T'		//T��ʾ֡ͷ
	#define P_TAIL		'Q'		//Q��ʾ֡β
	#define P_TRANS		'\\'	//ת���ַ�
	
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
