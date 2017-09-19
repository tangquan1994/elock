#include "Prtype.h"
#include "Uartport.h"
#include "Protocol.h"

#define	BW(H,L,W)	W = ((H << 8) & 0xff00)	|(L & 0xff)

extern UINT8  UartTxBuff[];
UINT8 gDeviceAddr[4];                   //��ַ�룬Ĭ��0xffffffff 

void DelayMs(UINT32 n)
{
    n = SYSFrequency/1000*n/TimeAdjust;
    while (n--);
}
/***************************************************************************************************
** Subroutine  : UartAdjust
** Function    : ���ڲ�����У׼ 
** Author      : yf
** Input       : ��
** Output      : �� 
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
** Function    : ͨѶ��ʼ��
** Author      : yf
** Input       : ��
** Output      : �� 
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
** Function    : ����CRCУ�� 
** Author      : yf
** Input       :  UINT8 *cpRecBuf --  У���  
** Output      : ����ֵ 
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
** Function    : CRCУ�� 
** Author      : yf
** Input       : UINT8 *cpRecBuf --  ���ݰ�ָ��  
** Output      : �ɹ�:0,ʧ��:1 
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
** Function    : ����Э����İ�ͷ 
** Author      : yf
** Input       : UINT8 *pHead --  ���ɵİ�ͷ���ָ�� 
**             : UINT8 Type  --  ����ʾ 
**             : UINT8 nDataLen   --  ������   
** Output      : �� 
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
** Function    : ��� 
** Author      : yf
** Input       : UINT8 *pData   --   ���ݡ�������ַ 
**             : UINT8 *pOut    --   ��ָ�� 
**             : UINT8 Type     --   ����ʾ 
**             : UINT8 nDataLen   --  ���ݳ��� 
** Output      : �� 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 MakePackage(UINT8 Type, UINT8 *pData, UINT32 nDataLen, UINT8 *pOut, UINT32* nOutLen)
{
    if((pData != NULL) && (pOut != NULL))
    {
        MakePackageHead(pOut, Type, nDataLen);//��ͷ          
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
** Function    : ��������� 
** Author      : yf
** Input       : UINT8 type     --  ����ʾ 
**             : UINT8 Datalen  --  ���ݳ��� 
**             : UINT8 INS      --  ������ 
**             : UINT8 pData    --  ����ָ�� 
** Output      : �ɹ�:CMD_OK  ʧ�ܣ�CMD_ERR 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 CMD_Send(UINT8 Type, UINT8 INS, UINT8 *pData, UINT16 Datalen)
{
    UINT8 cRet;
    UINT16 len, nCRC;
    
    MakePackageHead(UartTxBuff, Type, Datalen+1);//��ͷ     ???DataLen + 1[INS take 1 byte length]
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
** Function    : �������ݰ����� 
** Author      : yf
** Input       : UINT8 Type     --   ����ʾ 
**             : UINT8 Datalen  -- ���ݳ��� 
**             : UINT8 pData    --  ����ָ�� 
** Output      : �ɹ�:CMD_OK  ʧ�ܣ�CMD_ERR 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 CMD_DataSend(UINT8 Type, UINT8 *pData, UINT16 Datalen)
{
    UINT8 cRet;
    UINT16 len, nCRC;
    
    MakePackageHead(UartTxBuff, Type, Datalen);//��ͷ
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
** Function    : ����Э��� 
** Author      : yf
** Input       : UINT8 *pPakege  --   Э�����ŵ�ַ 
**               UINT16 *pLen   --    Э������ȣ���ͷ��У������������ֽ����� 
**             : UINT8 TimeOutMs -- ���ܳ�ʱ����λms 
**             : UINT8 *ERR  --     Ӧ���ȷ����  
** Output      : �ɹ�:CMD_OK  ��ʱ��CMD_TIMEOUT  У��ʹ���CMD_SUM_ERR
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
UINT8 CommPakgeRev(UINT8 *pPakege,UINT16 *pLen,UINT16 TimeOutMs,UINT8 *ERR)
{
    UINT16 i,len,CRCSUM;
    UINT8 cRet;
    UINT8 tBuf[16];
    //��ͷ���� 
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
                    return CMD_TIMEOUT;             //???Ӧ���ش����� 
                break;
            }            
        }        
    }while (1);
    //��ͷ���� 2��ͷ+4��ַ+1����ʾ+2���ݳ��� 
    len = (pPakege[7]<<8) + pPakege[8];//���ֽ���ǰ���ֽ��ں� 
    cRet = UartDataRev(&pPakege[9], len, TimeOutMs);//����ʣ���ֽ����� 
    if (cRet != RT_OK)
        return CMD_TIMEOUT;
    
    //У��� 
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
    return	CMD_OK;//����ʣ���ֽ����� 
}

/***************************************************************************************************
** Subroutine  : CommDataPakgeRev
** Function    : ���ݰ����ܺ��������յ����Ǵ����� 
** Author      : yf
** Input       : UINT8 *pData  --   �����ݴ�ŵ�ַ 
**               UINT16 *plen  --   ���ݳ��ȣ�������
**             : UINT8 TimeOutMs -- ���ܳ�ʱ����λms 
**             : UINT8 *ERR  --     Ӧ���ȷ����  
** Output      : �ɹ�:CMD_OK  ��ʱ��CMD_TIMEOUT  У��ʹ���CMD_SUM_ERR  �շ����߼�����CMD_LOGIC_ERR 
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
/*
1.����->�ӻ�   0x01 ����
2.�ӻ�->����   0x07 ����Ӧ�� 
3.�ӻ�->����   0x02 ���ݰ� 
4.�ӻ�->����   0x02 ���ݰ� 
...
n.�ӻ�->����   0x08 ���һ�����ݰ� 
*/
UINT8 CommDataPakgeRev(UINT8 *pData,UINT16 *plen,UINT16 TimeOutMs,UINT8 *ERR)
{
    UINT8 cRet,temp;
    UINT16 len,len1,lenPkg;
    cRet=CommPakgeRev(pData,&lenPkg, TimeOutMs,ERR);
    if (cRet!=CMD_OK)
        return cRet;
    //��ͷ���� 2��ͷ+4��ַ+1����ʾ+2���ݳ��� 
    if (pData[6]==0x07)
    {
    	if (*ERR!=0)
    	   return CMD_COMM_ERR;//Ӧ������� 
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
        	   return CMD_COMM_ERR;//Ӧ������� 
        	return CMD_LOGIC_ERR; //�߼����� 
        }        
    }while (1);   
}

