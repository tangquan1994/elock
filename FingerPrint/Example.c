#include "Prtype.h"
#include "Uartport.h"
#include "Protocol.h"


/***************************************************************************************************
** Subroutine  : FunHandshakeModule
** Function    : 注册指纹例程 
** Author      : yf
** Input       : UINT8 nTime  --   注册时需要按几次手指 
**               UINT16 wID   --   指纹ID号    
** Output      : 成功:CMD_OK  失败：CMD_ERR,超时：CMD_TIMEOUT  
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
/*
1.主机->从机:发送采图命令采指纹命令 
2.从机->主机:返回采集指纹结果。 
3.如果没有采集到指纹循环1.2步骤直到采集到指纹 或超时 
4.提取特征
5.重复1到4，如果要求注册3次需要重复5操作
6.合成模板
8.保存指纹 
*/
UINT8 ExampleErollFp(UINT8 nTime,UINT16 wID)
{
    UINT8 cRet, ERR, nErrTime, n;
    UINT8 Buf[128];
    UINT16 lenPkg;

//0.先握手一下，看模块是否工作正常 
    cRet = FunHandshakeModule(1);
    if (cRet != CMD_OK)
    {
    	//模块不正常，再次尝试一下 
         cRet = FunHandshakeModule(1);
        if (cRet != CMD_OK)
        {
        	//模块不正常
            return  RT_OVERTIME;   
        } 
    }  
    
    n = 0;   
    while(n < nTime)
    {
        //1.采指纹  	指令代码 29H 
        nErrTime = 0;
        do
        {
            SY_memset(Buf, 0, 128);
            lenPkg = 0;
            cRet=CommSingleInstruction(MASK_INS, PS_GetEnrollImage, NULL,&lenPkg, Buf,&ERR,500);        
        	if (cRet==CMD_OK&&ERR==0)//采集到指纹 
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
            //超时判断
            if (0)//超时退出 
            {
                return CMD_TIMEOUT;	
            }                   	
        }while (1); 
        //1.提取特征  	指令代码 02H
        Buf[0]= n+1; 
        lenPkg=1;
        cRet=CommSingleInstruction(MASK_INS, PS_GenChar, Buf,&lenPkg, Buf,&ERR,1000);       
        if (cRet==CMD_OK) 
    	{           
            if (ERR==0)    //取特征成功 
            {	
            } 
            else  if (ERR==1)    //表示收包有错 
            {	
                continue;
            } 
            else  if (ERR==0x06)    //表示指纹图像太乱而生不成特征 
            {	
                continue;
            } 
            else  if (ERR==0x07)    //表示指纹图像正常，但特征点太少而生不成特征 
            {	
                continue;
            } 
            else  if (ERR==0x28)    //表示当前指纹模板与之前模板之间有关联
            {	
            }       
            else  if (ERR==0x15)    //表示图像缓冲区内没有有效原始图而生不成图像
            {	
                continue;
            }        
    	}
    	else //提取特征失败 
    	{
    		return  CMD_STEP_ERR;
    	}    	
    	n++;
    }  
    //合并模板 指令代码 05H
    lenPkg=0;
    cRet=CommSingleInstruction(MASK_INS, PS_RegModel, NULL,&lenPkg, Buf,&ERR,50);
    if (cRet==CMD_OK&&ERR==0) //合并模板成功
	{                
	}
	else //合并模板失败 
	{
		return  CMD_STEP_ERR;
	} 
	 //存储模板 指令代码 06H
	Buf[0]= 1; //默认为1
    Buf[1]= (wID>>8); 
    Buf[2]= (wID&0xff);
    lenPkg=3;
    cRet=CommSingleInstruction(MASK_INS,0x06, Buf,&lenPkg, Buf,&ERR,100); 
    if (cRet==CMD_OK&&ERR==0) //注册成功 
	{                
        return CMD_OK; 
	}
	else //注册失败 
	{
		return  CMD_STEP_ERR;
	}   
}




