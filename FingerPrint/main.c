#include "global.h"


void main()
{        
    UINT8 cRet;
    UINT32 lRFIDCardID;    
    UINT16 wId;
    SCM_SetSysClk(2,SYS_CLK_240MHz);        //test k2en is  enable 
    Flash_Init(2, DUAL);
    ScmRegs.CK_DISABLE0.bit.Uart0 = 0;	// disable UART0 clock
    ScmRegs.SOFT_RST0.bit.Uart0 = 0;	// reset
    ScmRegs.CK_DISABLE0.bit.Uart0 = 1;	// enable UART0 clock
    ScmRegs.SOFT_RST0.bit.Uart0 = 1;	// Relinquish from reset
  //  ConfigAreaSize(0x8000,0x8000);
    
    Port_Init();
    
    _enable();
    cRet=FunHandshakeModule(100);
   lRFIDCardID=0;
//   cRet=ExampleErollFp( 4,1);
   _nop();
    while(1)
    {
         lRFIDCardID++;
       _nop();
//   cRet=FunHandshakeModule(1);
//   _delay_ms(100);
   if (cRet==0)
   {
   	_nop();
   }   
   _nop();
        _nop();
 //      cRet=ExampleErollFp( 2,1); 
      cRet= ExampleSeachFp(&wId);
       _nop();
       
       
    }  
} 