/***************************************************************************************************
** Subroutine  : CommSingleInstruction
** Function    : ��һָ��������˺��������������ָ����һվʽЭ�飬���ݽ��յ�ָ�� 
** Author      : yf
** Input       : UINT8 Type             ����ʾ 
**               UINT8 INS              ������ 
**               UINT8 *pDataIn         ��������������Ժ�У������ǰ������ 
**               UINT16 *Datalen [I/O]  ���ݳ���,���ؽ������ݰ��ĳ��ȣ��������ȣ��� 
**               UINT8 *pDataOut:       �������ݰ� 
**               UINT8 *pErr:           Ӧ���ȷ���� 
**               UINT16 TimeOutMs:      ָ��ȴ�Ӧ��ʱ 
** Output      : �ɹ�:CMD_OK  ʧ�ܣ�CMD_COMM_ERR 
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
		return CMD_ERR;//����ʹ��� 
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
** Function    : ������������������ݵ�ģ�飬�������ļ��ȡ���ָ�������ڷ������� 
** Author      : yf
** Input       : UINT8 Type ����ʾ 
**               UINT8 INS ������ 
**               UINT8 *pDataIn �·������ݣ������ݣ� 
**               UINT16 *Datalen ���ݳ���,���ؽ������ݰ��ĳ��ȣ��������ȣ��� 
**               UINT8 *pInsData: ����ָ������ 
**               UINT8 InsDataLen ָ�����ݳ��� 
**               UINT8 *pErr:    Ӧ���ȷ���� 
**               UINT16 TimeOutMs: ָ��ȴ�Ӧ��ʱ 
** Output      : �ɹ�:CMD_OK  ʧ�ܣ�CMD_COMM_ERR 
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
    		return CMD_ERR;//����ʹ��� 
    	}
        	
    	cRet = CommPakgeRev(Buf, &lenPkg, TimeOutMs, &ERR);
        if (cRet != CMD_OK)
            return cRet;
    	if (ERR==0x00)
    	{
    		break;
    	}    	
    	else //����������ģ�鷱æ����һ���ٳ��� 
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
** Function    : �������������ģ������������������ļ��ȡ���ָ�������ڷ������� 
** Author      : yf
** Input       : UINT8 Type ����ʾ 
**               UINT8 INS ������ 
**               UINT8 *pDataOut ��ȡ�����ݣ������ݣ���ŵ�ַ 
**               UINT16 *Datalen ���ݳ���,���ؽ������ݰ��ĳ��ȣ��������ȣ���  
**               UINT8 *pInsParaIn: ������� 
**               UINT8 *pErr:    Ӧ���ȷ���� 
**               UINT16 TimeOutMs: ָ��ȴ�Ӧ��ʱ 
** Output      : �ɹ�:CMD_OK  ʧ�ܣ�CMD_COMM_ERR 
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
    		return CMD_ERR;//����ʹ��� 
    	}
        	
    	cRet=CommPakgeRev(Buf, &lenPkg, TimeOutMs, &ERR);    	
        if (cRet!=CMD_OK)
            return cRet;
    	
        if (ERR==0x00)
    	{
    		break;
    	}    	
    	else //����������ģ�鷱æ����һ���ٳ��� 
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
** Function    : ģ�����֣���ģ���ϵ��Ժ���Ҫ��ģ������һ��һ��ͨ���ȶ� 
** Author      : yf
** Input       : UINT8 nTime  --   ���ֳ��Դ��� 
 
** Output      : �ɹ�:CMD_OK  ʧ�ܣ�CMD_COMM_ERR 
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




