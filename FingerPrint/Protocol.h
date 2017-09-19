#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#include "Prtype.h"

/* ָ�����ŵ�ַ���� */
#define COMM_PAKGE_LEN          9                               // ��ͷ���� 2��ͷ+4��ַ+1����ʾ+2���ݳ��� 
#define COMM_INS_BUF	        (UART_BUF)                      // �շ����ݰ���ʼ��ַ,����/�������ݰ�ʱ����ʼ��ַ
#define COMM_INS_PARA_BUF	    (UART_BUF+COMM_PAKGE_LEN)       // ���ݰ�������ʼ��ַ,Ӧ�ò㴦��ָ���Ӧ��Ĳ�����ŵ�ַ 
#define COMM_DATA_BUF	        (IMG_BUF)                       // ����ʾΪ���ݰ�ʱ,���ݵĴ�ŵ�ַ(��Ҫ���ڷ�������) 
#define COMM_IMG_BUF	        (IMG_BUF)                       // �շ�ͼ�����ݰ�ʱ��ͼ���ŵ�ַ 

/* ��ͷ�Ͱ���ʾ */
#define PKG_HEAD_BYTE0          0xef        // ��ͷ 
#define PKG_HEAD_BYTE1          0x01
#define PKG_HEAD                0x01ef      // ��ͷ 
#define PKG_DAT_FG              0x02        // ���ݰ����к������ݰ� 
#define PKG_DAT_END_FG          0x08        // ���һ�����ݰ� 
#define PKG_ACK_FG              0x07        // Ӧ��� 
#define PKG_INS_FG              0x01        // ָ���,�������͵�ָ�� 

/*Instruction set*/
#define PS_GetImage             0x01        // ��֤�û�ȡͼ��  
#define PS_GenChar              0x02        // ����ԭʼͼ������ָ���������������ļ������� 
#define PS_Match                0x03        // ��ȷ�ȶ������ļ��������е������ļ� 
#define PS_Search               0x04        // �������ļ��������е������ļ����������򲿷�ָ�ƿ� 
#define PS_RegModel             0x05        // �������ļ��ϲ�����ģ����������ļ������� 
#define PS_StoreChar            0x06        // �������������е��ļ��洢��flashָ�ƿ��� 
#define PS_LoadChar             0x07        // ��flashָ�ƿ��ж�ȡһ��ģ�嵽������������ 
#define PS_UpChar               0x08        // �������������е��ļ��ϴ�����λ�� 
#define PS_DownChar             0x09        // ����λ������һ�������ļ������������� 
#define PS_UpImage              0x0a        // �ϴ�ԭʼͼ�� 
#define PS_DownImage            0x0b        // ����ԭʼͼ�� 
#define PS_DeletChar            0x0c        // ɾ��flashָ�ƿ��е�һ�������ļ� 
#define PS_Empty                0x0d        // ���flashָ�ƿ� 
#define PS_WriteReg             0x0e        // дSOCϵͳ�Ĵ��� 
#define PS_ReadSysPara          0x0f        // ��ϵͳ�������� 
#define PS_SetPwd               0x12        // �����豸���ֿ��� 
#define PS_VfyPwd               0x13        // ��֤�豸���ֿ��� 
#define PS_GetRandmoCode        0x14        // ��������� 
#define PS_SerChipAddr          0x15        // ����оƬ��ַ 
#define PS_ReadINFpage          0x16        // ��ȡFlash Information Page���� 
#define PS_Port_Control         0x17        // ͨѶ�˿ڣ�UART/USB�����ؿ��� 
#define PS_WriteNotepad         0x18        // д���±� 
#define PS_ReadNotePad          0x19        // �����±� 
#define PS_BurnCode             0x1a        // ��дƬ��flash 
#define PS_HighSpeedSearch      0x1b        // ��������flash 
#define PS_GenBinImage          0x1c        // ���ɶ�ֵ��ָ��ͼ�� 
#define PS_ValidTempleteNum     0x1d        // ����Чģ����� 
#define PS_UserGPIOCommand      0x1e        // �û�GPIO�������� 
#define PS_ReadIndexTable       0x1f        // �������� 
#define PS_GetEnrollImage       0x29        // ע���û�ȡͼ��
#define PS_Cancle               0x30        // ȡ��ָ�� 
#define PS_AutoEnroll           0x31        // �Զ�ע��ģ��
#define PS_AutoIdentify         0x32        // �Զ���ָ֤�� 
#define PS_Sleep                0x33        // ����ָ�� 
#define PS_GetChipSN            0x34        // ��ȡоƬΨһ���к� 
#define PS_HandShake            0x35        // ����ָ�� 
#define PS_CheckSensor          0x36        // У�鴫���� 

