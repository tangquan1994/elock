#ifndef __FSMC_H__
#define __FSMC_H__
	#include "stm32f10x_type.h"
	#include "stm32f10x_conf.h"
	#include "stm32f10x_map.h"
	#include "stm32f10x_nvic.h"
	#include "SYS.h"
	#include "USART.h"
	#include "GPIO.h"
	
	/*=================================================*/
	/************************LCD************************/
	/*=================================================*/
	typedef struct
	{
		vu16 LCD_REG;
		vu16 LCD_RAM;
	} LCD_TypeDef;

	/* LCD is connected to the FSMC_Bank1_NOR/SRAM4 and NE4 is used as ship select signal */
	#define LCD_BASE    ((u32)(0x60000000 | 0x0C000000))		//因为使用的是bank1的区域4
	#define LCD         ((LCD_TypeDef *) LCD_BASE)



	/*=================================================*/
	/************************SRAM***********************/
	/*=================================================*/
	#define ExSRAM_BASE		((u32)(0x60000000 | 0x08000000))	//因为使用的是bank1的区域3
	
	
	
	/*=================================================*/
	/************************NAND***********************/
	/*=================================================*/
	typedef struct
	{
		u8 Maker_ID;
		u8 Device_ID;
		u8 Third_ID;
		u8 Fourth_ID;
	}NAND_IDTypeDef;

	typedef struct 
	{
		u16 Zone;
		u16 Block;
		u16 Page;
	} NAND_ADDRESS;

	typedef u8 NAND_IDdata;

	#define NAND_ADR_BASE					((u32)(BANK2_NAND_ADDR | 0x020000))		//FSMC_A17	连接到nand flash的CL线上,即command line
	#define NAND_CMD_BASE					((u32)(BANK2_NAND_ADDR | 0x010000))		//FSMC_A16	连接到nand flash的AL线上,即adress line
	#define NAND_DAT_BASE					((u32)(BANK2_NAND_ADDR | 0x000000))
	#define BANK2_NAND_ADDR				((u32)0x70000000)     

	#define NAND_ADR   *((u8 *) NAND_ADR_BASE)
	#define NAND_CMD   *((u8 *) NAND_CMD_BASE)
	#define NAND_DAT   *((u8 *) NAND_DAT_BASE)
	
	/** 
		* @brief  FSMC NAND memory command  
		*/  
	#define NAND_CMD_AREA_A            ((u8)0x00)
	#define NAND_CMD_AREA_B            ((u8)0x01)
	#define NAND_CMD_AREA_C            ((u8)0x50)
	#define NAND_CMD_AREA_TRUE1        ((u8)0x30)

	#define NAND_CMD_WRITE0            ((u8)0x80)
	#define NAND_CMD_WRITE_TRUE1       ((u8)0x10)

	#define NAND_CMD_ERASE0            ((u8)0x60)
	#define NAND_CMD_ERASE1            ((u8)0xD0)

	#define NAND_CMD_READID            ((u8)0x90)
	#define NAND_CMD_STATUS            ((u8)0x70)
	#define NAND_CMD_LOCK_STATUS       ((u8)0x7A)
	#define NAND_CMD_RESET             ((u8)0xFF)

	/** 
		* @brief  NAND memory status  
		*/  
	#define NAND_VALID_ADDRESS         ((u32)0x00000100)
	#define NAND_INVALID_ADDRESS       ((u32)0x00000200)
	#define NAND_TIMEOUT_ERROR         ((u32)0x00000400)
	#define NAND_BUSY                  ((u32)0x00000000)
	#define NAND_ERROR                 ((u32)0x00000001)
	#define NAND_READY                 ((u32)0x00000040)

	/** 
		* @brief  FSMC NAND memory parameters  
		*/  
	#define NAND_PAGE_SIZE             ((u16)0x0200) /* 512 bytes per page w/o Spare Area */
	#define NAND_BLOCK_SIZE            ((u16)0x0020) /* 32x512 bytes pages per block */
	#define NAND_ZONE_SIZE             ((u16)0x0400) /* 1024 Block per zone */
	#define NAND_SPARE_AREA_SIZE       ((u16)0x0010) /* last 16 bytes as spare area */
	#define NAND_MAX_ZONE              ((u16)0x0004) /* 4 zones of 1024 block */

	/** 
		* @brief  FSMC NAND memory address computation  
		*/  
	#define ADDR_1st_CYCLE(ADDR)       (u8)((ADDR)& 0xFF)               /* 1st addressing cycle */
	#define ADDR_2nd_CYCLE(ADDR)       (u8)(((ADDR)& 0xFF00) >> 8)      /* 2nd addressing cycle */
	#define ADDR_3rd_CYCLE(ADDR)       (u8)(((ADDR)& 0xFF0000) >> 16)   /* 3rd addressing cycle */
	#define ADDR_4th_CYCLE(ADDR)       (u8)(((ADDR)& 0xFF000000) >> 24) /* 4th addressing cycle */   



	/*=================================================*/
	/************************NOR************************/
	/*=================================================*/
	typedef struct
	{
		u16 Manufacturer_Code;
		u16 Device_Code1;
		u16 Device_Code2;
		u16 Device_Code3;
	}NOR_IDTypeDef;

	#define NOR_WRITE(Address, Data)  (*(vu16 *)(Address) = (Data))
	#define ADDR_SHIFT(A)             (Bank1_NOR2_ADDR + (2 * (A)))
	#define Bank1_NOR2_ADDR           ((u32)0x64000000)
	
	void FSMC_LCD_Init(void);
	void FSMC_SRAM_Init(void);
	void FSMC_NAND_Init(void);
	void FSMC_NAND_ReadID(NAND_IDTypeDef* NAND_ID);
	void NAND_Read(u8* buff,u32 add);
	void NAND_Write(u8* buff,u32 add);
	void FSMC_NOR_Init(void);
	void FSMC_NOR_ReadID(NOR_IDTypeDef* NOR_ID);
#endif

