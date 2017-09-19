#include "Prtype.h"
#include "Uartport.h"
#include "Protocol.h"

#define	BW(H,L,W)	W = ((H << 8) & 0xff00)	|(L & 0xff)

extern UINT8  UartTxBuff[];
UINT8 gDeviceAddr[4];                   //地址码，默认0xffffffff 

void DelayMs(UINT32 n)
{
    n = SYSFrequency/1000*n/TimeAdjust;
    while (n--);
}
/***************************************************************************************************
** Subroutine  : UartAdjust
** Function    : 串口波特率校准 
** Author      : yf
** Input       : 无
** Output      : 无 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/

void UartAdjust(void)
{
	UINT8 buf[16];
    UINT16 i;
	
    for (i = 0; i < 16; i++)
	{
		buf[i] = 0xaa;
	}	
	UartDataSend(buf, 16);
}

/***************************************************************************************************
** Subroutine  : Port_Init
** Function    : 通讯初始化
** Author      : yf
** Input       : 无
** Output      : 无 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
void Port_Init(void)
{	
	UartPortInit();
	gDeviceAddr[0] = 0xff;
	gDeviceAddr[1] = 0xff;
	gDeviceAddr[2] = 0xff;
	gDeviceAddr[3] = 0xff;
	UartAdjust();
}

void SY_memcpy(UINT8 *Src, UINT8 *pDes, UINT32 len)
{
	UINT32 i;
	for (i = 0; i < len; i++)
	{
		Src[i] = pDes[i];
	}	
}

void SY_memset(UINT8 *Src, UINT8 dat, UINT32 len)
{
	UINT32 i;
	
	for(i = 0; i < len; i++)
	{
		Src[i] = dat;
	}	
}

/***************************************************************************************************
** Subroutine  : CmdGenCHK
** Function    : 生成CRC校验 
** Author      : yf
** Input       :  UINT8 *cpRecBuf --  校验包  
** Output      : 检验值 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT16 CMD_GenCRC(UINT8 *cpRecBuf)
{
	UINT16 wLen;
	UINT32 i;
	UINT16 nwCHK = 0;
	
	wLen = cpRecBuf[7] * 256 + cpRecBuf[8];
	for(i = 6; i < (7 + wLen); i++)
		nwCHK += (cpRecBuf[i]&0xff);
		
	cpRecBuf[7+wLen] = nwCHK/256;
	cpRecBuf[8+wLen] = nwCHK%256;

	return(nwCHK);
}

/***************************************************************************************************
** Subroutine  : CMD_VerCRC
** Function    : CRC校验 
** Author      : yf
** Input       : UINT8 *cpRecBuf --  数据包指针  
** Output      : 成功:0,失败:1 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
BOOL CMD_VerCRC(UINT8 *cpRecBuf)
{	
	UINT16 nPKLen;
    UINT16 nRecCRC, nGenCRC;
	
	BW(cpRecBuf[7], cpRecBuf[8], nPKLen);
	BW(cpRecBuf[7 + nPKLen], cpRecBuf[8 + nPKLen], nRecCRC);	

	nGenCRC = CMD_GenCRC(cpRecBuf);	
	if(nGenCRC != nRecCRC)
		return CMD_ERR;
	else
		return CMD_OK;
}

/***************************************************************************************************
** Subroutine  : MakePackageHead
** Function    : 生成协议包的包头 
** Author      : yf
** Input       : UINT8 *pHead --  生成的包头存放指针 
**             : UINT8 Type  --  包标示 
**             : UINT8 nDataLen   --  包长度   
** Output      : 无 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
void MakePackageHead(UINT8* pHead,UINT8 Type,UINT32 nDataLen)
{ 	
	pHead[0] = 0xef;
	pHead[1] = 0x01;
	pHead[2] = gDeviceAddr[0];
	pHead[3] = gDeviceAddr[1];
	pHead[4] = gDeviceAddr[2];
	pHead[5] = gDeviceAddr[3];
	pHead[6] = Type;
	pHead[7] = ((nDataLen+2) >> 8) & 0xff;        
	pHead[8] = (nDataLen+2) & 0xff;
}

/***************************************************************************************************
** Subroutine  : MakePackage
** Function    : 组包 
** Author      : yf
** Input       : UINT8 *pData   --   数据、参数地址 
**             : UINT8 *pOut    --   包指针 
**             : UINT8 Type     --   包标示 
**             : UINT8 nDataLen   --  数据长度 
** Output      : 无 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 MakePackage(UINT8 Type, UINT8 *pData, UINT32 nDataLen, UINT8 *pOut, UINT32* nOutLen)
{
    if((pData != NULL) && (pOut != NULL))
    {
        MakePackageHead(pOut, Type, nDataLen);//包头          
        SY_memcpy(&pOut[COMM_PAKGE_LEN], pData, nDataLen);
    	CMD_GenCRC(pOut);
    	*nOutLen = COMM_PAKGE_LEN + nDataLen + 2 ;
    	return CMD_OK;    	
    } 
    else
        return CMD_ERR;   
}

/***************************************************************************************************
** Subroutine  : CommandSend
** Function    : 发送命令函数 
** Author      : yf
** Input       : UINT8 type     --  包标示 
**             : UINT8 Datalen  --  数据长度 
**             : UINT8 INS      --  命令码 
**             : UINT8 pData    --  数据指针 
** Output      : 成功:CMD_OK  失败：CMD_ERR 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 CMD_Send(UINT8 Type, UINT8 INS, UINT8 *pData, UINT16 Datalen)
{
    UINT8 cRet;
    UINT16 len, nCRC;
    
    MakePackageHead(UartTxBuff, Type, Datalen+1);//包头     ???DataLen + 1[INS take 1 byte length]
    len = COMM_PAKGE_LEN;
    UartTxBuff[COMM_PAKGE_LEN] = INS;
    len += 1;
    SY_memcpy(&UartTxBuff[COMM_PAKGE_LEN+1], pData, Datalen);
    len += Datalen;
    nCRC = CMD_GenCRC(UartTxBuff);
    len += 2;
//    len = COMM_PAKGE_LEN + Datalen + 3;
	cRet = UartDataSend(UartTxBuff, len);
	if (cRet != 0)
	{
		return CMD_ERR;
	}
	return CMD_OK;
}
/***************************************************************************************************
** Subroutine  : CommDataSend
** Function    : 发送数据包函数 
** Author      : yf
** Input       : UINT8 Type     --   包标示 
**             : UINT8 Datalen  -- 数据长度 
**             : UINT8 pData    --  数据指针 
** Output      : 成功:CMD_OK  失败：CMD_ERR 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 CMD_DataSend(UINT8 Type, UINT8 *pData, UINT16 Datalen)
{
    UINT8 cRet;
    UINT16 len, nCRC;
    
    MakePackageHead(UartTxBuff, Type, Datalen);//包头
    len = COMM_PAKGE_LEN;
    SY_memcpy(&UartTxBuff[COMM_PAKGE_LEN], pData, Datalen);
    len += Datalen; 
    nCRC = CMD_GenCRC(UartTxBuff);
    len += 2;
//    len=COMM_PAKGE_LEN+Datalen+2;
	cRet=UartDataSend(UartTxBuff, len);
	if (cRet!=0)
	{
		return CMD_ERR;
	}
	return CMD_OK;
}
/***************************************************************************************************
** Subroutine  : CommPakgeRev
** Function    : 接受协议包 
** Author      : yf
** Input       : UINT8 *pPakege  --   协议包存放地址 
**               UINT16 *pLen   --    协议包长度（包头到校验和整个包的字节数） 
**             : UINT8 TimeOutMs -- 接受超时，单位ms 
**             : UINT8 *ERR  --     应答包确认码  
** Output      : 成功:CMD_OK  超时：CMD_TIMEOUT  校验和错误：CMD_SUM_ERR
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 CommPakgeRev(UINT8 *pPakege,UINT16 *pLen,UINT16 TimeOutMs,UINT8 *ERR)
{
    UINT16 i,len,CRCSUM;
    UINT8 cRet;
    UINT8 tBuf[16];
    //包头解析 
    if (pPakege==NULL)
        return CMD_CODE_ERR;
    len=COMM_PAKGE_LEN;
    do
    {
        cRet=UartDataRev(tBuf,1,TimeOutMs);
        if (cRet!=RT_OK)
            return CMD_TIMEOUT;
        if (tBuf[0]==PKG_HEAD_BYTE0)
        {
        	cRet=UartDataRev(&tBuf[1],1,TimeOutMs);
            if (cRet!=RT_OK)
                return CMD_TIMEOUT;
            if (tBuf[1]==PKG_HEAD_BYTE1)
            {
                pPakege[0]=PKG_HEAD_BYTE0;
                pPakege[1]=PKG_HEAD_BYTE1;
                cRet=UartDataRev(&pPakege[2],COMM_PAKGE_LEN-2,TimeOutMs);
                if (cRet!=RT_OK)
                    return CMD_TIMEOUT;             //???应返回错误码 
                break;
            }            
        }        
    }while (1);
    //包头长度 2包头+4地址+1包标示+2数据长度 
    len = (pPakege[7]<<8) + pPakege[8];//高字节在前低字节在后 
    cRet = UartDataRev(&pPakege[9], len, TimeOutMs);//接收剩余字节数据 
    if (cRet != RT_OK)
        return CMD_TIMEOUT;
    
    //校验和 
    cRet=CMD_VerCRC(pPakege);
    if (cRet != CMD_OK)
    {
    	return CMD_SUM_ERR;
    }   
    if (pPakege[6]==MASK_ACK)     
    {
    	*ERR = pPakege[9];
    }    
    *pLen= len+ COMM_PAKGE_LEN; 
    return	CMD_OK;//接收剩余字节数据 
}

/***************************************************************************************************
** Subroutine  : CommDataPakgeRev
** Function    : 数据包接受函数，接收到的是纯数据 
** Author      : yf
** Input       : UINT8 *pData  --   纯数据存放地址 
**               UINT16 *plen  --   数据长度，纯数据
**             : UINT8 TimeOutMs -- 接受超时，单位ms 
**             : UINT8 *ERR  --     应答包确认码  
** Output      : 成功:CMD_OK  超时：CMD_TIMEOUT  校验和错误：CMD_SUM_ERR  收发的逻辑错误：CMD_LOGIC_ERR 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
/*
1.主机->从机   0x01 命令
2.从机->主机   0x07 命令应答 
3.从机->主机   0x02 数据包 
4.从机->主机   0x02 数据包 
...
n.从机->主机   0x08 最后一个数据包 
*/
UINT8 CommDataPakgeRev(UINT8 *pData,UINT16 *plen,UINT16 TimeOutMs,UINT8 *ERR)
{
    UINT8 cRet,temp;
    UINT16 len,len1,lenPkg;
    cRet=CommPakgeRev(pData,&lenPkg, TimeOutMs,ERR);
    if (cRet!=CMD_OK)
        return cRet;
    //包头长度 2包头+4地址+1包标示+2数据长度 
    if (pData[6]==0x07)
    {
    	if (*ERR!=0)
    	   return CMD_COMM_ERR;//应答包报错 
    } 
    len=0;
//    SY_memcpy(&pOut[COMM_PAKGE_LEN],pIn,wLen);
    do
    {
        cRet=CommPakgeRev(&pData[len],&lenPkg, TimeOutMs,ERR);
        if (cRet!=CMD_OK)
            return cRet;
        temp= pData[len+6]; 
        if (temp==0x02||temp==0x08)
        {
            len1=(pData[len+7]<<8)+pData[len+8];
        	SY_memcpy(&pData[len],&pData[len+9],len1);
        	len+=len1;
        	if (temp==0x08)
        	{
        		*plen=len1;
        		return CMD_OK;
        	}        	        	   
        } 
        else
        {
            if (*ERR!=0)
        	   return CMD_COMM_ERR;//应答包报错 
        	return CMD_LOGIC_ERR; //逻辑错误 
        }        
    }while (1);   
}

