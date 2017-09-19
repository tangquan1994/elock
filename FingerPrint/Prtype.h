#ifndef PROTOCOLTYPE_H_
#define PROTOCOLTYPE_H_

//------- BOOL type definition --------------
#ifndef	_BOOL_TYPE_
#define	_BOOL_TYPE_
typedef	unsigned char 	BOOL;
// #define FALSE	0
// #define TRUE	1
// #define false 	0
// #define true  	1
#endif

//------- UINT8 type definition --------------
#ifndef	_UINT8_TYPE_
#define _UINT8_TYPE_
typedef unsigned char  UINT8;
#endif

//------- UINT16 type definition --------------
#ifndef	_UINT16_TYPE_
#define _UINT16_TYPE_
typedef unsigned short 	UINT16;
#endif

//------- UINT32 type definition --------------
#ifndef	_UINT32_TYPE_
#define _UINT32_TYPE_
typedef unsigned long  UINT32;
#endif

//------- UINT64 type definition --------------
#ifndef	_UINT64_TYPE_
#define _UINT64_TYPE_
typedef unsigned long long UINT64;
#endif


//-------- INT8 type definition --------------
#ifndef	_INT8_TYPE_
#define _INT8_TYPE_
typedef char  INT8;
#endif

//-------- INT16 type definition --------------
#ifndef	_INT16_TYPE_
#define _INT16_TYPE_
typedef short 	INT16;
#endif

//-------- INT32 type definition --------------
#ifndef	_INT32_TYPE_
#define _INT32_TYPE_
typedef long  INT32;
#endif

//------- INT64 type definition --------------
#ifndef	_INT64_TYPE_
#define _INT64_TYPE_
typedef long long INT64;
#endif

//------- VUINT8 type definition --------------
#ifndef	_VUINT8_TYPE_
#define _VUINT8_TYPE_
typedef volatile unsigned char  VUINT8;
#endif

//------- VUINT16 type definition --------------
#ifndef	_VUINT16_TYPE_
#define _VUINT16_TYPE_
typedef volatile unsigned short 	VUINT16;
#endif

#define NULL 0
#define RT_OK            0x00 //success
#define RT_FAIL          0x01  //fail
#define RT_OVERTIME      0x04  //over time



#endif
