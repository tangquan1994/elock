#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#include "Prtype.h"

/* 指令包存放地址定义 */
#define COMM_PAKGE_LEN          9                               // 包头长度 2包头+4地址+1包标示+2数据长度 
#define COMM_INS_BUF	        (UART_BUF)                      // 收发数据包起始地址,接收/发送数据包时的起始地址
#define COMM_INS_PARA_BUF	    (UART_BUF+COMM_PAKGE_LEN)       // 数据包参数起始地址,应用层处理指令后应答的参数存放地址 
#define COMM_DATA_BUF	        (IMG_BUF)                       // 包标示为数据包时,数据的存放地址(主要用于发送数据) 
#define COMM_IMG_BUF	        (IMG_BUF)                       // 收发图像数据包时，图像存放地址 

/* 包头和包标示 */
#define PKG_HEAD_BYTE0          0xef        // 包头 
#define PKG_HEAD_BYTE1          0x01
#define PKG_HEAD                0x01ef      // 包头 
#define PKG_DAT_FG              0x02        // 数据包还有后续数据包 
#define PKG_DAT_END_FG          0x08        // 最后一个数据包 
#define PKG_ACK_FG              0x07        // 应答包 
#define PKG_INS_FG              0x01        // 指令包,主机发送的指令 

/*Instruction set*/
#define PS_GetImage             0x01        // 验证用获取图像  
#define PS_GenChar              0x02        // 根据原始图像生成指纹特征存于特征文件缓冲区 
#define PS_Match                0x03        // 精确比对特征文件缓冲区中的特征文件 
#define PS_Search               0x04        // 以特征文件缓冲区中的特征文件搜索整个或部分指纹库 
#define PS_RegModel             0x05        // 将特征文件合并生成模板存于特征文件缓冲区 
#define PS_StoreChar            0x06        // 将特征缓冲区中的文件存储到flash指纹库中 
#define PS_LoadChar             0x07        // 从flash指纹库中读取一个模板到特征缓冲区中 
#define PS_UpChar               0x08        // 将特征缓冲区中的文件上传给上位机 
#define PS_DownChar             0x09        // 从上位机下载一个特征文件到特征缓冲区 
#define PS_UpImage              0x0a        // 上传原始图像 
#define PS_DownImage            0x0b        // 下载原始图像 
#define PS_DeletChar            0x0c        // 删除flash指纹库中的一个特征文件 
#define PS_Empty                0x0d        // 清空flash指纹库 
#define PS_WriteReg             0x0e        // 写SOC系统寄存器 
#define PS_ReadSysPara          0x0f        // 读系统基本参数 
#define PS_SetPwd               0x12        // 设置设备握手口令 
#define PS_VfyPwd               0x13        // 验证设备握手口令 
#define PS_GetRandmoCode        0x14        // 采样随机数 
#define PS_SerChipAddr          0x15        // 设置芯片地址 
#define PS_ReadINFpage          0x16        // 读取Flash Information Page内容 
#define PS_Port_Control         0x17        // 通讯端口（UART/USB）开关控制 
#define PS_WriteNotepad         0x18        // 写记事本 
#define PS_ReadNotePad          0x19        // 读记事本 
#define PS_BurnCode             0x1a        // 烧写片内flash 
#define PS_HighSpeedSearch      0x1b        // 高速搜索flash 
#define PS_GenBinImage          0x1c        // 生成二值化指纹图像 
#define PS_ValidTempleteNum     0x1d        // 读有效模板个数 
#define PS_UserGPIOCommand      0x1e        // 用户GPIO控制命令 
#define PS_ReadIndexTable       0x1f        // 读索引表 
#define PS_GetEnrollImage       0x29        // 注册用获取图像
#define PS_Cancle               0x30        // 取消指令 
#define PS_AutoEnroll           0x31        // 自动注册模板
#define PS_AutoIdentify         0x32        // 自动验证指纹 
#define PS_Sleep                0x33        // 休眠指令 
#define PS_GetChipSN            0x34        // 获取芯片唯一序列号 
#define PS_HandShake            0x35        // 握手指令 
#define PS_CheckSensor          0x36        // 校验传感器 

