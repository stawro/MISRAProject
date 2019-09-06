#include "Basic_Types.h"
#include "BLTD.h"
#include "BLMGR.h"
#include "CRC.h"
#include "DIO.h"
#include "UART_Drv.h"
#include "BLMGR_CFG.h"

/*********************************************************************************/
/*Local Symbols*/
/*********************************************************************************/
/*Paring States*/
#define PAIRING_STATE_IDLE                  0xffU
#define PAIRING_STATE_INITIALIZING          0x00U
#define PAIRING_STATE_WAIT_INIT_RESP        0x01U
#define PAIRING_STATE_INQUIRE               0x02U
#define PAIRING_STATE_WAIT_INQUIRE_RESP     0x03U
#define PAIRING_STATE_WAIT_PAIR_REQ         0x04U
#define PAIRING_STATE_CONNECTED_DONE        0x05U
#define PAIRING_STATE_FAILED                0x06U
#define PAIRING_STATE_START_WAIT_PAIR_REQ   0x07U
/*********************************************************************************/
/*Handshaking states*/
#define HANDSHAKE_STATE_IDLE             0xffU
#define HANDSHAKE_STATE_SEND_ID_FRAME    0x01U
#define HANDSHAKE_STATE_RECV_ID_FRAME    0x02U
#define HANDSHAKE_STATE_SEND_VAL_FRMAE   0x03U
#define HANDSHAKE_STATE_RECV_VAL_FRAME   0x04U
#define HANDSHAKE_STATE_SEND_ERR_FRAME   0x05U
#define HANDSHAKE_STATE_HANDSHAKING_DONE 0x06U
#define HANDSHAKE_STATE_FAILED           0x07U
/*********************************************************************************/
/*Communication states*/
#define COMM_STATE_IDLE             0xffU
#define COMM_STATE_SEND_DATA_FRAME  0x01U
#define COMM_STATE_RECV_DATA_FRAME  0x02U
#define COMM_STATE_SEND_ERROR_FRAME 0x03U
#define COMM_STATE_FAILED           0x04U
/*********************************************************************************/
/*Bluetooth States*/
#define BLUETOOTH_STATE_STOPPED       0xffU
#define BLUETOOTH_STATE_DISCONNECTED  0x00U
#define BLUETOOTH_STATE_PAIRING       0x01U
#define BLUETOOTH_STATE_HANDSHAKING   0x02U
#define BLUETOOTH_STATE_COMMUNICATION 0x03U
/*********************************************************************************/
/*Lengths Configuration*/
#define ID_FRAME_LENGTH        18U
#define VAL_FRAME_LENGTH       18U
#define DATA_FRAME_LENGTH      18U
#define ERROR_FRAME_LENGTH     18U
#define MAX_DEV_NAME_LENGTH    6U
#define MAX_DATA_BUFFER_LENGTH 18
/*********************************************************************************/
/*Timeout Counts Configuration*/
#define PAIRING_MAX_COUNTS     1U
#define HANDSHAKING_MAX_COUNTS 10U
#define COMM_MAX_COUNTS        10U
#define MAX_PAIRING_FAIL_REPT  10U
#define MAX_HANDSHAKE_FAIL_REP 5U
#define MAX_COMM_FAIL_REP      20U
#define MAX_DISCONNECTION_COUNT 2U
/*********************************************************************************/
/*Error States*/
#define ERRH_TIMEOUT_ERROR             0x00U
#define ERRH_HANDSHAKE_ERROR           0x01U
#define ERRH_CHECKSUM_ERROR            0x03U
#define ERRH_INVALID_FRAME_ERROR       0x04U
#define ERRH_CRC_ERROR                 0x05U
#define ERRH_INVALID_RECEIVER_ERROR    0x06U
#define ERRH_WRONG_FRAME_ERROR         0x07U
#define ERRH_NO_ERROR                  0xffU
/*********************************************************************************/
/*Buffer Indices*/
#define FRAME_HEADER_IDX   0x00U
#define FRAME_SENDER_IDX   0x02U
#define FRAME_RECVER_IDX   0x03U
#define FRAME_TYPE_IDX     0x04U
#define PARAM_LENGTH_IDX   0x05U
#define OS_TYPE_IDX        0x06U
#define DEV_TYPE_IDX       0x07U
#define DEV_NAME_IDX       0x08U
#define FRAME_CRC_IDX      0x0EU
#define FRAME_CHECKSUM_IDX 0x10U
#define FRAME_TAIL_IDX     0x11U
#define FRAME_VAL_CRC_IDX  0x06U
#define BATT_LEVEL_IDX     0x06U
#define DIRECTION_IDX      0x06U
#define SPEED_DEGREE_IDX   0x07U
#define ERROR_TYPE_IDX     0x06U
/*********************************************************************************/
/*Default Values*/
#define TX_OS_TYPE       0xffU
#define TX_DEV_TYPE      0x04U
#define TX_FRAME_DEFUALT 0x00U
#define TX_CRC_DEFAULT   0xffU
/*********************************************************************************/
/*Frame types*/
#define FRAME_TYPE_ID_FRAME    0x01U
#define FRAME_TYPE_VAL_FRAME   0x02U
#define FRAME_TYPE_DATA_FRAME  0x03U
#define FRAME_TYPE_ERROR_FRAME 0x04U
/*********************************************************************************/
/*Error Types*/
#define ERROR_TYPE_RESEND_LAST_FRMAE     0x01U
#define ERROR_TYPE_START_NEW_HANDSHAKE   0x02U
#define ERROR_TYPE_UPDATE_UR_TRANSMITTER 0x03U
/*********************************************************************************/
/*Private functions*/
/*********************************************************************************/
/*State machines*/
static void PairingInit(void);
static void PairingStateMachine(void);
static void ErrorHandlingStateMachine(void);
static void HandShakeInit(void);
static void HandShakingStateMachine(void);
static void CommunicationInit(void);
static void CommStateMachine(void);
static void DisconnectInit(void);
static void DisconnectStateMachine(void);
/*********************************************************************************/
/*Frames handlers*/
static void UpdateIdFrame(void);
static u8   CheckIdFrame(void);
static void UpdateValFrame(void);
static u8   CheckValFrame(void);
static void UpdateDataFrame(void);
static u8 CheckDataFrame(void);
static void UpdateErrorFrame(u8 ErrorType);
static void CheckErrorFrame(void);
/*********************************************************************************/
/*Utilities*/
static void RxBufferDnCallBackNotif(void);
static void MemCpy( u8 DesPtr[], const u8 SrcPtr[], u16 Length,u8 StartIndex1,u8 StartIndex2);
static void MemSet(u8 DesPtr[], u8 ConstVal, u16 Length,u8 StartIndex3);
#if (COMM_CINFIG == SLAVE_COMM)
static u8 MemCmp(const u8* Src1Ptr,const u8* Src2Ptr,u16 Length);
static u8 GetCrcKey(void);
#endif
static u8 CalculateCheksum(const u8 BufferPtr[], u16 BufferLength);
static void BuzzerSound(void);
static u8 CheckCrc(void);
static void PowerBluetoothOn(void);
static void PowerBluetoothOff(void);
static void BuzzerInit(void);
static void PowerBlueToothInit(void);
static void BlueToothKeyInit(void);
static void InserBreakPoint(void);
/*********************************************************************************/
/*Static variables*/
/*********************************************************************************/
static u8  BLMGR_PairingState;
static u32 BLMGR_PairingTimeoutCounter;
static u8  BLMGR_HandShakeState;
static u8  BLMGR_HandShakeTimeOutCounter;
static u8  BLMGR_DataRxBuffer[MAX_DATA_BUFFER_LENGTH];
static u8  BLMGR_DataTxBuffer[MAX_DATA_BUFFER_LENGTH];
static u8  BLMGR_FrameReceivedNotificationFlag;
static u8  BLMGR_ErrorState;
static u8  BLMGR_RxOsType;
static u8  BLMGR_RxDeviceType;
static u8  BLMGR_RxDevicName[MAX_DEV_NAME_LENGTH];
static u8  BLMGR_TxDevicName[MAX_DEV_NAME_LENGTH];
static u8  BLMGR_RxDeviceNameLength;
static u16  BLMGR_TxDeviceNameLength;
static u8  BLMGR_TxFrameReceiver;
static u8  BLMGR_RxFrameSender;
static u32 BLMGR_CrcKey;
static u8  BLMGR_CommState;
static u8  BLMGR_CommTimeOutCounter;
static u8  BLMGR_TxBattLevel;
static u8  BLMGR_RxDirection;
static u8  BLMGR_RxSpeedDegree;
#if (COMM_CINFIG == SLAVE_COMM)
static u8  BLMGR_TxDirection;
static u8  BLMGR_TxSpeedDegree;
static u8  BLMGR_RxBattLevel;
#endif
static u8  BLMGR_BluetoothState;
static u8  BLMGR_BluetoothStartRequest;
static u8  BLMGR_StopDeviceRequest;
static u8  BLMGR_PairingFailRepetionCount;
static u8  BLMGR_HandshakeFailRepCount;
static u8  BLMGR_CommFailReptCount;
static u8  BLMGR_ExpectedReceivedFrame;
static u8  BLMGR_DisconectionTimeOut;
static u8 testflag = 0U;
static u8 BLMGR_DevicePaired;
/*********************************************************************************/
/*Global Services*/
/*********************************************************************************/
void BLMGR_StartDevice(void)
{
	BLMGR_BluetoothStartRequest = 1U;
}
/*********************************************************************************/
void BLMGR_Test(void)
{
	BuzzerSound();

	
	
}

