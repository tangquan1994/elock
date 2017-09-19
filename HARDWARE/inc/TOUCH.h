#ifndef __TOUCH_H__
#define __TOUCH_H__
	#include "SYS.h"
	#include "USART.h"
	#include "GPIO.h"
	#include "SPI.h"
	#include "GUI.h"
	#include "DRIVE.h"

	#define TOUCH_INT		PGin(7)
	#define TOUCH_CS		PBout(12)
	#define X_AXIS			0
	#define Y_AXIS			1
	//对不同显示屏X、Y坐标不一样
// 	#define X_CHANNEL		0x90
// 	#define Y_CHANNEL		0xD0
	#define X_CHANNEL		0xD0
	#define Y_CHANNEL		0x90

	extern bool IS_CORRECTED;

	void TOUCH_Init(void);
	u16 TOUCH_GetData(u8 channel);
	void TOUCH_WAIT_PRESS(void);
	void TOUCH_WAIT_LOSEN(void);
	bool TOUCH_PRESSED(int *x,int *y);
	void TOUCH_READ(void);
	void TOUCH_ADJ(void);
#endif