/*Err Response Set*/
#define SAR_OK                  0x00        //�ɹ�
#define SAR_FAIL                0x01        //�հ�����
#define SAR_NOFIG               0x02        //������������ָ 
#define SAR_GETIMAGE_FAIL       0x03        //¼��ָ��ͼ��ʧ��
#define SAR_IMAGE_DRY           0x04        //ָ��ͼ��̫�ɡ�̫��������������
#define SAR_IMAGE_WET           0x05        //ָ��ͼ��̫ʪ��̫��
#define SAR_IMAGE_DISORDER      0x06        //ָ��ͼ��̫�Ҷ�����������
#define SAR_FEATUREPOOR         0x07        //������̫�ٶ�����������
#define SAR_FIG_NOTMATCH        0x08        //ָ�Ʋ�ƥ��
#define SAR_SERCHNOTHING        0x09        //û������ָ�� 
#define SAR_MERGE_FAIL          0x0a        //�ϲ�ʧ�ܣ���öָ�Ʋ�����ͬһ��ָ��
#define SAR_PID_OUTLINE         0x0b        //PageID����ָ�ƿⷶΧ
#define SAR_MOD_UNVALID         0x0c        //�����д��ģ����Ч
#define SAR_INS_RUNFAIL         0x0d        //ָ��ִ��ʧ�ܣ��ϴ�����ʧ�� 
#define SAR_PACK_RECFAIL        0x0e        //���ܽ��ܺ������ݰ�
#define SAR_PACK_SENDFAIL       0x0f        //���ܷ��ͺ������ݰ����ϴ�ͼ��ʧ�� 
#define SAR_MODDELET_FAIL       0x10        //ɾ��ģ��ʧ�� 
#define SAR_CLEAR_FAIL          0x11        //���ָ�ƿ�ʧ�� 
#define SAR_NOLOWPOWER_STATE    0x12        //���ܽ���͹���״̬ 
#define SAR_INS_ERR             0x13        //�����ȷ
#define SAR_RESET_FAIL          0x14        //ϵͳ��λʧ�� 
#define SAR_NOORIGINIMG         0x15        //ͼ�񻺳�����û����Чԭʼͼ��������ͼ��
#define SAR_UPDATA_FAIL         0x16        //��������ʧ�� 
#define SAR_FIG_NOTMOVE         0x17        //����ָ�ƻ����βɼ�֮����ָû���ƶ��� 
#define SAR_FLASH_WRERR         0x18        //��дFlash����
#define SAR_GENRAND_FAIL        0x19        //���������ʧ�� 
#define SAR_REGSERIES_ERR       0x1a        //�Ĵ������к����� 
#define SAR_REGSET_ERR          0x1b        //�Ĵ����趨���ݴ����
#define SAR_PAGEID_ERR          0x1c        //���±�ҳ��ָ������ 
#define SAR_PORT_RUNFAIL        0x1d        //�˿ڲ���ʧ��
#define SAR_AUTOENROLL_FAIL     0x1e        //�Զ�ע��ʧ�� 
#define SAR_FIGLIB_FULL         0x1f        //ָ�ƿ�����
#define SAR_DEVADDR_ERR         0x20        //�豸��ַ����
#define SAR_KEY_ERR             0x21        //�������� 
#define SAR_FIGMOD_NOTNULL      0x22        //ָ��ģ��ǿ�
#define SAR_FIGMOD_NULL         0x23        //ָ��ģ��Ϊ�� 
#define SAR_FIGLIB_NULL         0x24        //ָ�ƿ�Ϊ�� 
#define SAR_LOGCOUNT_ERR        0x25        //¼��������ô���
#define SAR_TIMEOUT             0x26        //��ʱ
#define SAR_FIG_EXIST           0x27        //ָ���Ѵ��� 
#define SAR_MODRELATION         0x28        //��ǰָ��ģ����֮ǰģ��֮���й���
#define SAR_ECCSENSE_ERR        0x29        //У�鴫�������� 
#define SAR_F0_ERR              0xf0        //�к������ݰ���ָ���ȷ���պ���0xf0Ӧ��
#define SAR_F1_ERR              0xf1        //�к������ݰ���ָ��������0xf1Ӧ��
#define SAR_FCRC_ERR            0xf2        //��д�ڲ�flash��У��ʹ���
#define SAR_FFLAG_ERR           0xf3        //��д�ڲ�flash������־����
#define SAR_FLEN_ERR            0xf4        //��д�ڲ�flash�������ȴ���
#define SAR_FCODELEN_LONG       0xf5        //��д�ڲ�flash�����볤��̫��
#define SAR_FLASH_FAIL          0xf6        //��дflashʧ�� 
//Reserved 2aH--efH
    