/***************************************************************************************************
** Subroutine  : ExampleSeachFp
** Function    : 注册指纹例程 
** Author      : yf
** Input       : UINT8 nTime  --   注册时需要按几次手指 
**               UINT16 wID   --   指纹ID号    
** Output      : 成功:CMD_OK  失败：CMD_ERR,超时：CMD_TIMEOUT  
** Description :   
** Date        : 2016/10/26
** ModifyRecord:
***************************************************************************************************/
/*
1.主机->从机:发送采图命令采指纹命令 
2.从机->主机:返回采集指纹结果。 
3.如果没有采集到指纹循环1.2步骤直到采集到指纹 或超时 
4.提取特征
5.搜索 
*/
UINT8 ExampleSeachFp(UINT16 *wID)
{
    UINT8 cRet,ERR,nErrTime,n;
    UINT8 Buf[128];
    UINT16 lenPkg,sSoucre,FpId;
    //1.采指纹  	指令代码 29H 
    nErrTime=0;
    do
    {
        SY_memset(Buf,0,128);
        lenPkg=0;
        cRet=CommSingleInstruction(MASK_INS,0x29, NULL,&lenPkg, Buf,&ERR,500);
    
    	if (cRet==CMD_OK&&ERR==0)//采集到指纹 
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
        //超时判断
        if (0)//超时退出 
        {
            return CMD_TIMEOUT;	
        }                   	
    }while (1); 
    //1.提取特征  	指令代码 02H
    Buf[0]= 1; 
    lenPkg=1;
    cRet=CommSingleInstruction(MASK_INS,0x02, Buf,&lenPkg, Buf,&ERR,1000);       
    if (cRet==CMD_OK&&ERR==0) //取特征成功
	{                
	}
	else //提取特征失败 
	{
		return  CMD_STEP_ERR;
	}    	

    //搜索 指令代码 1BH
    Buf[0]= 1;
    Buf[1]= 0;
    Buf[2]= 0;
    Buf[3]= (1024>>8);
    Buf[4]= (1024&0xff);
    lenPkg=5;
    cRet=CommSingleInstruction(MASK_INS,0x1B, Buf,&lenPkg, Buf,&ERR,2000);
    if (cRet==CMD_OK&&ERR==0) //搜索成功
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
	else //合并模板失败 
	{
		return  CMD_STEP_ERR;
	} 

}

/***************************************************************************************************
** Subroutine  : FptEx_SaveChar
** Function    : 传感器获取指纹并生成特征上传给上位机 
** Author      : Comon
** Input       : UINT8 nTime  --   注册时需要按几次手指 
**               UINT16 wID   --   指纹ID号    
** Output      : 成功:CMD_OK  失败：CMD_ERR,超时：CMD_TIMEOUT  
** Description :   
** Date        : 2016/11/9
** ModifyRecord:
***************************************************************************************************/
/*
1.主机->从机:发送采图命令采指纹命令 
2.从机->主机:返回采集指纹结果。 
3.如果没有采集到指纹循环1.2步骤直到采集到指纹 或超时 
4.提取特征
5.搜索 
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
    nOverTime = 500;     //设定范围如何确定 
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
    //1.order如何定义；2.数据分包大小；3.发包数据总长度；4. 
//    for(i = 0; i < tLen/padLen; i++)
//    {
//        CommUpDataInstruction(MASK_DAT, MASK_DAT, Buf, &padLen, Buf, &cRerr, nOverTime);
//    	
//    }    
//    CommUpDataInstruction(MASK_DAT_LAST, MASK_DAT_LAST, Buf, &(tLen % padLen), );
    
    
//接收后续数据包？

	return 0;
}    

UINT8 FptEx_AutoEnroll()
{
    UINT8 cRet, cRerr;
    UINT8 Buf[128];
    UINT16 nBufLen = 0;
    UINT16 n = 0, nLevel = 10;
    UINT16 nOutTimeMS = 500;
    //指令合法性，是否握手？ 
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
       
       Buf[0] = n;  //BuffID 取值规则，范围 
       Buf[1] = n & 0xff;       //PageID取值规则，范围
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
        
    //指纹重复检测成功？？？GetEnroll指令？
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
