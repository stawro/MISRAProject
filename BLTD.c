/*
 * BLTD.c
 *
 * Created: 31/07/2015 12:35:22 ص
 *  Author: hossam
 */ 
#include "Basic_Types.h"
#include "UART_Drv.h"
#include "BLTD.h"


static u8 IsRespRecieved = 0U;
static u8 RxBuffer[100];
static void BTCommandSend(const u8 Command[],u16 CommandLength);
static  void MemCpy(u8 Des[],const u8 Src[],u16 Length,u8 StartIndex1,u8 StartIndex2);
static void RxcCallBackFun(void);
static u8 MemCmp(const u8 Src1[],const u8 Src2[],u16 CmpLength);
/***************************************************************************************************************/
void BLTD_SendInitCmd(void)
{	IsRespRecieved = 0U;
	UART_StartReception(RxBuffer,4U,&RxcCallBackFun);
	BTCommandSend("+INIT",5U);
}
/***************************************************************************************************************/
void BLTD_SendInquireCmd(void)
{	IsRespRecieved = 0U;
	UART_StartReception(RxBuffer,4U,&RxcCallBackFun);
	BTCommandSend("+INQ",4U);
}
u8 BLTD_CheckForResponse(const u8* Response,u16 RespLength)
{
	u8 RespStatus;
	u8 IsEqual;
	if(IsRespRecieved == 1U)
	{
		IsRespRecieved = 0U;
		IsEqual = MemCmp(Response,RxBuffer,RespLength);
		if(IsEqual == 0U)
		{
			RespStatus = BLTD_RESP_STATUS_OK;
			
		}
		else
		{
			RespStatus = BLTD_RESP_STATUS_NOK;
		}
		
	}
	else
	{

		RespStatus = BLTD_RESP_STATUS_NON;
	}
	return RespStatus;
}	
/***************************************************************************************************************/
void BLTD_StartWaitPairing(void)
{
	UART_StopRception();
	UART_StartReception(RxBuffer,4U,&RxcCallBackFun);
	/*BTCommandSend(0,0);*/
	
}
/***************************************************************************************************************/	
void BLTD_SendMessage(const u8 *Message,u16 MsgLength)
{
	UART_TxBuffer(Message,MsgLength);
}	
/***************************************************************************************************************/
u8 BLTD_GetRecievedData(u8 Data[], u16 Length)
{
	u8 RespStatus2;
	u8 i;
	if(IsRespRecieved == 1U)
	{
		IsRespRecieved = 0U;
		RespStatus2 = BLTD_RESP_STATUS_OK;
		for( i = 0U; i< Length ; i++)
		{
			Data[i] = RxBuffer[i];
		}
	}
	else
	{
		RespStatus2 = BLTD_RESP_STATUS_NON;
	}		
		
	return RespStatus2;
}
/***************************************************************************************************************/
void BLTD_StartReceivingData(u8* DataBuffer,u16 BufferLength,CbkPfnType CbkFnPtr)
{
	UART_StartReception(DataBuffer,BufferLength,CbkFnPtr);
	
}
/***************************************************************************************************************/
u8 BLTD_CheckForData(u8 *Data2)
{
	u16 RxBytesNum;
	u8 IsReceived;
	RxBytesNum = UART_GetNumOfRxbytes();
	if(RxBytesNum > 0U)
	{
		IsReceived = 0x01U;
		*Data2 = RxBuffer[RxBytesNum+1U];
		UART_StopRception();
		UART_StartReception(RxBuffer,100U,&RxcCallBackFun);
	}
	else
	{
		IsReceived = 0x00U;
		*Data2  = 0U;
	}
	return IsReceived;
}

/***************************************************************************************************************/	
void BLTD_SenTestCmd(void)
{
	UART_StartReception(RxBuffer,4U,&RxcCallBackFun);
	BTCommandSend((u8*)0,0U);
	
}
/***************************************************************************************************************/
static void BTCommandSend(const u8 Command[],u16 CommandLength)
	{
    static u8 BTCommandBuffer[100];
		BTCommandBuffer[0] = (u8)'A';
		BTCommandBuffer[1] = (u8)'T';
		MemCpy(BTCommandBuffer,Command,CommandLength,2U,0U);
		BTCommandBuffer[CommandLength+2U] = 0x0dU;
		BTCommandBuffer[CommandLength+3U] = 0x0aU;
		UART_TxBuffer(BTCommandBuffer,CommandLength + 4U);
		
	}
/***************************************************************************************************************/
static  void MemCpy(u8 Des[],const u8 Src[],u16 Length,u8 StartIndex1,u8 StartIndex2)
	{
	u16 i3;
	for(i3 = 0U ; i3<Length ; i3++)
		{
		Des[i3+StartIndex1] = Src[i3+StartIndex2];
		}
	}
/***************************************************************************************************************/	
static void RxcCallBackFun(void)
{

	IsRespRecieved = 1U;
}
/***************************************************************************************************************/
static u8 MemCmp(const u8 Src1[],const u8 Src2[],u16 CmpLength)
	{
	u8 RetVal = 0U;
	u16 i2;
	for(i2 = 0U ;(i2 < CmpLength); i2++)
		{
		if(Src1[i2] != Src2[i2])
			{
			RetVal = 1U;
			break;
			}
		}
	return RetVal;
	}
/***************************************************************************************************************/
