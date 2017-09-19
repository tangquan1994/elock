#include "Prtype.h"
#include "Uartport.h"
#include "Protocol.h"


/***************************************************************************************************
** Subroutine  : FunHandshakeModule
** Function    : ע��ָ������ 
** Author      : yf
** Input       : UINT8 nTime  --   ע��ʱ��Ҫ��������ָ 
**               UINT16 wID   --   ָ��ID��    
** Output      : �ɹ�:CMD_OK  ʧ�ܣ�CMD_ERR,��ʱ��CMD_TIMEOUT  
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
/*
1.����->�ӻ�:���Ͳ�ͼ�����ָ������ 
2.�ӻ�->����:���زɼ�ָ�ƽ���� 
3.���û�вɼ���ָ��ѭ��1.2����ֱ���ɼ���ָ�� ��ʱ 
4.��ȡ����
5.�ظ�1��4�����Ҫ��ע��3����Ҫ�ظ�5����
6.�ϳ�ģ��
8.����ָ�� 
*/
UINT8 ExampleErollFp(UINT8 nTime,UINT16 wID)
{
    UINT8 cRet, ERR, nErrTime, n;
    UINT8 Buf[128];
    UINT16 lenPkg;

//0.������һ�£���ģ���Ƿ������� 
    cRet = FunHandshakeModule(1);
    if (cRet != CMD_OK)
    {
    	//ģ�鲻�������ٴγ���һ�� 
         cRet = FunHandshakeModule(1);
        if (cRet != CMD_OK)
        {
        	//ģ�鲻����
            return  RT_OVERTIME;   
        } 
    }  
    
    n = 0;   
    while(n < nTime)
    {
        //1.��ָ��  	ָ����� 29H 
        nErrTime = 0;
        do
        {
            SY_memset(Buf, 0, 128);
            lenPkg = 0;
            cRet=CommSingleInstruction(MASK_INS, PS_GetEnrollImage, NULL,&lenPkg, Buf,&ERR,500);        
        	if (cRet==CMD_OK&&ERR==0)//�ɼ���ָ�� 
        	{
        		break;
        	}        	
        	else if (cRet==CMD_TIMEOUT)
        	{
        		return CMD_TIMEOUT;
        	}	
        	else //CMD_SUM_ERR
        	{
        		
        	} 
            //��ʱ�ж�
            if (0)//��ʱ�˳� 
            {
                return CMD_TIMEOUT;	
            }                   	
        }while (1); 
        //1.��ȡ����  	ָ����� 02H
        Buf[0]= n+1; 
        lenPkg=1;
        cRet=CommSingleInstruction(MASK_INS, PS_GenChar, Buf,&lenPkg, Buf,&ERR,1000);       
        if (cRet==CMD_OK) 
    	{           
            if (ERR==0)    //ȡ�����ɹ� 
            {	
            } 
            else  if (ERR==1)    //��ʾ�հ��д� 
            {	
                continue;
            } 
            else  if (ERR==0x06)    //��ʾָ��ͼ��̫�Ҷ����������� 
            {	
                continue;
            } 
            else  if (ERR==0x07)    //��ʾָ��ͼ����������������̫�ٶ����������� 
            {	
                continue;
            } 
            else  if (ERR==0x28)    //��ʾ��ǰָ��ģ����֮ǰģ��֮���й���
            {	
            }       
            else  if (ERR==0x15)    //��ʾͼ�񻺳�����û����Чԭʼͼ��������ͼ��
            {	
                continue;
            }        
    	}
    	else //��ȡ����ʧ�� 
    	{
    		return  CMD_STEP_ERR;
    	}    	
    	n++;
    }  
    //�ϲ�ģ�� ָ����� 05H
    lenPkg=0;
    cRet=CommSingleInstruction(MASK_INS, PS_RegModel, NULL,&lenPkg, Buf,&ERR,50);
    if (cRet==CMD_OK&&ERR==0) //�ϲ�ģ��ɹ�
	{                
	}
	else //�ϲ�ģ��ʧ�� 
	{
		return  CMD_STEP_ERR;
	} 
	 //�洢ģ�� ָ����� 06H
	Buf[0]= 1; //Ĭ��Ϊ1
    Buf[1]= (wID>>8); 
    Buf[2]= (wID&0xff);
    lenPkg=3;
    cRet=CommSingleInstruction(MASK_INS,0x06, Buf,&lenPkg, Buf,&ERR,100); 
    if (cRet==CMD_OK&&ERR==0) //ע��ɹ� 
	{                
        return CMD_OK; 
	}
	else //ע��ʧ�� 
	{
		return  CMD_STEP_ERR;
	}   
}