/***************************************************************************************************
** Subroutine  : CommSingleInstruction
** Function    : 单一指令处理函数，此函数不适用与符合指令如一站式协议，数据接收等指令 
** Author      : yf
** Input       : UINT8 Type             包标示 
**               UINT8 INS              命令码 
**               UINT8 *pDataIn         输入参数命令码以后校验码以前的数据 
**               UINT16 *Datalen [I/O]  数据长度,返回接收数据包的长度（整包长度）。 
**               UINT8 *pDataOut:       接收数据包 
**               UINT8 *pErr:           应答包确认码 
**               UINT16 TimeOutMs:      指令等待应答超时 
** Output      : 成功:CMD_OK  失败：CMD_COMM_ERR 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 CommSingleInstruction(UINT8 Type, UINT8 INS, UINT8 *pDataIn, UINT16 *Datalen, UINT8 *pDataOut, UINT8 *pErr, UINT16 TimeOutMs)
//UINT8 APDU_INS(UINT8 Type, UINT8 INS, UINT8 *pDataIn, UINT16 *Datalen,  UINT8 *pDataOut, UINT8 *pErr, UINT8 nOutTime)
{
    UINT8 cRet, cERR;
    UINT16 rLen;

    ClearTxBuff();
    ClearRxBuff();

    cRet = CMD_Send(Type, INS, pDataIn, *Datalen);
	if (cRet != CMD_OK)
	{
		return CMD_ERR;//命令发送错误 
	}
    	
	cRet=CommPakgeRev(pDataOut, &rLen, TimeOutMs, &cERR);
    if (cRet != CMD_OK)
        return cRet;

    *pErr = cERR;//pDataOut[9];
    *Datalen = rLen;
	return CMD_OK;

}


/***************************************************************************************************
** Subroutine  : CommDownDataInstruction
** Function    : 下载数据命令，下载数据到模组，如特征文件等。次指令是属于符合命令 
** Author      : yf
** Input       : UINT8 Type 包标示 
**               UINT8 INS 命令码 
**               UINT8 *pDataIn 下发的数据（纯数据） 
**               UINT16 *Datalen 数据长度,返回接收数据包的长度（整包长度）。 
**               UINT8 *pInsData: 命令指令数据 
**               UINT8 InsDataLen 指令数据长度 
**               UINT8 *pErr:    应答包确认码 
**               UINT16 TimeOutMs: 指令等待应答超时 
** Output      : 成功:CMD_OK  失败：CMD_COMM_ERR 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 CommDownDataInstruction(UINT8 Type, UINT8 INS, UINT8 *pInsData, UINT16 InsDataLen, UINT8 *pDataIn, UINT16 Datalen, UINT16 MaxLenPerPkg, UINT8 *pErr, UINT16 TimeOutMs)
{
    UINT8 cRet,ERR;
    UINT16 lenPkg;
    int n;
    UINT8 Buf[128];
    n=2;
    while (n>0)
    {
        ClearTxBuff();
        ClearRxBuff();
    
        cRet = CMD_Send(Type, INS, pInsData, InsDataLen);
    	if (cRet != CMD_OK)
    	{
    		return CMD_ERR;//命令发送错误 
    	}
        	
    	cRet = CommPakgeRev(Buf, &lenPkg, TimeOutMs, &ERR);
        if (cRet != CMD_OK)
            return cRet;
    	if (ERR==0x00)
    	{
    		break;
    	}    	
    	else //传输错误或者模组繁忙，过一会再尝试 
    	{
            *pErr=ERR;//pDataOut[9];
            if (n-->0)  
            {
                DelayMs(20);
            	continue;
            }   
    		return CMD_COMM_ERR;
    	}		
    } 
    
    for (n=0;n<Datalen;)
    {
        if((Datalen-n)<MaxLenPerPkg)
        {
        	cRet=  CMD_DataSend( MASK_DAT_LAST, &pDataIn[n], Datalen-n); 
        	n+=Datalen-n;
        }                    
        else
        {
        	cRet=  CMD_DataSend( MASK_DAT, &pDataIn[n],MaxLenPerPkg); 
            n+= MaxLenPerPkg;
            DelayMs(20);
        }                   
    }   
    return CMD_OK;   
}

/***************************************************************************************************
** Subroutine  : CommDownDataInstruction
** Function    : 接受数据命令，从模块读数据数，如特征文件等。次指令是属于符合命令 
** Author      : yf
** Input       : UINT8 Type 包标示 
**               UINT8 INS 命令码 
**               UINT8 *pDataOut 读取的数据（纯数据）存放地址 
**               UINT16 *Datalen 数据长度,返回接收数据包的长度（整包长度）。  
**               UINT8 *pInsParaIn: 命令参数 
**               UINT8 *pErr:    应答包确认码 
**               UINT16 TimeOutMs: 指令等待应答超时 
** Output      : 成功:CMD_OK  失败：CMD_COMM_ERR 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 CommUpDataInstruction(UINT8 Type,UINT8 INS, UINT8 *pInsData, UINT16 InsDataLen, UINT8 *pDataOut,UINT16 *Datalen,UINT16 MaxLenPerPkg,UINT8 *pErr,UINT16 TimeOutMs)
{
    UINT8 cRet,ERR;
    UINT16 lenPkg;
    int n;
    UINT8 Buf[128];
   
    n=2;
    while (n>0)
    {
        ClearTxBuff();
        ClearRxBuff();
    
        cRet=CMD_Send(Type, INS, pInsData, InsDataLen);
    	if (cRet != CMD_OK)
    	{
    		return CMD_ERR;//命令发送错误 
    	}
        	
    	cRet=CommPakgeRev(Buf, &lenPkg, TimeOutMs, &ERR);    	
        if (cRet!=CMD_OK)
            return cRet;
    	
        if (ERR==0x00)
    	{
    		break;
    	}    	
    	else //传输错误或者模组繁忙，过一会再尝试 
    	{
            *pErr=ERR;//pDataOut[9];
            if (n-->0)  
            {
                DelayMs(20);
            	continue;
            }   
    		return CMD_COMM_ERR;
    	}		
    } 
    cRet=CommDataPakgeRev(pDataOut,&lenPkg,TimeOutMs,&ERR);
    *pErr=ERR;
    *Datalen=lenPkg;
    return cRet;  
}


/***************************************************************************************************
** Subroutine  : FunHandshakeModule
** Function    : 模块握手，给模块上电以后需要与模块握手一下一遍通信稳定 
** Author      : yf
** Input       : UINT8 nTime  --   握手尝试次数 
 
** Output      : 成功:CMD_OK  失败：CMD_COMM_ERR 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 FunHandshakeModule(UINT8 nTime)
{
    UINT8 cRet,ERR;
    UINT8 Buf[128];
    UINT16 lenPkg;
    while (nTime--)
    {
        UartAdjust();
        lenPkg=0;
        cRet = CommSingleInstruction(MASK_INS, 0x35, NULL, &lenPkg, Buf,&ERR,50);
    
        if (cRet!=CMD_OK)
        {
        	continue;
        }  
        else
        {
            if (ERR==0)
        	{
        		return CMD_OK;
        	}	
        }              
    }   
    return CMD_TIMEOUT; 
}