/*********************************************************************************/
void BLMGR_BluetoothInit(void)
{
	/*Init UART*/
	UART_Init();
	/*Init State*/
	BLMGR_BluetoothState = BLUETOOTH_STATE_STOPPED;
	BLMGR_BluetoothStartRequest = 0U;
	BLMGR_StopDeviceRequest = 0U;
	BLMGR_DevicePaired = 0U;
	/*Init Pairing*/
	PairingInit();
	/*Init Handshaking*/
	HandShakeInit();
	/*Init Communication*/
	CommunicationInit();
	/*Disconnection Init*/
	DisconnectInit();
	/*Init Buzzer*/
	BuzzerInit();
	/*Init Bluetooth Power Control*/
	PowerBlueToothInit();
	/*Init Key*/
	BlueToothKeyInit();
	
}
/*********************************************************************************/
void BLMGR_BluetoothStateMachine(void)
{
	switch(BLMGR_BluetoothState)
	{
		case BLUETOOTH_STATE_STOPPED:
		{
			/*Check if application need to start bluetooth*/
			if(BLMGR_BluetoothStartRequest == 1U)
			{
				/*Power On the module*/
				PowerBluetoothOn();
				PairingInit();
				BLMGR_BluetoothState = BLUETOOTH_STATE_PAIRING;
				
			}
			break;
		}

		case BLUETOOTH_STATE_PAIRING:
		{
			PairingStateMachine();
			if(BLMGR_PairingState == PAIRING_STATE_CONNECTED_DONE)
			{
				/*BLMGR_Test();*/
				/*Pairing succeeded, start handshaking*/
				HandShakeInit();
				BLMGR_BluetoothState = BLUETOOTH_STATE_HANDSHAKING;
			}
			else if(BLMGR_PairingState == PAIRING_STATE_FAILED)
			{
				/*Pairing failed, disconnect the module*/
				PairingInit();
				BLMGR_BluetoothState = BLUETOOTH_STATE_DISCONNECTED;
			}
			else
			{
				/*pairing is in progress*/
			}

	        break;
		}
		case BLUETOOTH_STATE_HANDSHAKING:
		{
			HandShakingStateMachine();
			if(BLMGR_HandShakeState == HANDSHAKE_STATE_HANDSHAKING_DONE)
			{
				/*Handshake succeeded, start communication*/
				CommunicationInit();
				BLMGR_BluetoothState = BLUETOOTH_STATE_COMMUNICATION;
				BuzzerSound();
			}
			else if (BLMGR_HandShakeState == HANDSHAKE_STATE_FAILED)
			{
				/*handshake failed, disconnect the module*/
				HandShakeInit();
				BLMGR_BluetoothState = BLUETOOTH_STATE_DISCONNECTED;
			}
			else
			{
				/*handshake still in progress*/
			}

	        break;
		}
		case BLUETOOTH_STATE_COMMUNICATION:
		{
			CommStateMachine();
			if(BLMGR_CommState == COMM_STATE_FAILED)
			{
				/*Disconnect the module*/
				CommunicationInit();
				BLMGR_BluetoothState = BLUETOOTH_STATE_DISCONNECTED;
			}
			else
			{
				/*Communication is in progress*/
				
			}

	        break;
		}

		case BLUETOOTH_STATE_DISCONNECTED:
		{
			DisconnectStateMachine();
			/*Check if application need to start bluetooth*/
			if(BLMGR_BluetoothStartRequest == 1U)
			{
				/*Power On the module*/
				/*PowerBluetoothOn();*/
				PairingInit();
				/*PowerBluetoothOff();*/
				/*InserBreakPoint();*/
				BLMGR_PairingState = PAIRING_STATE_WAIT_PAIR_REQ;
				if(BLMGR_DisconectionTimeOut > MAX_DISCONNECTION_COUNT)
				{
					BLMGR_BluetoothState = BLUETOOTH_STATE_PAIRING;
					BLMGR_DisconectionTimeOut = 0U;
					
				}
				else
				{
					/*	BuzzerSound();*/
					BLMGR_DisconectionTimeOut ++;
				}
				
			}
			else if (BLMGR_StopDeviceRequest == 1U)
			{
				/*PowerBluetoothOff();*/
				DisconnectInit();
				BLMGR_BluetoothState = BLUETOOTH_STATE_STOPPED;
			}
			else
			{
				/*Still disconnected*/
			}

	        break;
		}
		default:
		    break;
	}
}
/*********************************************************************************/
/*Private Funcions definitions*/
/*********************************************************************************/
static void PairingInit(void)
{
	BLMGR_PairingState = PAIRING_STATE_IDLE;
	BLMGR_PairingTimeoutCounter = 0U;
	BLMGR_PairingFailRepetionCount = 0U;
	/*BLMGR_DevicePaired = 0;*/
}
/*********************************************************************************/
static void HandShakeInit(void)
{
	BLMGR_HandShakeState = HANDSHAKE_STATE_IDLE;
	BLMGR_PairingTimeoutCounter = 0U;
	BLMGR_FrameReceivedNotificationFlag = 0U;
	BLMGR_HandshakeFailRepCount = 0U;
}
/*********************************************************************************/
static void CommunicationInit(void)
{
	BLMGR_CommState = COMM_STATE_IDLE;
	BLMGR_CommTimeOutCounter = 0U;
	BLMGR_CommFailReptCount = 0U;
}
/*********************************************************************************/