//����ʾ���� 
#define MASK_INS                0x01        //ָ��� 
#define MASK_DAT                0x02        //���ݰ�  
#define MASK_ACK                0x07        //Ӧ��� 
#define MASK_DAT_LAST           0x08        //������ݰ� 


//������
#define CMD_OK                  0x00        //ͨѶ�ɹ� 
#define CMD_ERR                 0x01        //ͨѶʧ�� 
#define CMD_TIMEOUT             0x02        //���ʱ 
#define CMD_SUM_ERR             0x03        //Ӧ���У����� 
#define CMD_COMM_ERR            0x04        //�յ�������Ӧ�������Ӧ�����ȷ���뱨�� 
#define CMD_LOGIC_ERR           0x0A        //�߼������յ���Ӧ����뷢�͵�ָ�һ�� 
#define CMD_CODE_ERR            0x0B        //�����д���� 
#define CMD_STEP_ERR            0x0C        //����ʧ�� 

void  Port_Init(void);
UINT8 CMD_Send(UINT8 Type, UINT8 INS, UINT8 *pData, UINT16 Datalen);
UINT8 CMD_DataSend(UINT8 Type, UINT8 *pData, UINT16 Datalen);
UINT8 CommPakgeRev(UINT8 *pPakege,UINT16 *plen,UINT16 TimeOutMs,UINT8 *ERR);
UINT8 CommDataPakgeRev(UINT8 *pData,UINT16 *plen,UINT16 TimeOutMs,UINT8 *ERR);
UINT8 CommSingleInstruction(UINT8 Type, UINT8 INS, UINT8 *pDataIn, UINT16 *Datalen, UINT8 *pDataOut, UINT8 *pErr, UINT16 TimeOutMs);
//UINT8 CommSingleInstruction(UINT8 mark,UINT8 Order,  UINT8 *pDataIn,UINT16 *Datalen, UINT8 *pDataOut,UINT8 *pErr,UINT16 TimeOutMs);
//UINT8 CommDownDataInstruction(UINT8 mark,UINT8 Order, UINT8 *pInsParaIn,  UINT8 *pDataIn,UINT16 Datalen,UINT16 MaxLenPerPkg,UINT8 *pErr,UINT16 TimeOutMs);
//UINT8 CommUpDataInstruction(UINT8 mark,UINT8 Order, UINT8 *pInsParaIn,  UINT8 *pDataOut,UINT16 *Datalen,UINT16 MaxLenPerPkg,UINT8 *pErr,UINT16 TimeOutMs);
UINT8 CommDownDataInstruction(UINT8 Type, UINT8 INS, UINT8 *pInsData, UINT16 InsDataLen, UINT8 *pDataIn, UINT16 Datalen, UINT16 MaxLenPerPkg, UINT8 *pErr, UINT16 TimeOutMs);
UINT8 CommUpDataInstruction(UINT8 Type,UINT8 INS, UINT8 *pInsData, UINT16 InsDataLen, UINT8 *pDataOut,UINT16 *Datalen,UINT16 MaxLenPerPkg,UINT8 *pErr,UINT16 TimeOutMs);
UINT8 FunHandshakeModule(UINT8 nTime);

void SY_memcpy(UINT8 *Src, UINT8 *pDes, UINT32 len);
void SY_memset(UINT8 *Src, UINT8 dat, UINT32 len);
#endif