/***************************************************************************************************
** Subroutine  : ExampleSeachFp
** Function    : ע��ָ������ 
** Author      : yf
** Input       : UINT8 nTime  --   ע��ʱ��Ҫ��������ָ 
**               UINT16 wID   --   ָ��ID��    
** Output      : �ɹ�:CMD_OK  ʧ�ܣ�CMD_ERR,��ʱ��CMD_TIMEOUT  
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
/*
1.����->�ӻ�:���Ͳ�ͼ�����ָ������ 
2.�ӻ�->����:���زɼ�ָ�ƽ���� 
3.���û�вɼ���ָ��ѭ��1.2����ֱ���ɼ���ָ�� ��ʱ 
4.��ȡ����
5.���� 
*/
UINT8 ExampleSeachFp(UINT16 *wID)
{
    UINT8 cRet,ERR,nErrTime,n;
    UINT8 Buf[128];
    UINT16 lenPkg,sSoucre,FpId;
    //1.��ָ��  	ָ����� 29H 
    nErrTime=0;
    do
    {
        SY_memset(Buf,0,128);
        lenPkg=0;
        cRet=CommSingleInstruction(MASK_INS,0x29, NULL,&lenPkg, Buf,&ERR,500);
    
    	if (cRet==CMD_OK&&ERR==0)//�ɼ���ָ�� 
    	{
    		break;
    	}        	
    	else if (cRet==CMD_TIMEOUT)
    	{
    		return CMD_TIMEOUT;
    	}	
    	else //CMD_SUM_ERR
    	{
    		
    	} 
        //��ʱ�ж�
        if (0)//��ʱ�˳� 
        {
            return CMD_TIMEOUT;	
        }                   	
    }while (1); 
    //1.��ȡ����  	ָ����� 02H
    Buf[0]= 1; 
    lenPkg=1;
    cRet=CommSingleInstruction(MASK_INS,0x02, Buf,&lenPkg, Buf,&ERR,1000);       
    if (cRet==CMD_OK&&ERR==0) //ȡ�����ɹ�
	{                
	}
	else //��ȡ����ʧ�� 
	{
		return  CMD_STEP_ERR;
	}    	

    //���� ָ����� 1BH
    Buf[0]= 1;
    Buf[1]= 0;
    Buf[2]= 0;
    Buf[3]= (1024>>8);
    Buf[4]= (1024&0xff);
    lenPkg=5;
    cRet=CommSingleInstruction(MASK_INS,0x1B, Buf,&lenPkg, Buf,&ERR,2000);
    if (cRet==CMD_OK&&ERR==0) //�����ɹ�
	{
        if (lenPkg==16) 
        {
        	sSoucre=(Buf[12]<<8)+Buf[13];
        	FpId=(Buf[10]<<8)+Buf[11];
        	*wID=FpId;
        	return  CMD_OK;
        }                    
        return CMD_LOGIC_ERR ;  
	}
	else //�ϲ�ģ��ʧ�� 
	{
		return  CMD_STEP_ERR;
	} 

}

/***************************************************************************************************
** Subroutine  : FptEx_SaveChar
** Function    : ��������ȡָ�Ʋ����������ϴ�����λ�� 
** Author      : Comon
** Input       : UINT8 nTime  --   ע��ʱ��Ҫ��������ָ 
**               UINT16 wID   --   ָ��ID��    
** Output      : �ɹ�:CMD_OK  ʧ�ܣ�CMD_ERR,��ʱ��CMD_TIMEOUT  
** Description :   
** Date        : 2016/11/9
** ModifyRecord:
***************************************************************************************************/
/*
1.����->�ӻ�:���Ͳ�ͼ�����ָ������ 
2.�ӻ�->����:���زɼ�ָ�ƽ���� 
3.���û�вɼ���ָ��ѭ��1.2����ֱ���ɼ���ָ�� ��ʱ 
4.��ȡ����
5.���� 
*/
UINT8 FptEx_SaveChar()
{
    UINT8 cRet, cRerr;
    UINT16 nOverTime, n;
    UINT8 Buf[128];
    UINT16 nInBufLen = 0;
    UINT16 i;
//    UINT16 sSoucre, FpId;
    
    SY_memset(Buf, 0, sizeof(Buf));
    nOverTime = 500;     //�趨��Χ���ȷ�� 
    while(1)
    {
        cRet = CommSingleInstruction(MASK_INS, PS_GetImage, NULL, &nInBufLen, Buf, &cRerr, nOverTime);
        if(cRet == CMD_TIMEOUT)
            return cRet;
        else if((cRet != CMD_OK) || (cRerr != 0))
            continue;
        
        Buf[0] = 1; 
        nInBufLen = 1;    
        cRet = CommSingleInstruction(MASK_INS, PS_GenChar, Buf, &nInBufLen, Buf, &cRerr, nOverTime);
        if(cRet == CMD_TIMEOUT)
            return cRet;
        else if((cRet != CMD_OK) || (cRerr != 0))
            continue;
        else 
            break;   	
    }    

    do{
        Buf[0] = 1; 
        nInBufLen = 1;
        cRet = CommSingleInstruction(MASK_INS, PS_UpChar, Buf, &nInBufLen, Buf, &cRerr, nOverTime);
        if(cRet == CMD_TIMEOUT)
            return cRet;
        else if((cRet != CMD_OK) || (cRerr != 0))
            continue;
        else 
            break;    
    }while(1);
    //1.order��ζ��壻2.���ݷְ���С��3.���������ܳ��ȣ�4. 
//    for(i = 0; i < tLen/padLen; i++)
//    {
//        CommUpDataInstruction(MASK_DAT, MASK_DAT, Buf, &padLen, Buf, &cRerr, nOverTime);
//    	
//    }    
//    CommUpDataInstruction(MASK_DAT_LAST, MASK_DAT_LAST, Buf, &(tLen % padLen), );
    
    
//���պ������ݰ���

	return 0;
}    