static void PairingStateMachine(void)
{
	u8 ResponseState;
	
	if(BLMGR_DevicePaired == 1U)
	{
		/*InserBreakPoint();*/
		BLMGR_PairingState = PAIRING_STATE_START_WAIT_PAIR_REQ;
		BLMGR_DevicePaired = 0U;
	}
	switch(BLMGR_PairingState)
	{
		case PAIRING_STATE_IDLE:
		{
			/*wait for 1 second for stabilization*/
			if(BLMGR_PairingTimeoutCounter > PAIRING_MAX_COUNTS)
			{
				BLMGR_PairingTimeoutCounter = 0U;
				/*go to the init state*/
				BLMGR_PairingFailRepetionCount = 0U;
				BLMGR_PairingState = PAIRING_STATE_INITIALIZING;
				
				
			}
			else
			{
				BLMGR_PairingTimeoutCounter ++;
			}

	        break;
		}

		case PAIRING_STATE_INITIALIZING:
		{
			/*send init Command*/
			BLTD_SendInitCmd();
			/*Go to next state to read response*/
			BLMGR_PairingState = PAIRING_STATE_WAIT_INIT_RESP;

	        break;
		}

		case PAIRING_STATE_WAIT_INIT_RESP:
		{
			u8 RespArray[4];
			RespArray[0] = (u8)'O';
			RespArray[1] = (u8)'K';
			RespArray[2] = 0x0dU;
			RespArray[3] = 0x0aU;
			
			ResponseState = BLTD_CheckForResponse(RespArray,4U);
			switch(ResponseState)
			{
				case BLTD_RESP_STATUS_OK:
				BLMGR_PairingFailRepetionCount = 0U;
				BLMGR_PairingTimeoutCounter = 0U;
				/*Respnse recieved and go to send the inquire request*/
				BLMGR_PairingState = PAIRING_STATE_INQUIRE;
				
				break;

				case BLTD_RESP_STATUS_NOK:
				
				if(BLMGR_PairingFailRepetionCount <= MAX_PAIRING_FAIL_REPT)
				{
					/*response received not ok so re send the command again*/
					BLMGR_PairingState = PAIRING_STATE_INITIALIZING;
					BLMGR_PairingFailRepetionCount ++;
					
				}
				else
				{
					BLMGR_PairingFailRepetionCount = 0U;
					BLMGR_PairingState = PAIRING_STATE_INQUIRE;
				}

				break;

				case BLTD_RESP_STATUS_NON:
				
				/*response not received and wait until timeout*/
				BLMGR_PairingTimeoutCounter ++;
				if(BLMGR_PairingTimeoutCounter > PAIRING_MAX_COUNTS)
				{
					if(BLMGR_PairingFailRepetionCount <= MAX_PAIRING_FAIL_REPT)
					{
						BLMGR_PairingFailRepetionCount ++;
						BLMGR_PairingTimeoutCounter = 0U;
						BLMGR_PairingState = PAIRING_STATE_INITIALIZING;
					}
					else
					{
						BLMGR_PairingFailRepetionCount = 0U;
						BLMGR_PairingState = PAIRING_STATE_FAILED;
					}
				}
				else
				{
					BLMGR_PairingTimeoutCounter ++;
					BLMGR_PairingState = PAIRING_STATE_WAIT_INIT_RESP;
				}
				break;
				default:
				    break;
			}

	        break;
		}
		case PAIRING_STATE_INQUIRE:
		{
			
			/*Send Inquire Command*/
			BLTD_SendInquireCmd();
			/*wait for the Inquire Response*/
			BLMGR_PairingState = PAIRING_STATE_WAIT_INQUIRE_RESP;

	        break;
		}
		case PAIRING_STATE_WAIT_INQUIRE_RESP:
		{
			u8 RespArray2[4];
			RespArray2[0] = (u8)'O';
			RespArray2[1] = (u8)'K';
			RespArray2[2] = 0x0dU;
			RespArray2[3] = 0x0aU;
			ResponseState = BLTD_CheckForResponse(RespArray2,4U);
			switch(ResponseState)
			{
				case BLTD_RESP_STATUS_OK:
				BLMGR_PairingFailRepetionCount = 0U;
				BLMGR_PairingTimeoutCounter = 0U;
				/*Respnse recieved and go to send the inquire request*/
				BLMGR_PairingState = PAIRING_STATE_START_WAIT_PAIR_REQ;

				break;

				case BLTD_RESP_STATUS_NOK:
				if(BLMGR_PairingFailRepetionCount <= MAX_PAIRING_FAIL_REPT)
				{
					BLMGR_PairingFailRepetionCount ++;
					/*response received not ok so re send the command again*/
					BLMGR_PairingState = PAIRING_STATE_WAIT_INQUIRE_RESP;
				}
				else
				{
					BLMGR_PairingFailRepetionCount = 0U;
					BLMGR_PairingState = PAIRING_STATE_INITIALIZING;
				}
				break;

				case BLTD_RESP_STATUS_NON:
				/*response not received and wait until timeout*/
				BLMGR_PairingTimeoutCounter ++;
				if(BLMGR_PairingTimeoutCounter > PAIRING_MAX_COUNTS)
				{
					if(BLMGR_PairingFailRepetionCount <= MAX_PAIRING_FAIL_REPT)
					{
						BLMGR_PairingFailRepetionCount ++;
						BLMGR_PairingTimeoutCounter = 0U;
						BLMGR_PairingState = PAIRING_STATE_INQUIRE;
					}
					else
					{
						BLMGR_PairingFailRepetionCount = 0U;
						BLMGR_PairingState = PAIRING_STATE_FAILED;
					}
				}
				else
				{
					BLMGR_PairingTimeoutCounter ++;
					BLMGR_PairingState = PAIRING_STATE_WAIT_INQUIRE_RESP;
				}
				break;
				default:
				    break;
			}

	        break;
		}
		case PAIRING_STATE_START_WAIT_PAIR_REQ:
		BLTD_StartWaitPairing();
		BuzzerSound();
		BLMGR_PairingState = PAIRING_STATE_WAIT_PAIR_REQ;
		break;
		case PAIRING_STATE_WAIT_PAIR_REQ:
		{
			u8 RespArray3[4];
			RespArray3[0] = (u8)'O';
			RespArray3[1] = (u8)'K';
			RespArray3[2] = 0x0dU;
			RespArray3[3] = 0x0aU;
			ResponseState = BLTD_CheckForResponse(RespArray3,4U);
			switch(ResponseState)
			{
				case BLTD_RESP_STATUS_OK:
				BLMGR_PairingFailRepetionCount = 0U;
				BLMGR_PairingTimeoutCounter = 0U;
				/*Respnse recieved and go to send the inquire request*/
				BLMGR_PairingState = PAIRING_STATE_CONNECTED_DONE;
				/*BuzzerSound();*/
				BLMGR_DevicePaired = 1U;
				break;

				case BLTD_RESP_STATUS_NOK:
				if(BLMGR_PairingFailRepetionCount <= MAX_PAIRING_FAIL_REPT)
				{
					BLMGR_PairingFailRepetionCount ++;
					/*response received not ok so re send the command again*/
					BLMGR_PairingState = PAIRING_STATE_INQUIRE;
				}
				else
				{
					BLMGR_PairingFailRepetionCount = 0U;
					BLMGR_PairingState = PAIRING_STATE_FAILED;
				}
				break;

				case BLTD_RESP_STATUS_NON:
				/*response not received and wait until timeout*/
				BLMGR_PairingState = PAIRING_STATE_WAIT_PAIR_REQ;
			/*  BuzzerSound(); */
				break;
				default:
				  break;
			}

	        break;
		}
		default:
		break;
	}
}
/*********************************************************************************/
static void HandShakingStateMachine(void)
{
	u8 IsFrameValid1;
	switch (BLMGR_HandShakeState)
	{
		case HANDSHAKE_STATE_IDLE:
		{
			/*Check for the Device Comm Mode*/
			#if(COMM_CINFIG == MSTER_COMM)
			/* the device will master the communication*/
			BLMGR_HandShakeState = HANDSHAKE_STATE_SEND_ID_FRAME;
			#elif(COMM_CINFIG == SLAVE_COMM)
			/*the Device will be mastered by the other device*/
			BLTD_StartReceivingData(BLMGR_DataRxBuffer,ID_FRAME_LENGTH,RxBufferDnCallBackNotif);
			BLMGR_HandShakeState = HANDSHAKE_STATE_RECV_ID_FRAME;
			#else
			/*Wrong Config, State in Idle*/
			/*To do: Managing dev Errors*/
			#endif

	        break;
		}
		case HANDSHAKE_STATE_SEND_ID_FRAME:
		{
			/*Update the ID frame by  loading tx data buffer*/
			UpdateIdFrame();
			#if(COMM_CINFIG == MSTER_COMM)
			/*Start Receiving the slave response*/
			BLTD_StartReceivingData(BLMGR_DataRxBuffer,ID_FRAME_LENGTH,&RxBufferDnCallBackNotif);
			BLMGR_ExpectedReceivedFrame = FRAME_TYPE_ID_FRAME;
			BLMGR_HandShakeState = HANDSHAKE_STATE_RECV_ID_FRAME;
			#elif(COMM_CINFIG == SLAVE_COMM)
			/*Start receiving Validation frame*/
			BLTD_StartReceivingData(BLMGR_DataRxBuffer,VAL_FRAME_LENGTH,RxBufferDnCallBackNotif);
			BLMGR_ExpectedReceivedFrame = FRAME_TYPE_VAL_FRAME;
			BLMGR_HandShakeState = HANDSHAKE_STATE_RECV_VAL_FRAME;
			#else
			/*Wrong Config, State in Idle*/
			/*To do: Managing dev Errors*/
			#endif
			/*Send the ID Frame*/
			BLTD_SendMessage(BLMGR_DataTxBuffer,ID_FRAME_LENGTH);

	        break;
		}
		case HANDSHAKE_STATE_RECV_ID_FRAME:
		{
			/*Check that a frame was received*/
			if(BLMGR_FrameReceivedNotificationFlag == 1U)
			{
				BLMGR_FrameReceivedNotificationFlag = 0U;
				BLMGR_HandShakeTimeOutCounter = 0U;
				IsFrameValid1 = CheckIdFrame();
				if(IsFrameValid1 == 1U)
				{
					BLMGR_HandshakeFailRepCount = 0U;
					/*Frame is valid*/
					#if(COMM_CINFIG == MSTER_COMM)
					/*Send the Validation frame*/
					BLTD_StartReceivingData(BLMGR_DataRxBuffer,VAL_FRAME_LENGTH,&RxBufferDnCallBackNotif);
					BLMGR_HandShakeState = HANDSHAKE_STATE_HANDSHAKING_DONE;
					#elif(COMM_CINFIG == SLAVE_COMM)
					/*Start receiving validation frame*/
					BLTD_StartReceivingData(BLMGR_DataRxBuffer,VAL_FRAME_LENGTH,RxBufferDnCallBackNotif);
					BLMGR_HandShakeState = HANDSHAKE_STATE_RECV_VAL_FRAME;
					#else
					/*Wrong Config, State in Idle*/
					/*To do: Managing dev Errors*/
					#endif
				}
				else
				{
					/*Frame is invalid*/
					BLMGR_HandShakeState = HANDSHAKE_STATE_SEND_ERR_FRAME;
				}
			}
			else
			{
				/*No frame Received, Check Timeout*/
				if(BLMGR_HandShakeTimeOutCounter > HANDSHAKING_MAX_COUNTS)
				{
					/*Handle Timeout Error*/
					BLMGR_HandShakeTimeOutCounter = 0U;
					BLMGR_ErrorState = ERRH_TIMEOUT_ERROR;
					BLMGR_HandShakeState = HANDSHAKE_STATE_SEND_ERR_FRAME;
				}
				else
				{
					BLMGR_HandShakeTimeOutCounter ++;
				}
			}

	        break;
		}
		case HANDSHAKE_STATE_SEND_VAL_FRMAE:
		{
			/*Prepare Validation frame*/
			UpdateValFrame();
			/*Sending Validation frame*/
			BLTD_StartReceivingData(BLMGR_DataRxBuffer,VAL_FRAME_LENGTH,&RxBufferDnCallBackNotif);
			BLTD_SendMessage(BLMGR_DataTxBuffer,VAL_FRAME_LENGTH);
			#if(COMM_CINFIG == MSTER_COMM)
			BLMGR_ExpectedReceivedFrame = FRAME_TYPE_VAL_FRAME;
			BLMGR_HandShakeState = HANDSHAKE_STATE_RECV_VAL_FRAME;
			#elif(COMM_CINFIG == SLAVE_COMM)
			BLMGR_HandShakeState = HANDSHAKE_STATE_HANDSHAKING_DONE;
			#else
			/*Wrong Config, State in Idle*/
			/*To do: Managing dev Errors*/
			#endif

	        break;
		}
		case HANDSHAKE_STATE_RECV_VAL_FRAME:
		{
			/*Check that a frame was received*/
			if(BLMGR_FrameReceivedNotificationFlag == 1U)
			{
				BLMGR_FrameReceivedNotificationFlag = 0U;
				BLMGR_HandShakeTimeOutCounter = 0U;
				IsFrameValid1 = CheckValFrame();
				if(IsFrameValid1 == 1U)
				{
					BLMGR_HandshakeFailRepCount = 0U;
					#if(COMM_CINFIG == MSTER_COMM)
					/*Master the Communication phase*/
					BLMGR_HandShakeState = HANDSHAKE_STATE_HANDSHAKING_DONE;
					#elif(COMM_CINFIG == SLAVE_COMM)
					BLMGR_HandShakeState = HANDSHAKE_STATE_SEND_VAL_FRMAE;
					/*Start Receiving validation frame*/
					BLTD_StartReceivingData(BLMGR_DataRxBuffer,VAL_FRAME_LENGTH,&RxBufferDnCallBackNotif);
					#else
					/*Wrong Config, State in Idle*/
					/*To do: Managing dev Errors*/
					#endif
				}
				else
				{
					/*Handle validation error*/
					BLMGR_ErrorState = ERRH_HANDSHAKE_ERROR;
					BLMGR_HandShakeState = HANDSHAKE_STATE_SEND_ERR_FRAME;
				}
			}
			else
			{
				/*No frame Received, Check Timeout*/
				if(BLMGR_HandShakeTimeOutCounter > HANDSHAKING_MAX_COUNTS)
				{
					/*Handle Timeout Error*/
					BLMGR_HandShakeTimeOutCounter = 0U;
					BLMGR_ErrorState = ERRH_TIMEOUT_ERROR;
					BLMGR_HandShakeState = HANDSHAKE_STATE_SEND_ERR_FRAME;
				}
				else
				{
					BLMGR_HandShakeTimeOutCounter ++;
				}
			}

	        break;
		}
		case HANDSHAKE_STATE_SEND_ERR_FRAME:
		{
			ErrorHandlingStateMachine();

	        break;
		}
		default:
		break;
	}
}
/*********************************************************************************/
static void CommStateMachine(void)
{
	u8 IsFrameValid2;
	switch (BLMGR_CommState)
	{
		case COMM_STATE_IDLE:
		{
			#if(COMM_CINFIG == MSTER_COMM)
			/*Start Sending Data frame*/
			BLMGR_CommState = COMM_STATE_SEND_DATA_FRAME;
			#elif(COMM_CINFIG == SLAVE_COMM)
			/*Start Receiving data frame*/
			BLMGR_ExpectedReceivedFrame = FRAME_TYPE_DATA_FRAME;
			BLMGR_CommState = COMM_STATE_RECV_DATA_FRAME;
			BLTD_StartReceivingData(BLMGR_DataRxBuffer,DATA_FRAME_LENGTH,RxBufferDnCallBackNotif);
			#else
			/*Wrong Config, State in Idle*/
			/*To do: Managing dev Errors*/
			#endif

	        break;
		}
		case COMM_STATE_SEND_DATA_FRAME:
		{
			/*Update Data Frame*/
			UpdateDataFrame();
			/*Start Receiving data frame*/
			BLTD_StartReceivingData(BLMGR_DataRxBuffer,DATA_FRAME_LENGTH,&RxBufferDnCallBackNotif);
			BLMGR_ExpectedReceivedFrame = FRAME_TYPE_DATA_FRAME;
			BLMGR_CommState = COMM_STATE_RECV_DATA_FRAME;
			/*Send the Data Frame*/
			BLTD_SendMessage(BLMGR_DataTxBuffer,DATA_FRAME_LENGTH);

	        break;
		}
		case COMM_STATE_RECV_DATA_FRAME:
		{
			/*Check that a frame was received*/
			if(BLMGR_FrameReceivedNotificationFlag == 1U)
			{
				BLMGR_FrameReceivedNotificationFlag = 0U;
				BLMGR_HandShakeTimeOutCounter = 0U;
				BLMGR_CommTimeOutCounter = 0U;
				/*Check Received data frame*/
				IsFrameValid2 = CheckDataFrame();
				if(IsFrameValid2 == 1U)
				{
					BLMGR_CommFailReptCount = 0U;
					BLMGR_CommState = COMM_STATE_SEND_DATA_FRAME;
				}
				else
				{
					BuzzerSound();
					BLMGR_CommFailReptCount = 0U;
					/*Handshaking failed*/
					BLMGR_CommState = COMM_STATE_FAILED;
				}
			}
			else
			{
				
				if(BLMGR_CommTimeOutCounter > COMM_MAX_COUNTS)
				{
					
					/*InserBreakPoint();*/
					/*Handle Timeout Error*/
					BLMGR_CommTimeOutCounter = 0U;
					BLMGR_ErrorState = ERRH_TIMEOUT_ERROR;
					BLMGR_CommState = COMM_STATE_SEND_ERROR_FRAME;
				}
				else
				{
					BLMGR_CommTimeOutCounter ++;
				}
			}

	        break;
		}
		case COMM_STATE_SEND_ERROR_FRAME:
		{
			
			ErrorHandlingStateMachine();

	        break;
		}
		default:
		break;
	}

}

