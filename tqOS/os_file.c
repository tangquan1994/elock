#include "os_core.h"

FATFS *SDFs;	//SD���ļ�ϵͳ�ṹ��ָ��
extern SD_CardInfo SDCardInfo;

//ͨ�����ڴ�ӡSD�������Ϣ
static void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//������ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//����Ե�ַ
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//��ʾ����
 	printf("Card BlockSize:%d\r\n",SDCardInfo.CardBlockSize);			//��ʾ���С
}

void OS_FileInit(void)
{
	//SD����ʼ���Լ��ļ�ϵͳ����
	SDFs = new(MemPool,sizeof(FATFS));
	while(f_mount(SDFs, "", 0)!= 0) printf("line:%d\r\n",__LINE__);
	show_sdcard_info();	//��ӡSD�������Ϣ
// 	FONT_Init();
}

//���ļ�����
//����򿪳ɹ����ļ��Ľṹ��ָ�뷵��
//�����ʧ���򷵻�nullָ��
FIL* OS_OpenFile(char *path,unsigned char mode)
{
    FIL *file = new(MemPool,sizeof(FIL));
	if(file != null)
	{
		if(f_open(file,path,mode) == FR_OK)
			return file;	//�ļ��򿪳ɹ������ļ����ָ��
	}
	free(file);		//���򽫷�����ڴ��ͷ��ٷ��ؿ�ָ��
	return null;
}

//�ر��ļ�����
//fileΪ�ļ��ṹ��ָ��
//�ر��ļ�֮���ļ��ṹ���ڴ��ͷ�
void OS_CloseFile(FIL *file)
{
	if(file != null)
	{
		f_close(file);
		free(file);
	}
}

//���ļ��к���
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

//�ر��ļ��к���
//ͬʱ�����ļ��нṹ���ڴ�
void OS_CloseDir(DIR *dir)
{
	if(dir != null)
	{
		f_closedir(dir);
		free(dir);
	}
}

//��ӡһ��Ŀ¼���������
//��ӡģʽ�����м����Ϣ��ӡ��������Ϣ��ӡ
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
	//�����ڴ潲���ȷ���ĺ����/�������Ȼ��
	//��ѭ��ջʽ���Ƚ����ԭ��,�������Ա�֤��������
	//�����յĿ��ڴ��ϲ�����,��Ϊ�����ڴ�����ٶ�
	//�ϲ����Ǵ�ͷ��ʼ��,���Ǵӱ����յ��ڴ���λ��
	//���ʼ���յ�,�������������ȷ�����ڴ�����
	//����������ڴ��ȫ���ϲ���
	free(dir);
	free(fno);
}

//��ȡһ���ļ����趨ƫ�������ĸ��������ֽڵ�����
//pathΪ�ļ���·���ұ����Ǿ���·��
//�����ȡ�ֽ�������lenΪ0��ʾ��ȡ�ļ������ֽ���
//����ļ��������򷵻�0��ʾû�ж�ȡ������
//��������ļ���������ʵ��ȡ�����ֽ���������
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
			//ÿ�ζ�1KB,û���������ȡ
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

//��ȡһ���ļ����趨ƫ�������ĸ��������ֽڵ�����
//fileΪ�ļ��Ľṹ��ָ��
//�����ȡ�ֽ�������lenΪ0��ʾ��ȡ�ļ������ֽ���
//����ļ��������򷵻�0��ʾû�ж�ȡ������
//��������ļ���������ʵ��ȡ�����ֽ���������
int OS_ReadFile(FIL *file,char *buff,int offset,int len)
{
	#define BYTES_READ_ONCE		1024
	unsigned int br = 0;
	if(offset > 0)
		if(f_lseek(file,offset) != FR_OK)
			return 0;
	//��ȡ����Ϊ0��ʾ��ȡ�ļ�ȫ������
	if(len == 0)
		len = file->fsize;
	if(f_read(file,buff,len,&br) != FR_OK)
		return 0;
	return br;
}

//��ָ��ƫ����offset��д�볤��Ϊlen�����ݲ�����ʵ��д����ֽ���
//fileΪ�ļ��Ľṹ��ָ��
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