UINT8 FptEx_AutoEnroll()
{
    UINT8 cRet, cRerr;
    UINT8 Buf[128];
    UINT16 nBufLen = 0;
    UINT16 n = 0, nLevel = 10;
    UINT16 nOutTimeMS = 500;
    //ָ��Ϸ��ԣ��Ƿ����֣� 
    cRet = FunHandshakeModule(1);
    if(cRet != CMD_OK)
        return CMD_ERR;	
    
    while(n < nLevel)
    {
        n++;
    	cRet = CommSingleInstruction(MASK_INS, PS_GetEnrollImage, Buf, &nBufLen, Buf, &cRerr, nOutTimeMS);
    	if(cRet == CMD_TIMEOUT)
            return cRet;
        else if((cRet != CMD_OK) || (cRerr != 0))
            continue;
        
        Buf[0] = n;
        nBufLen = 1;
        cRet = CommSingleInstruction(MASK_INS, PS_GenChar, Buf, &nBufLen, Buf, &cRerr, nOutTimeMS);
        if(cRet == CMD_TIMEOUT)
            return cRet;
        else if((cRet != CMD_OK) || (cRerr != 0))
            continue;
       
       Buf[0] = n;  //BuffID ȡֵ���򣬷�Χ 
       Buf[1] = n & 0xff;       //PageIDȡֵ���򣬷�Χ
       Buf[2] = (n>>8) & 0xff;
       nBufLen = 3;
       cRet = CommSingleInstruction(MASK_INS, PS_StoreChar, Buf, &nBufLen, Buf, &cRerr, nOutTimeMS);
       if(cRet == CMD_TIMEOUT)
       {
       	    return cRet;
       }       
       else if((cRet != CMD_OK) || (cRerr != 0))
            continue;            
    } 
    
    cRet = CommSingleInstruction(MASK_INS, PS_RegModel, Buf, &nBufLen, Buf, &cRerr, nOutTimeMS);
    if((cRet != CMD_OK) || (cRet != 0))
        return cRet;
        
    //ָ���ظ����ɹ�������GetEnrollָ�
    cRet = CommSingleInstruction(MASK_INS, PS_GetEnrollImage, Buf, &nBufLen, Buf, &cRerr, nOutTimeMS);
    if((cRet != CMD_OK) || (cRerr != 0))
    {
    	return cRet;
    } 
    
    Buf[0] = 1;
    nBufLen = 1; 
    cRet = CommSingleInstruction(MASK_INS, PS_UpChar, Buf, &nBufLen, Buf, &cRerr, nOutTimeMS);
    if((cRet != CMD_OK) || (cRerr != 0))
        return cRet;
    else
        return CMD_OK; 
}

UINT8 FptEx_AutoVery()
{
	UINT8 cRet, cRerr;
	UINT8 Buf[128];
	UINT16 nBufLen = 0;
	UINT16 nOutTimeMS = 500;
	
	cRet = FunHandshakeModule(1);
	if(cRet != CMD_OK)
		return CMD_ERR;
	
	cRet = CommSingleInstruction(MASK_INS, PS_GetEnrollImage, Buf, &nBufLen, Buf, &cRerr, nOutTimeMS);
	if((cRet != CMD_OK) || (cRerr != 0))
		return cRet;
	
	Buf[0] = 1;
	nBufLen = 1;
	cRet = CommSingleInstruction(MASK_INS, PS_GenChar, Buf, &nBufLen, Buf, &cRerr, nOutTimeMS);
	if((cRet != CMD_OK) || (cRerr != 0))
		return cRet;
	
	cRet = CommSingleInstruction(MASK_INS, PS_Match, Buf, &nBufLen, Buf, &cRerr, nOutTimeMS);
	if((cRet != CMD_OK) || (cRerr != 0))
	   return cRet;
	else 
	   return CMD_OK;
}