/*********************************************************************************/
static void BuzzerSound(void)
{
	u8 LoopIndex;
	for(LoopIndex = 0U; LoopIndex < 2U ; LoopIndex ++)
	{
		DIO_WritePort(BuzzerConfig.Portname,BUZEER_ON,BuzzerConfig.Portmask);
		_delay_ms(25);
		DIO_WritePort(BuzzerConfig.Portname,BUZEER_ON,BuzzerConfig.Portmask);
		_delay_ms(25);
		
	}

}
/*********************************************************************************/
static void RxBufferDnCallBackNotif(void)
{

	BLMGR_FrameReceivedNotificationFlag = 1U;
	
}
/*********************************************************************************/
void BLMGR_SetReceiver(u8 Receiver)
{
	BLMGR_TxFrameReceiver = Receiver;
}
/*********************************************************************************/
void BLMGR_SetDeviceName(u8 const DeviceName[],u16 DeviceNameLength)
{
	MemCpy(BLMGR_TxDevicName,DeviceName,DeviceNameLength,0U,0U);

	BLMGR_TxDeviceNameLength = DeviceNameLength;
}
/*********************************************************************************/

static void UpdateIdFrame(void)
{
	/*Set Tx Frame to default values*/
	MemSet(BLMGR_DataTxBuffer,TX_FRAME_DEFUALT,(u16)MAX_DATA_BUFFER_LENGTH,FRAME_HEADER_IDX);

	/*Set Header of Frame*/
	MemSet(BLMGR_DataTxBuffer,0xaaU,2U,FRAME_HEADER_IDX);
	/*Set Device Sender*/
	BLMGR_DataTxBuffer[FRAME_SENDER_IDX] = DEVICE_ROLE;
	/*Set Device Receiver*/
	BLMGR_DataTxBuffer[FRAME_RECVER_IDX] = BLMGR_TxFrameReceiver;
	/*Set frame type*/
	BLMGR_DataTxBuffer[FRAME_TYPE_IDX] = FRAME_TYPE_ID_FRAME;
	/*Set paramter length*/
	BLMGR_DataTxBuffer[PARAM_LENGTH_IDX] = 2 + BLMGR_TxDeviceNameLength;
	/*Update Os Type*/
	BLMGR_DataTxBuffer[OS_TYPE_IDX] = TX_OS_TYPE;
	/*Update Device Type*/
	BLMGR_DataTxBuffer[DEV_TYPE_IDX] = TX_DEV_TYPE;
	/*Update Device Name*/
	MemCpy(BLMGR_DataTxBuffer,BLMGR_TxDevicName,BLMGR_TxDeviceNameLength,DEV_NAME_IDX,0U);
	/*update Default CRC*/
	MemSet(BLMGR_DataTxBuffer,TX_CRC_DEFAULT,2U,FRAME_CRC_IDX);
	/*update Frame CheckSum*/
	BLMGR_DataTxBuffer[FRAME_CHECKSUM_IDX] = CalculateCheksum(BLMGR_DataTxBuffer,FRAME_CHECKSUM_IDX -1);
	/*update frame tail*/
	MemSet(BLMGR_DataTxBuffer,0x55U,1U,FRAME_TAIL_IDX);
}
/*********************************************************************************/
static u8 CheckIdFrame(void)
{
	u8 IsFrameValid3;
	u8 TempVar1;
	/* Perform a Checksum on the frame*/
	TempVar1 = CalculateCheksum(BLMGR_DataRxBuffer,FRAME_CHECKSUM_IDX -1U);

	if (TempVar1 == BLMGR_DataRxBuffer[FRAME_CHECKSUM_IDX])
	{
		
		/*Perform Start and end of frame validation*/
		if((BLMGR_DataRxBuffer[FRAME_HEADER_IDX] == 0xaaU) &&
		(BLMGR_DataRxBuffer[FRAME_HEADER_IDX + 1U] == 0xaaU) &&
		(BLMGR_DataRxBuffer[FRAME_TAIL_IDX] == 0x55U))
		{

			/*Validate Frame Type*/
			if(BLMGR_DataRxBuffer[FRAME_TYPE_IDX] == FRAME_TYPE_ID_FRAME)
			{

				/* Check CRC*/
				if((BLMGR_DataRxBuffer[FRAME_CRC_IDX] == TX_CRC_DEFAULT) &&
				(BLMGR_DataRxBuffer[FRAME_CRC_IDX] == TX_CRC_DEFAULT))
				{
					/*Validate Frame Receiver*/
					if(BLMGR_DataRxBuffer[FRAME_RECVER_IDX] == DEVICE_ROLE)
					{
						/*Validate Device Name*/
						BLMGR_RxDeviceNameLength = 8 - BLMGR_DataRxBuffer[PARAM_LENGTH_IDX];
						if(BLMGR_RxDeviceNameLength <= MAX_DEV_NAME_LENGTH)
						{
							
							/*Update received paramters*/
							/*Update Frame sender*/
							BLMGR_RxFrameSender = BLMGR_DataRxBuffer[FRAME_SENDER_IDX];
							/*Update OS Type*/
							BLMGR_RxOsType = BLMGR_DataRxBuffer[OS_TYPE_IDX];
							/*Update Device Type*/
							BLMGR_RxDeviceType = BLMGR_DataRxBuffer[DEV_TYPE_IDX];
							/*Update Device Name*/
							MemCpy(BLMGR_RxDevicName,BLMGR_DataRxBuffer,BLMGR_RxDeviceNameLength,0U,DEV_NAME_IDX);
							BLMGR_ErrorState = ERRH_NO_ERROR;
							IsFrameValid3 = 1U;
						}
						else
						{
							/*Invalid Frame receiver*/
							BLMGR_ErrorState = ERRH_INVALID_FRAME_ERROR;
							IsFrameValid3 = 0U;
						}
					}
					else
					{
						/*Invalid Frame receiver*/
						BLMGR_ErrorState = ERRH_INVALID_RECEIVER_ERROR;
						IsFrameValid3 = 0U;
					}
				}
				else
				{
					/*Crc Error Found*/
					BLMGR_ErrorState = ERRH_CRC_ERROR;
					IsFrameValid3 = 0U;
				}
			}
			else
			{
				/*Invalid Frame Type*/
				BLMGR_ErrorState = ERRH_WRONG_FRAME_ERROR;
				IsFrameValid3 = 0U;
			}
		}
		else
		{
			/*Invalid Frame Detected*/
			BLMGR_ErrorState = ERRH_INVALID_FRAME_ERROR;
			IsFrameValid3 = 0U;
		}
	}
	else
	{
		/*Checksum error*/
		BLMGR_ErrorState = ERRH_CHECKSUM_ERROR;
		IsFrameValid3 = 0U;
	}
	return IsFrameValid3;
}
/*********************************************************************************/
static void UpdateValFrame(void)
{
	u16 Crc=0U;
	u32 CrcKey=0U;
	static u8 TempBuffer[MAX_DATA_BUFFER_LENGTH];
	/*Set Tx Frame to default values*/
	MemSet(BLMGR_DataTxBuffer,TX_FRAME_DEFUALT,(u16)MAX_DATA_BUFFER_LENGTH,FRAME_HEADER_IDX);
	/*Set Header of Frame*/
	MemSet(BLMGR_DataTxBuffer,0xaaU,2U,FRAME_HEADER_IDX);
	/*Set Device Sender*/
	BLMGR_DataTxBuffer[FRAME_SENDER_IDX] = DEVICE_ROLE;
	/*Set Device Receiver*/
	BLMGR_DataTxBuffer[FRAME_RECVER_IDX] = BLMGR_TxFrameReceiver;
	/*Set frame type*/
	BLMGR_DataTxBuffer[FRAME_TYPE_IDX] = FRAME_TYPE_VAL_FRAME;
	/*Set paramter length*/
	BLMGR_DataTxBuffer[PARAM_LENGTH_IDX] = 2U;
	#if(COMM_CINFIG == MSTER_COMM)
	/* Start Generating the Key for CRC*/
	SECR_CrcPolynomialGenerate(&CrcKey,Crc);
	BLMGR_CrcKey = CrcKey;
	#endif
	/*Calculate CRC*/
	/*Prepare Data*/
	TempBuffer[0x00] = BLMGR_RxOsType;
	TempBuffer[0x01] = BLMGR_RxDeviceType;
	MemCpy(TempBuffer,BLMGR_RxDevicName,(u16)BLMGR_RxDeviceNameLength,0x02U,0U);

	SECR_GnerateCrc(TempBuffer,BLMGR_RxDeviceNameLength + 2u, &Crc,BLMGR_CrcKey);
	/*Update Crc*/
	BLMGR_DataTxBuffer[FRAME_VAL_CRC_IDX] = (u8)Crc;
	BLMGR_DataTxBuffer[FRAME_VAL_CRC_IDX + 1U] = (u8)(Crc >> 8);
	/*update Default CRC*/
	MemSet(BLMGR_DataTxBuffer,TX_CRC_DEFAULT,2U,FRAME_CRC_IDX);
	/*update Frame CheckSum*/
	BLMGR_DataTxBuffer[FRAME_CHECKSUM_IDX] = CalculateCheksum(BLMGR_DataTxBuffer,FRAME_CHECKSUM_IDX -1U);
	/*update frame tail*/
	MemSet(BLMGR_DataTxBuffer,0x55U,1U,FRAME_TAIL_IDX);
}
/*********************************************************************************/
static u8 CheckValFrame(void)
{
	u8 IsFrameValid4;
	u8 TempVar2;
	/* Perform a Checksum on the frame*/
	TempVar2 = CalculateCheksum(BLMGR_DataRxBuffer,(u16)FRAME_CHECKSUM_IDX -1U);
	if (TempVar2 == BLMGR_DataRxBuffer[FRAME_CHECKSUM_IDX])
	{
		/*Perform Start and end of frame validation*/
		if((BLMGR_DataRxBuffer[FRAME_HEADER_IDX] == 0xaaU) &&
		(BLMGR_DataRxBuffer[FRAME_HEADER_IDX + 1U] == 0xaaU) &&
		(BLMGR_DataRxBuffer[FRAME_TAIL_IDX] == 0x55U))
		{
			/*Validate Frame Type*/
			if(BLMGR_DataRxBuffer[FRAME_TYPE_IDX] == FRAME_TYPE_VAL_FRAME)
			{
				/* Check CRC*/
				if((BLMGR_DataRxBuffer[FRAME_CRC_IDX] == TX_CRC_DEFAULT) &&
				(BLMGR_DataRxBuffer[FRAME_CRC_IDX] == TX_CRC_DEFAULT))
				{
					/*Validate Frame Receiver*/
					if(BLMGR_DataRxBuffer[FRAME_RECVER_IDX] == DEVICE_ROLE)
					{
						#if(COMM_CINFIG == MSTER_COMM)
						/*Validate CRC */
						IsFrameValid4 = CheckCrc();


						#elif(COMM_CINFIG == SLAVE_COMM)
						/*Get the Crc Key*/
						IsFrameValid4 = GetCrcKey();
						#endif
					}
					else
					{
						/*Invalid Frame receiver*/
						BLMGR_ErrorState = ERRH_INVALID_RECEIVER_ERROR;
						IsFrameValid4 = 0U;
					}

				}
				else
				{
					/*Crc Error Found*/
					BLMGR_ErrorState = ERRH_CRC_ERROR;
					IsFrameValid4 = 0U;
				}
			}
			else
			{
				/*Invalid Frame Type*/
				BLMGR_ErrorState = ERRH_WRONG_FRAME_ERROR;
				IsFrameValid4 = 0U;
			}
		}
		else
		{
			/*Invalid Frame Detected*/
			BLMGR_ErrorState = ERRH_INVALID_FRAME_ERROR;
			IsFrameValid4 = 0U;
		}
	}
	else
	{
		/*Checksum error*/
		BLMGR_ErrorState = ERRH_CHECKSUM_ERROR;
		IsFrameValid4 = 0U;
	}
	return IsFrameValid4;
}
/*********************************************************************************/
void BLMGR_SetBattLevel(u8 BattLevel)
{
	BLMGR_TxBattLevel= BattLevel;
}

