#include "os_core.h"

FATFS *SDFs;	//SD卡文件系统结构体指针
extern SD_CardInfo SDCardInfo;

//通过串口打印SD卡相关信息
static void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//制造商ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//卡相对地址
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//显示容量
 	printf("Card BlockSize:%d\r\n",SDCardInfo.CardBlockSize);			//显示块大小
}

void OS_FileInit(void)
{
	//SD卡初始化以及文件系统挂载
	SDFs = new(MemPool,sizeof(FATFS));
	while(f_mount(SDFs, "", 0)!= 0) printf("line:%d\r\n",__LINE__);
	show_sdcard_info();	//打印SD卡相关信息
// 	FONT_Init();
}

//打开文件函数
//如果打开成功则将文件的结构体指针返回
//如果打开失败则返回null指针
FIL* OS_OpenFile(char *path,unsigned char mode)
{
    FIL *file = new(MemPool,sizeof(FIL));
	if(file != null)
	{
		if(f_open(file,path,mode) == FR_OK)
			return file;	//文件打开成功返回文件句柄指针
	}
	free(file);		//否则将分配的内存释放再返回空指针
	return null;
}

//关闭文件函数
//file为文件结构体指针
//关闭文件之后将文件结构体内存释放
void OS_CloseFile(FIL *file)
{
	if(file != null)
	{
		f_close(file);
		free(file);
	}
}

//打开文件夹函数
DIR* OS_OpenDir(char *path)
{
    DIR *dir = new(MemPool,sizeof(DIR));
	if(dir != null)
	{
		if(f_opendir(dir, path) == FR_OK)		/* Open the directory */
			return dir;
	}
	free(dir);
	return null;
}

//关闭文件夹函数
//同时回收文件夹结构体内存
void OS_CloseDir(DIR *dir)
{
	if(dir != null)
	{
		f_closedir(dir);
		free(dir);
	}
}

//打印一个目录里面的内容
//打印模式可以有简洁信息打印和完整信息打印
void OS_PrintDir(char *path,DIR_PRINT_MODE mode)
{
	FRESULT res;
    FILINFO *fno = new(MemPool,sizeof(FILINFO));
    DIR *dir = new(MemPool,sizeof(DIR));

	res = f_opendir(dir, path);		/* Open the directory */
	if(res == FR_OK)
	{
		while(1)
		{
			res = f_readdir(dir, fno);		/* Read a directory item */
			if (res != FR_OK || fno->fname[0] == 0)
				break;
			if(mode == DIR_PRINT_MODE_SIM)
				printf("%s ",fno->fname);
			else
			{
				if(fno->fattrib & AM_DIR)
					printf("D");
				else
					printf("F");
				printf("\t%10s\t%dKB\r\n",fno->fname,fno->fsize / 1024);
			}
		}
	}
	f_closedir(dir);
	//回收内存讲究先分配的后回收/后分配的先会后
	//遵循堆栈式的先进后出原则,这样可以保证将连续的
	//被回收的空内存块合并起来,因为估计内存回收速度
	//合并不是从头开始的,而是从被回收的内存块的位置
	//向后开始回收的,所以最后回收最先分配的内存块可以
	//将所分配的内存块全部合并了
	free(dir);
	free(fno);
}

//读取一个文件的设定偏移量处的给定长度字节的数据
//path为文件的路径且必须是绝对路径
//如果读取字节数长度len为0表示读取文件所有字节数
//如果文件不存在则返回0表示没有读取到数据
//否则如果文件存在则将真实读取到的字节数并返回
int OS_ReadFile2(char *path,char *buff,int offset,int len)
{
	#define BYTES_READ_ONCE		1024
	unsigned int br = 0;
    FIL *file = new(MemPool,sizeof(FIL));
	if(f_open(file,path,FA_READ | FA_OPEN_EXISTING) == FR_OK)
	{
		if(offset > 0)
			f_lseek(file,offset);
		if(len > 0)
		{
			f_read(file,buff,len,&br);
		}
		else
		{
			f_read(file,buff,file->fsize,&br);
			//每次读1KB,没读完继续读取
// 			while(f_read(file,buff,BYTES_READ_ONCE,&br2) == FR_OK)
// 			{
// 				if(br2 == 0)
// 					break;
// 				buff += BYTES_READ_ONCE;
// 				br += br2;
// 			}
		}
		f_close(file);
	}
	free(file);
	return br;
}

//读取一个文件的设定偏移量处的给定长度字节的数据
//file为文件的结构体指针
//如果读取字节数长度len为0表示读取文件所有字节数
//如果文件不存在则返回0表示没有读取到数据
//否则如果文件存在则将真实读取到的字节数并返回
int OS_ReadFile(FIL *file,char *buff,int offset,int len)
{
	#define BYTES_READ_ONCE		1024
	unsigned int br = 0;
	if(offset > 0)
		if(f_lseek(file,offset) != FR_OK)
			return 0;
	//读取长度为0表示读取文件全部数据
	if(len == 0)
		len = file->fsize;
	if(f_read(file,buff,len,&br) != FR_OK)
		return 0;
	return br;
}

//在指定偏移量offset处写入长度为len的数据并返回实际写入的字节数
//file为文件的结构体指针
int OS_WriteFile(FIL *file,char *buff,int offset,int len)
{
	unsigned int bw;
	if(file == null || len == 0 || buff == null)
		return 0;
	if(offset > 0)
		if(f_lseek(file,offset) != FR_OK)
			return 0;
	if(f_write(file,buff,len,&bw) != FR_OK)
		return 0;
	return bw;
}