/*Err Response Set*/
#define SAR_OK                  0x00        //成功
#define SAR_FAIL                0x01        //收包错误
#define SAR_NOFIG               0x02        //传感器上无手指 
#define SAR_GETIMAGE_FAIL       0x03        //录入指纹图像失败
#define SAR_IMAGE_DRY           0x04        //指纹图像太干、太淡而生不成特征
#define SAR_IMAGE_WET           0x05        //指纹图像太湿、太糊
#define SAR_IMAGE_DISORDER      0x06        //指纹图像太乱而生不成特征
#define SAR_FEATUREPOOR         0x07        //特征点太少而生不成特征
#define SAR_FIG_NOTMATCH        0x08        //指纹不匹配
#define SAR_SERCHNOTHING        0x09        //没搜索到指纹 
#define SAR_MERGE_FAIL          0x0a        //合并失败（两枚指纹不属于同一手指）
#define SAR_PID_OUTLINE         0x0b        //PageID超出指纹库范围
#define SAR_MOD_UNVALID         0x0c        //读出有错或模板无效
#define SAR_INS_RUNFAIL         0x0d        //指令执行失败，上传特征失败 
#define SAR_PACK_RECFAIL        0x0e        //不能接受后续数据包
#define SAR_PACK_SENDFAIL       0x0f        //不能发送后续数据包，上传图像失败 
#define SAR_MODDELET_FAIL       0x10        //删除模板失败 
#define SAR_CLEAR_FAIL          0x11        //清空指纹库失败 
#define SAR_NOLOWPOWER_STATE    0x12        //不能进入低功耗状态 
#define SAR_INS_ERR             0x13        //口令不正确
#define SAR_RESET_FAIL          0x14        //系统复位失败 
#define SAR_NOORIGINIMG         0x15        //图像缓冲区内没有有效原始图而生不成图像
#define SAR_UPDATA_FAIL         0x16        //在线升级失败 
#define SAR_FIG_NOTMOVE         0x17        //残留指纹或两次采集之间手指没有移动过 
#define SAR_FLASH_WRERR         0x18        //读写Flash出错
#define SAR_GENRAND_FAIL        0x19        //生成随机数失败 
#define SAR_REGSERIES_ERR       0x1a        //寄存器序列号有误 
#define SAR_REGSET_ERR          0x1b        //寄存器设定内容错误号
#define SAR_PAGEID_ERR          0x1c        //记事本页码指定错误 
#define SAR_PORT_RUNFAIL        0x1d        //端口操作失败
#define SAR_AUTOENROLL_FAIL     0x1e        //自动注册失败 
#define SAR_FIGLIB_FULL         0x1f        //指纹库已满
#define SAR_DEVADDR_ERR         0x20        //设备地址错误
#define SAR_KEY_ERR             0x21        //密码有误 
#define SAR_FIGMOD_NOTNULL      0x22        //指纹模板非空
#define SAR_FIGMOD_NULL         0x23        //指纹模板为空 
#define SAR_FIGLIB_NULL         0x24        //指纹库为空 
#define SAR_LOGCOUNT_ERR        0x25        //录入次数设置错误
#define SAR_TIMEOUT             0x26        //超时
#define SAR_FIG_EXIST           0x27        //指纹已存在 
#define SAR_MODRELATION         0x28        //当前指纹模板与之前模板之间有关联
#define SAR_ECCSENSE_ERR        0x29        //校验传感器出错 
#define SAR_F0_ERR              0xf0        //有后续数据包的指令，正确接收后用0xf0应答
#define SAR_F1_ERR              0xf1        //有后续数据包的指令，命令包用0xf1应答
#define SAR_FCRC_ERR            0xf2        //烧写内部flash，校验和错误
#define SAR_FFLAG_ERR           0xf3        //烧写内部flash，包标志错误
#define SAR_FLEN_ERR            0xf4        //烧写内部flash，包长度错误
#define SAR_FCODELEN_LONG       0xf5        //烧写内部flash，代码长度太长
#define SAR_FLASH_FAIL          0xf6        //烧写flash失败 
//Reserved 2aH--efH
    
//包标示类型 
#define MASK_INS                0x01        //指令包 
#define MASK_DAT                0x02        //数据包  
#define MASK_ACK                0x07        //应答包 
#define MASK_DAT_LAST           0x08        //最后数据包 


//返回码
#define CMD_OK                  0x00        //通讯成功 
#define CMD_ERR                 0x01        //通讯失败 
#define CMD_TIMEOUT             0x02        //命令超时 
#define CMD_SUM_ERR             0x03        //应答包校验错误 
#define CMD_COMM_ERR            0x04        //收到正常的应答包，但应答包的确认码报错 
#define CMD_LOGIC_ERR           0x0A        //逻辑错误，收到的应答包与发送的指令不一致 
#define CMD_CODE_ERR            0x0B        //代码编写有误 
#define CMD_STEP_ERR            0x0C        //操作失败 

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