static void UpdateDataFrame(void)
{
	static u8 TempBuffer2[MAX_DATA_BUFFER_LENGTH];
	u16 Crc2=0U;
	/*Set Tx Frame to default values*/
	MemSet(BLMGR_DataTxBuffer,TX_FRAME_DEFUALT,(u16)MAX_DATA_BUFFER_LENGTH,FRAME_HEADER_IDX);
	/*Set Header of Frame*/
	MemSet(BLMGR_DataTxBuffer,0xaaU,2U,FRAME_HEADER_IDX);
	/*Set Device Sender*/
	BLMGR_DataTxBuffer[FRAME_SENDER_IDX] = DEVICE_ROLE;
	/*Set Device Receiver*/
	BLMGR_DataTxBuffer[FRAME_RECVER_IDX] = BLMGR_TxFrameReceiver;
	/*Set frame type*/
	BLMGR_DataTxBuffer[FRAME_TYPE_IDX] = FRAME_TYPE_DATA_FRAME;
	#if(COMM_CINFIG == MSTER_COMM)
	/*Set paramter length*/
	BLMGR_DataTxBuffer[PARAM_LENGTH_IDX] = 1U;
	/*Set Batterly level*/
	BLMGR_DataTxBuffer[BATT_LEVEL_IDX] = BLMGR_TxBattLevel;
	/*Calculate CRC*/
	MemCpy(TempBuffer2,BLMGR_DataTxBuffer,1U,0U,BATT_LEVEL_IDX);
	SECR_GnerateCrc(TempBuffer2,1U, &Crc2,BLMGR_CrcKey);



	#elif(COMM_CINFIG == SLAVE_COMM)
	/*Set paramter length*/
	BLMGR_DataTxBuffer[PARAM_LENGTH_IDX] = 2U;
	/*Set Direction*/
	BLMGR_DataTxBuffer[DIRECTION_IDX]= BLMGR_TxDirection;
	/*Set Speed degree*/
	BLMGR_DataTxBuffer[SPEED_DEGREE_IDX]= BLMGR_TxSpeedDegree;
	/*Calculate CRC*/
	MemCpy(TempBuffer2,BLMGR_DataTxBuffer,2U,0U,DIRECTION_IDX);
	SECR_GnerateCrc(TempBuffer2,2, &Crc2,BLMGR_CrcKey);
	#else
	/*Wrong Config, State in Idle*/
	/*To do: Managing dev Errors*/
	#endif
	/*Update Crc*/
	BLMGR_DataTxBuffer[FRAME_CRC_IDX] = (u8)Crc2;
	BLMGR_DataTxBuffer[FRAME_CRC_IDX + 1U] = (u8)(Crc2 >> 8U);
	/*update Frame CheckSum*/
	BLMGR_DataTxBuffer[FRAME_CHECKSUM_IDX] = CalculateCheksum(BLMGR_DataTxBuffer,FRAME_CHECKSUM_IDX -1U);
	/*update frame tail*/
	MemSet(BLMGR_DataTxBuffer,0x55U,1U,FRAME_TAIL_IDX);
}
/*********************************************************************************/
static u8 CheckDataFrame(void)
{
	static u8 TempBuffer3[MAX_DATA_BUFFER_LENGTH];
	u8 IsFrameValid5;
	u8 TempVar3;
	u16 GenCrc=0U;
	u16 RecvdCrc;
	/* Perform a Checksum on the frame*/
	TempVar3 = CalculateCheksum(BLMGR_DataRxBuffer,FRAME_CHECKSUM_IDX -1U);
	if (TempVar3 == BLMGR_DataRxBuffer[FRAME_CHECKSUM_IDX])
	{
		/*Perform Start and end of frame validation*/
		if((BLMGR_DataRxBuffer[FRAME_HEADER_IDX] == 0xaaU) &&
		(BLMGR_DataRxBuffer[FRAME_HEADER_IDX + 1U] == 0xaaU) &&
		(BLMGR_DataRxBuffer[FRAME_TAIL_IDX] == 0x55U))
		{
			/*Validate Frame Type*/
			if(BLMGR_DataRxBuffer[FRAME_TYPE_IDX] == FRAME_TYPE_DATA_FRAME)
			{
				/* Check CRC*/
				/*Calculate Crc from received data*/
				#if(COMM_CINFIG == MSTER_COMM)
				TempBuffer3[0x00] = BLMGR_DataRxBuffer[DIRECTION_IDX];
				TempBuffer3[0x01] = BLMGR_DataRxBuffer[SPEED_DEGREE_IDX];
				SECR_GnerateCrc(TempBuffer3, 2U,&GenCrc,BLMGR_CrcKey);

				#elif(COMM_CINFIG == SLAVE_COMM)
				TempBuffer3[0x00] = BLMGR_DataRxBuffer[BATT_LEVEL_IDX];
				SECR_GnerateCrc(TempBuffer3, 1, &GenCrc,BLMGR_CrcKey);
				#else
				/*Wrong Config, State in Idle*/
				/*To do: Managing dev Errors*/
				#endif
				/*Read Received CRC*/
				RecvdCrc = 0x00U;
				RecvdCrc = BLMGR_DataRxBuffer[FRAME_CRC_IDX];
				RecvdCrc |= ((BLMGR_DataRxBuffer[FRAME_CRC_IDX + 1U]) << 8U);

				/*Compare the Two Crcs*/
				/*if(GenCrc == RecvdCrc)*/
				{
					/*Validate Frame Receiver*/
					if(BLMGR_DataRxBuffer[FRAME_RECVER_IDX] == DEVICE_ROLE)
					{
						/*Update received paramters*/
						/*Update Frame sender*/
						BLMGR_RxFrameSender = BLMGR_DataRxBuffer[FRAME_SENDER_IDX];
						#if(COMM_CINFIG == MSTER_COMM)
						/*Update Direction*/
						BLMGR_RxDirection = BLMGR_DataRxBuffer[DIRECTION_IDX];
						/*Update Speed degree*/
						BLMGR_RxSpeedDegree = BLMGR_DataRxBuffer[SPEED_DEGREE_IDX];
						#elif(COMM_CINFIG == SLAVE_COMM)
						BLMGR_RxBattLevel = BLMGR_DataRxBuffer[BATT_LEVEL_IDX];
						#else
						/*Wrong Config, State in Idle*/
						/*To do: Managing dev Errors*/
						#endif
						/*Update error state*/
						BLMGR_ErrorState = ERRH_NO_ERROR;
						IsFrameValid5 = 1U;

					}
					else
					{
						/*Invalid Frame receiver*/
						BLMGR_ErrorState = ERRH_INVALID_RECEIVER_ERROR;
						IsFrameValid5 = 0U;
					}

				}

			}
			else
			{
				/*Invalid Frame Type*/
				BLMGR_ErrorState = ERRH_WRONG_FRAME_ERROR;
				IsFrameValid5 = 0U;
			}
		}
		else
		{
			/*Invalid Frame Detected*/
			BLMGR_ErrorState = ERRH_INVALID_FRAME_ERROR;
			IsFrameValid5 = 0U;
		}
	}
	else
	{
		/*Checksum error*/
		BLMGR_ErrorState = ERRH_CHECKSUM_ERROR;
		IsFrameValid5 = 0U;
	}
	return IsFrameValid5;
}
/*********************************************************************************/
static void ErrorHandlingStateMachine(void)
{
	switch(BLMGR_ErrorState)
	{
		case ERRH_TIMEOUT_ERROR:
		{
			/*Check the Expected frame to be received*/
			switch(BLMGR_ExpectedReceivedFrame)
			{
				case FRAME_TYPE_ID_FRAME:
				{
					if(BLMGR_HandshakeFailRepCount <= MAX_HANDSHAKE_FAIL_REP)
					{
						BLMGR_HandshakeFailRepCount ++;
						BLMGR_HandShakeState = HANDSHAKE_STATE_RECV_ID_FRAME;
						/*Send Error frame*/
						UpdateErrorFrame(ERROR_TYPE_RESEND_LAST_FRMAE);
						BLTD_StartReceivingData(BLMGR_DataRxBuffer,ID_FRAME_LENGTH,&RxBufferDnCallBackNotif);
						BLTD_SendMessage(BLMGR_DataTxBuffer,ERROR_FRAME_LENGTH);
					}
					else
					{
						BLMGR_HandshakeFailRepCount = 0U;
						/*Handshaking failed*/
						BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;
					}

	                break;
				}
				case FRAME_TYPE_VAL_FRAME:
				{
					if(BLMGR_HandshakeFailRepCount <= MAX_HANDSHAKE_FAIL_REP)
					{
						BLMGR_HandshakeFailRepCount ++;
						BLMGR_HandShakeState = HANDSHAKE_STATE_RECV_VAL_FRAME;
						/*Send Error frame*/
						UpdateErrorFrame(ERROR_TYPE_RESEND_LAST_FRMAE);
						BLTD_StartReceivingData(BLMGR_DataRxBuffer,VAL_FRAME_LENGTH,&RxBufferDnCallBackNotif);
						BLTD_SendMessage(BLMGR_DataTxBuffer,ERROR_FRAME_LENGTH);
					}
					else
					{
						BLMGR_HandshakeFailRepCount = 0U;
						/*Handshaking failed*/
						BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;
					}

	                break;
				}
				case FRAME_TYPE_DATA_FRAME:
				{
					
					
					if(BLMGR_CommFailReptCount <= MAX_COMM_FAIL_REP)
					{
						/*InserBreakPoint();*/
						BLMGR_CommFailReptCount ++;
						BLMGR_CommState = COMM_STATE_SEND_DATA_FRAME;
						/*Send Error frame*/
						/*UpdateErrorFrame(ERROR_TYPE_RESEND_LAST_FRMAE);*/
						/*BLTD_StartReceivingData(BLMGR_DataRxBuffer,DATA_FRAME_LENGTH,RxBufferDnCallBackNotif);*/
						/*BLTD_SendMessage(BLMGR_DataTxBuffer,ERROR_FRAME_LENGTH);*/
					}
					else
					{
						/*InserBreakPoint();*/
						BuzzerSound();
						BLMGR_CommFailReptCount = 0U;
						/*Handshaking failed*/
						BLMGR_CommState = COMM_STATE_FAILED;
					}

	                break;
				}
				default:
				    break;
			}

	        break;
		}
		case ERRH_HANDSHAKE_ERROR:
		{
			if(BLMGR_HandshakeFailRepCount <= MAX_HANDSHAKE_FAIL_REP)
			{
				/*Start new handshaking*/
				BLMGR_HandshakeFailRepCount ++;
				BLMGR_HandShakeState = HANDSHAKE_STATE_IDLE;
				/*Send Error frame*/
				UpdateErrorFrame(ERROR_TYPE_START_NEW_HANDSHAKE);
				BLTD_StartReceivingData(BLMGR_DataRxBuffer,DATA_FRAME_LENGTH,&RxBufferDnCallBackNotif);
				BLTD_SendMessage(BLMGR_DataTxBuffer,ERROR_FRAME_LENGTH);
			}
			else
			{
				BLMGR_HandshakeFailRepCount = 0U;
				/*Handshaking failed*/
				BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;
			}

	        break;
		}
		case ERRH_CHECKSUM_ERROR:
		{
			/*Check the Expected frame to be received*/
			switch(BLMGR_ExpectedReceivedFrame)
			{
				case FRAME_TYPE_ID_FRAME:
				{
					if(BLMGR_HandshakeFailRepCount <= MAX_HANDSHAKE_FAIL_REP)
					{
						BLMGR_HandshakeFailRepCount ++;
						BLMGR_HandShakeState = HANDSHAKE_STATE_RECV_ID_FRAME;
						/*Send Error frame*/
						UpdateErrorFrame(ERROR_TYPE_RESEND_LAST_FRMAE);
						BLTD_StartReceivingData(BLMGR_DataRxBuffer,ID_FRAME_LENGTH,&RxBufferDnCallBackNotif);
						BLTD_SendMessage(BLMGR_DataTxBuffer,ERROR_FRAME_LENGTH);
					}
					else
					{
						BLMGR_HandshakeFailRepCount = 0U;
						/*Handshaking failed*/
						BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;
					}

	                break;
				}
				case FRAME_TYPE_VAL_FRAME:
				{
					if(BLMGR_HandshakeFailRepCount <= MAX_HANDSHAKE_FAIL_REP)
					{
						BLMGR_HandshakeFailRepCount ++;
						BLMGR_HandShakeState = HANDSHAKE_STATE_RECV_VAL_FRAME;
						/*Send Error frame*/
						UpdateErrorFrame(ERROR_TYPE_RESEND_LAST_FRMAE);
						BLTD_StartReceivingData(BLMGR_DataRxBuffer,VAL_FRAME_LENGTH,&RxBufferDnCallBackNotif);
						BLTD_SendMessage(BLMGR_DataTxBuffer,ERROR_FRAME_LENGTH);
					}
					else
					{
						BLMGR_HandshakeFailRepCount = 0U;
						/*Handshaking failed*/
						BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;
					}

	                break;
				}
				case FRAME_TYPE_DATA_FRAME:
				{
					if(BLMGR_CommFailReptCount <= MAX_COMM_FAIL_REP)
					{
						BLMGR_CommFailReptCount ++;
						BLMGR_CommState = COMM_STATE_RECV_DATA_FRAME;
						/*Send Error frame*/
						UpdateErrorFrame(ERROR_TYPE_RESEND_LAST_FRMAE);
						BLTD_StartReceivingData(BLMGR_DataRxBuffer,DATA_FRAME_LENGTH,&RxBufferDnCallBackNotif);
						BLTD_SendMessage(BLMGR_DataTxBuffer,ERROR_FRAME_LENGTH);
					}
					else
					{
						BLMGR_CommFailReptCount = 0U;
						/*Handshaking failed*/
						BLMGR_CommState = COMM_STATE_FAILED;
					}

	                break;
				}
				default:
				    break;
			}

	        break;
		}
		case ERRH_INVALID_FRAME_ERROR:
		{
			switch(BLMGR_ExpectedReceivedFrame)
			{
				case FRAME_TYPE_ID_FRAME:
				{
					BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;

	                break;
				}
				case FRAME_TYPE_VAL_FRAME:
				{
					BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;

	                break;
				}
				case FRAME_TYPE_DATA_FRAME:
				{
					BLMGR_CommState = COMM_STATE_FAILED;

	                break;
				}
				default:
				    break;
			}

	        break;
		}
		case ERRH_CRC_ERROR:
		{
			switch(BLMGR_ExpectedReceivedFrame)
			{
				case FRAME_TYPE_ID_FRAME:
				{
					BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;

	                break;
				}
				case FRAME_TYPE_VAL_FRAME:
				{
					BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;

	                break;
				}
				case FRAME_TYPE_DATA_FRAME:
				{
					BLMGR_CommState = COMM_STATE_FAILED;

	                break;
				}
				default:
				    break;
			}

	        break;
		}
		case ERRH_INVALID_RECEIVER_ERROR:
		{
			/*Check the Expected frame to be received*/
			switch(BLMGR_ExpectedReceivedFrame)
			{
				case FRAME_TYPE_ID_FRAME:
				{
					if(BLMGR_HandshakeFailRepCount <= MAX_HANDSHAKE_FAIL_REP)
					{
						BLMGR_HandshakeFailRepCount ++;
						BLMGR_HandShakeState = HANDSHAKE_STATE_RECV_ID_FRAME;
						/*Send Error frame*/
						UpdateErrorFrame(ERROR_TYPE_UPDATE_UR_TRANSMITTER);
						BLTD_StartReceivingData(BLMGR_DataRxBuffer,ID_FRAME_LENGTH,&RxBufferDnCallBackNotif);
						BLTD_SendMessage(BLMGR_DataTxBuffer,ERROR_FRAME_LENGTH);
					}
					else
					{
						BLMGR_HandshakeFailRepCount = 0U;
						/*Handshaking failed*/
						BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;
					}

	                break;
				}
				case FRAME_TYPE_VAL_FRAME:
				{
					if(BLMGR_HandshakeFailRepCount <= MAX_HANDSHAKE_FAIL_REP)
					{
						BLMGR_HandshakeFailRepCount ++;
						BLMGR_HandShakeState = HANDSHAKE_STATE_RECV_VAL_FRAME;
						/*Send Error frame*/
						UpdateErrorFrame(ERROR_TYPE_UPDATE_UR_TRANSMITTER);
						BLTD_StartReceivingData(BLMGR_DataRxBuffer,VAL_FRAME_LENGTH,&RxBufferDnCallBackNotif);
						BLTD_SendMessage(BLMGR_DataTxBuffer,ERROR_FRAME_LENGTH);
					}
					else
					{
						BLMGR_HandshakeFailRepCount = 0U;
						/*Handshaking failed*/
						BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;
					}

	                break;
				}
				case FRAME_TYPE_DATA_FRAME:
				{
					if(BLMGR_CommFailReptCount <= MAX_COMM_FAIL_REP)
					{
						BLMGR_CommFailReptCount ++;
						BLMGR_CommState = COMM_STATE_RECV_DATA_FRAME;
						/*Send Error frame*/
						UpdateErrorFrame(ERROR_TYPE_UPDATE_UR_TRANSMITTER);
						BLTD_StartReceivingData(BLMGR_DataRxBuffer,DATA_FRAME_LENGTH,&RxBufferDnCallBackNotif);
						BLTD_SendMessage(BLMGR_DataTxBuffer,ERROR_FRAME_LENGTH);
					}
					else
					{
						BLMGR_CommFailReptCount = 0U;
						/*Handshaking failed*/
						BLMGR_CommState = COMM_STATE_FAILED;
					}

	                break;
				}
				default:
				    break;
			}

	        break;
		}
		case ERRH_WRONG_FRAME_ERROR:
		{
			if(BLMGR_DataRxBuffer[FRAME_TYPE_IDX] == FRAME_TYPE_ERROR_FRAME)
			{
				/*Handle Error frame*/
				CheckErrorFrame();
			}
			else
			{
				switch(BLMGR_ExpectedReceivedFrame)
				{
					case FRAME_TYPE_ID_FRAME:
					{
						BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;

	                    break;
					}
					case FRAME_TYPE_VAL_FRAME:
					{
						BLMGR_HandShakeState = HANDSHAKE_STATE_FAILED;

	                    break;
					}
					case FRAME_TYPE_DATA_FRAME:
					{
						BLMGR_CommState = COMM_STATE_FAILED;

					break;
					}
					default:
					    break;
				}
			}

		break;
		}
		default:
		    break;
	}
}
/*********************************************************************************/
static void UpdateErrorFrame(u8 ErrorType)
{
	/*Set Tx Frame to default values*/
	MemSet(BLMGR_DataTxBuffer,TX_FRAME_DEFUALT,(u16)MAX_DATA_BUFFER_LENGTH,FRAME_HEADER_IDX);
	/*Set Header of Frame*/
	MemSet(BLMGR_DataTxBuffer,0xaaU,2U,FRAME_HEADER_IDX);
	/*Set Device Sender*/
	BLMGR_DataTxBuffer[FRAME_SENDER_IDX] = DEVICE_ROLE;
	/*Set Device Receiver*/
	BLMGR_DataTxBuffer[FRAME_RECVER_IDX] = BLMGR_TxFrameReceiver;
	/*Set frame type*/
	BLMGR_DataTxBuffer[FRAME_TYPE_IDX] = FRAME_TYPE_ERROR_FRAME;
	/*Set paramter length*/
	BLMGR_DataTxBuffer[PARAM_LENGTH_IDX] = 1U;
	/*Set Error type*/
	BLMGR_DataTxBuffer[ERROR_TYPE_IDX] = ErrorType;
	/*Update Crc*/
	BLMGR_DataTxBuffer[FRAME_CRC_IDX] = TX_CRC_DEFAULT;
	BLMGR_DataTxBuffer[FRAME_CRC_IDX + 1U] = TX_CRC_DEFAULT;
	/*update Frame CheckSum*/
	BLMGR_DataTxBuffer[FRAME_CHECKSUM_IDX] = CalculateCheksum(BLMGR_DataTxBuffer,FRAME_CHECKSUM_IDX - 1U);
	/*update frame tail*/
	MemSet(BLMGR_DataTxBuffer,0x55U,1U,FRAME_TAIL_IDX);
}
/*********************************************************************************/
static void CheckErrorFrame(void)
{
	u8 ErrorType2;
	ErrorType2 = BLMGR_DataRxBuffer[ERROR_TYPE_IDX];
	switch(ErrorType2)
	{
		case ERROR_TYPE_RESEND_LAST_FRMAE:
		{
			switch(BLMGR_ExpectedReceivedFrame)
			{
				case FRAME_TYPE_ID_FRAME:
				{
					BLMGR_HandShakeState = HANDSHAKE_STATE_SEND_ID_FRAME;

	                break;
				}
				case FRAME_TYPE_VAL_FRAME:
				{
					BLMGR_HandShakeState = HANDSHAKE_STATE_SEND_VAL_FRMAE;

	                break;
				}
				case FRAME_TYPE_DATA_FRAME:
				{
					BLMGR_CommState = COMM_STATE_SEND_DATA_FRAME;

	                break;
				}
				default:
				    break;
			}

	        break;
		}
		case ERROR_TYPE_START_NEW_HANDSHAKE:
		{
			BLMGR_HandShakeState = HANDSHAKE_STATE_IDLE;

	        break;
		}
		case ERROR_TYPE_UPDATE_UR_TRANSMITTER:
		{
			switch(BLMGR_ExpectedReceivedFrame)
			{
				case FRAME_TYPE_ID_FRAME:
				{
					BLMGR_HandShakeState = HANDSHAKE_STATE_SEND_ID_FRAME;

	                break;
				}
				case FRAME_TYPE_VAL_FRAME:
				{
					BLMGR_HandShakeState = HANDSHAKE_STATE_SEND_VAL_FRMAE;

	                break;
				}
				case FRAME_TYPE_DATA_FRAME:
				{
					BLMGR_CommState = COMM_STATE_SEND_DATA_FRAME;

	                break;
				}
				default:
				    break;
			}

	        break;
		}
		default:
		    break;
	}
}
/*********************************************************************************/
static void MemCpy( u8 DesPtr[], const u8 SrcPtr[], u16 Length,u8 StartIndex1,u8 StartIndex2)
{
	u16 LoopIndex2First=StartIndex1,LoopIndex2Second=StartIndex2,i;
	for(i=0U; i < Length; i++)
	{

	    DesPtr[LoopIndex2First+i] =  SrcPtr[LoopIndex2Second+i];

	}
}
/*********************************************************************************/
static void MemSet(u8 DesPtr[], u8 ConstVal, u16 Length,u8 StartIndex3)
{
	u16 LoopIndex3;
	for(LoopIndex3 = 0U; LoopIndex3 < Length; LoopIndex3 ++)
	{
		DesPtr[LoopIndex3+StartIndex3]=ConstVal;
	}
}
/*********************************************************************************/
#if (COMM_CINFIG == SLAVE_COMM)
static u8 MemCmp(const u8* Src1Ptr,const u8* Src2Ptr,u16 Length)
{
	u8 IsEqual = 1;
	u8 LoopIndex4;
	for (LoopIndex4 = 0U; (LoopIndex4 < Length) && (IsEqual == 1) ; LoopIndex4 ++)
	{
		if(*(Src1Ptr + LoopIndex4) != *(Src2Ptr + LoopIndex4))
		{
			IsEqual = 0U;
		}
	}
	return IsEqual;
}
#endif
/*********************************************************************************/
static u8 CalculateCheksum(const u8 BufferPtr[], u16 BufferLength)
{
	u32 Checksum = 0x00U;
	u16 LoopIndex5;
	for (LoopIndex5 = 0U; LoopIndex5 <= BufferLength; LoopIndex5 ++)
	{
		Checksum += BufferPtr[LoopIndex5];
	}
	Checksum = Checksum % 256U;
	return (u8)Checksum;
}
/*********************************************************************************/
static u8 CheckCrc(void)
{
	u16 RxCrc;
	u16 GenCrc2=0U;
	u8 TempBuffer4[MAX_DATA_BUFFER_LENGTH];
	u8 IsFrameValid6;
	RxCrc = 0x00U;
	RxCrc = BLMGR_DataRxBuffer[FRAME_VAL_CRC_IDX];
	RxCrc |= ((u16)BLMGR_DataRxBuffer[FRAME_VAL_CRC_IDX +1]) << 8;
	TempBuffer4[0x00] = TX_OS_TYPE;
	TempBuffer4[0x01] = TX_DEV_TYPE;
	MemCpy(TempBuffer4,BLMGR_TxDevicName,BLMGR_TxDeviceNameLength,0x02U,0U);
	SECR_GnerateCrc(TempBuffer4,BLMGR_TxDeviceNameLength + 2, &GenCrc2,BLMGR_CrcKey);
	if(GenCrc2 == RxCrc)
	{
		BLMGR_ErrorState = ERRH_NO_ERROR;
		IsFrameValid6 = 1U;
	}
	else
	{
		/*Crc Error Found*/
		BLMGR_ErrorState = ERRH_CRC_ERROR;
		IsFrameValid6 = 0U;
	}
	return IsFrameValid6;
}
/*********************************************************************************/
#if (COMM_CINFIG == SLAVE_COMM)
static u8 GetCrcKey(void)
{
	u8 IsFrameValid7;
	u16 RxCrc;
	u16 GenCrc3;
	static u8 TempBuffer5[MAX_DATA_BUFFER_LENGTH];
	u8 LoopTerminated;
	u32 LoopIndex6;
	RxCrc = 0x00;
	RxCrc = BLMGR_DataRxBuffer[FRAME_VAL_CRC_IDX];
	RxCrc |= ((u16)BLMGR_DataRxBuffer[FRAME_VAL_CRC_IDX +1]) << 8;
	TempBuffer5[0x00] = TX_OS_TYPE;
	TempBuffer5[0x01] = TX_DEV_TYPE;
	MemCpy(&TempBuffer5[0x02],BLMGR_TxDevicName,BLMGR_TxDeviceNameLength);
	LoopTerminated = 0;
	for (LoopIndex6 = 0U; (LoopIndex6 < 0xffff) && (LoopTerminated == 0U); LoopIndex6 ++)
	{
		SECR_GnerateCrc(TempBuffer5,BLMGR_TxDeviceNameLength + 2, &GenCrc3,(LoopIndex6 | 0x10000));
		if(GenCrc3 == RxCrc)
		{
			BLMGR_CrcKey = LoopIndex6;
			LoopTerminated = 1;
		}
	}
	if(LoopTerminated == 1)
	{
		/*Done and CRC Key Found*/
		BLMGR_ErrorState = ERRH_NO_ERROR;
		IsFrameValid7 = 1;
	}
	else
	{
		/*Crc Error Found*/
		BLMGR_ErrorState = ERRH_CRC_ERROR;
		IsFrameValid7 = 0U;
	}
	return IsFrameValid7;
}
#endif
/*********************************************************************************/
static void PowerBluetoothOn(void)
{
	DIO_WritePort(BlueToothPwrConfig.Portname,BLOUETOOTH_ON,BlueToothPwrConfig.Portmask);
}
/*********************************************************************************/
static void PowerBluetoothOff(void)
{
	DIO_WritePort(BlueToothPwrConfig.Portname,!BLOUETOOTH_ON,BlueToothPwrConfig.Portmask);
}
/*********************************************************************************/
static void DisconnectStateMachine(void)
{
}
/*********************************************************************************/
static void DisconnectInit(void)
{
	BLMGR_DisconectionTimeOut = 0U;
}
/*********************************************************************************/
static void BuzzerInit(void)
{
	DIO_InitPortDirection(BuzzerConfig.Portname,0xffU,BuzzerConfig.Portmask);
	DIO_WritePort(BuzzerConfig.Portname,0x00U,BuzzerConfig.Portmask);
}
/*********************************************************************************/
static void PowerBlueToothInit(void)
{
	DIO_InitPortDirection(BlueToothPwrConfig.Portname,0xffU,BlueToothPwrConfig.Portmask);
	DIO_WritePort(BlueToothPwrConfig.Portname,0x00U,BlueToothPwrConfig.Portmask);
}
/*********************************************************************************/
static void BlueToothKeyInit(void)
{
	DIO_InitPortDirection(BluetoothKeyConfig.Portname,0xffU,BluetoothKeyConfig.Portmask);
	DIO_WritePort(BluetoothKeyConfig.Portname,0xffU,BluetoothKeyConfig.Portmask);
}

static void InserBreakPoint(void)
{
	DIO_WritePort(BuzzerConfig.Portname,0xffU,BuzzerConfig.Portmask);
	while(1){

	}
}
