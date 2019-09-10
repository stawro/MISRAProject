/*
 * L3G4200D.c
 *
 * Created: 24/09/2015 09:13:31 م
 *  Author: hossam
 */ 

#include "L3G4200D.h"
#include "L3G4200D_Cfg.h"
#include "L3G4200D_Lcfg.h"
#include "SPI.h"
/*****************************************************************************************************/
                                         /* Local Symbols */
/*****************************************************************************************************/
/*Registers addresses*/
#define WHO_AM_I_REG_ADD 0x0fU
#define CTRL_REG1_ADD 0x20U
#define CTRL_REG2_ADD 0x21U
#define CTRL_REG3_ADD 0x22U
#define CTRL_REG4_ADD 0x23U
#define CTRL_REG5_ADD 0x24U
#define REFERENCE_ADD 0x25U
#define OUT_TEMP_ADD 0x26U
#define STATUS_REG_ADD 0x27U
#define OUT_X_L_ADD 0x28U
#define OUT_X_H_ADD 0x29U
#define OUT_Y_L_ADD 0x2aU
#define OUT_Y_H_ADD 0x2bU
#define OUT_Z_L_ADD 0x2cU
#define OUT_Z_H_ADD 0x2dU
#define FIFO_CTRL_REG_ADD 0x2eU
#define FIFO_SRC_REG_ADD 0x2fU
#define INT1_CFG_ADD 0x30U
#define INT1_SRC_ADD 0x31U
#define INT1_TSH_XH_ADD 0x32U
#define INT1_TSH_XL_ADD 0x33U
#define INT1_TSH_YH_ADD 0x34U
#define INT1_TSH_YL_ADD 0x35U
#define INT1_TSH_ZH_ADD 0x36U
#define INT1_TSH_ZL_ADD 0x37U
#define INT1_DURATION_ADD 0x38U

u8 data1 = 1u;

/*****************************************************************************************************/
/*General Write Read Macros*/
/*
#define L3G4200D_REG_WRITE(ADD,DATA)
 DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
SPI_u8TransfereByte(ADD);
SPI_u8TransfereByte(DATA);
GYHD_DEACTIVATE_SLAVE_SELECT();
									 
#define L3G4200D_REG_READ(ADD,DATA_PTR)  DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);\
                                     SPI_u8TransfereByte(ADD|0x80);\
									 (*DATA_PTR) = SPI_u8TransfereByte(0x00);\
									 GYHD_DEACTIVATE_SLAVE_SELECT();*/
/*****************************************************************************************************/
/* MAcros To Axis Registers*/									 
/*#define L3G4200D_READ_WHO_AM_I(DATA_PTR)           L3G4200D_REG_READ(WHO_AM_I_REG_ADD,DATA_PTR)*/
/*#define L3G4200D_READ_CTRL_REG1(DATA_PTR)          L3G4200D_REG_READ(CTRL_REG1_ADD,DATA_PTR)*/
/*#define L3G4200D_WRITE_CTRL_REG1(DATA)             L3G4200D_REG_WRITE(CTRL_REG1_ADD,DATA)
#define L3G4200D_READ_CTRL_REG2(DATA_PTR)          L3G4200D_REG_READ(CTRL_REG2_ADD,DATA_PTR)
#define L3G4200D_WRITE_CTRL_REG2(DATA)             L3G4200D_REG_WRITE(CTRL_REG2_ADD,DATA)
#define L3G4200D_READ_CTRL_REG3(DATA_PTR)          L3G4200D_REG_READ(CTRL_REG3_ADD,DATA_PTR)
#define L3G4200D_WRITE_CTRL_REG3(DATA)             L3G4200D_REG_WRITE(CTRL_REG3_ADD,DATA)
#define L3G4200D_READ_CTRL_REG4(DATA_PTR)          L3G4200D_REG_READ(CTRL_REG4_ADD,DATA_PTR)
#define L3G4200D_WRITE_CTRL_REG4(DATA)             L3G4200D_REG_WRITE(CTRL_REG4_ADD,DATA)
#define L3G4200D_READ_CTRL_REG5(DATA_PTR)          L3G4200D_REG_READ(CTRL_REG5_ADD,DATA_PTR)
#define L3G4200D_WRITE_CTRL_REG5(DATA)             L3G4200D_REG_WRITE(CTRL_REG5_ADD,DATA)
#define L3G4200D_READ_REFERENCE(DATA_PTR)          L3G4200D_REG_READ(REFERENCE_ADD,DATA_PTR)
#define L3G4200D_WRITE_REFERENCE(DATA)             L3G4200D_REG_WRITE(REFERENCE_ADD,DATA)
#define L3G4200D_READ_OUT_TEMP(DATA_PTR)           L3G4200D_REG_READ(OUT_TEMP_ADD,DATA_PTR)	
#define L3G4200D_READ_STATUS_REG(DATA_PTR)         L3G4200D_REG_READ(STATUS_REG_ADD,DATA_PTR)	
#define L3G4200D_READ_OUT_X_L(DATA_PTR)            L3G4200D_REG_READ(OUT_X_L_ADD,DATA_PTR)	
#define L3G4200D_READ_OUT_X_H(DATA_PTR)            L3G4200D_REG_READ(OUT_X_H_ADD,DATA_PTR)	
#define L3G4200D_READ_OUT_Y_L(DATA_PTR)            L3G4200D_REG_READ(OUT_Y_L_ADD,DATA_PTR)	
#define L3G4200D_READ_OUT_Y_H(DATA_PTR)            L3G4200D_REG_READ(OUT_Y_H_ADD,DATA_PTR)	
#define L3G4200D_READ_OUT_Z_L(DATA_PTR)            L3G4200D_REG_READ(OUT_Z_L_ADD,DATA_PTR)	
#define L3G4200D_READ_OUT_Z_H(DATA_PTR)            L3G4200D_REG_READ(OUT_Z_H_ADD,DATA_PTR)	
#define L3G4200D_READ_FIFO_CTRL_REG(DATA_PTR)      L3G4200D_REG_READ(FIFO_CTRL_REG_ADD,DATA_PTR)	
#define L3G4200D_WRITE_FIFO_CTRL_REG(DATA)         L3G4200D_REG_WRITE(FIFO_CTRL_REG_ADD,DATA)
#define L3G4200D_READ_FIFO_SRC_REG(DATA_PTR)       L3G4200D_REG_READ(FIFO_SRC_REG_ADD,DATA_PTR)	
#define L3G4200D_READ_INT1_CFG(DATA_PTR)           L3G4200D_REG_READ(INT1_CFG_ADD,DATA_PTR)	
#define L3G4200D_WRITE_INT1_CFG(DATA)              L3G4200D_REG_WRITE(INT1_CFG_ADD,DATA)
#define L3G4200D_READ_INT1_SRC(DATA_PTR)           L3G4200D_REG_READ(INT1_SRC_ADD,DATA_PTR)	
#define L3G4200D_READ_INT1_TSH_XH(DATA_PTR)        L3G4200D_REG_READ(INT1_TSH_XH_ADD,DATA_PTR)	
#define L3G4200D_WRITE_INT1_TSH_XH(DATA)           L3G4200D_REG_WRITE(INT1_TSH_XH_ADD,DATA)
#define L3G4200D_READ_INT1_TSH_XL(DATA_PTR)        L3G4200D_REG_READ(INT1_TSH_XL_ADD,DATA_PTR)	
#define L3G4200D_WRITE_INT1_TSH_XL(DATA)           L3G4200D_REG_WRITE(INT1_TSH_XL_ADD,DATA)
#define L3G4200D_READ_INT1_TSH_YH(DATA_PTR)        L3G4200D_REG_READ(INT1_TSH_YH_ADD,DATA_PTR)	
#define L3G4200D_WRITE_INT1_TSH_YH(DATA)           L3G4200D_REG_WRITE(INT1_TSH_YH_ADD,DATA)
#define L3G4200D_READ_INT1_TSH_YL(DATA_PTR)        L3G4200D_REG_READ(INT1_TSH_YL_ADD,DATA_PTR)	
#define L3G4200D_WRITE_INT1_TSH_YL(DATA)           L3G4200D_REG_WRITE(INT1_TSH_YL_ADD,DATA)
#define L3G4200D_READ_INT1_TSH_ZH(DATA_PTR)        L3G4200D_REG_READ(INT1_TSH_ZH_ADD,DATA_PTR)	
#define L3G4200D_WRITE_INT1_TSH_ZH(DATA)           L3G4200D_REG_WRITE(INT1_TSH_ZH_ADD,DATA)
#define L3G4200D_READ_INT1_TSH_ZL(DATA_PTR)        L3G4200D_REG_READ(INT1_TSH_ZL_ADD,DATA_PTR)	
#define L3G4200D_WRITE_INT1_TSH_ZL(DATA)           L3G4200D_REG_WRITE(INT1_TSH_ZL_ADD,DATA)
#define L3G4200D_READ_INT1_DURATION(DATA_PTR)      L3G4200D_REG_READ(INT1_DURATION_ADD,DATA_PTR)	
#define L3G4200D_WRITE_INT1_DURATION(DATA)         L3G4200D_REG_WRITE(INT1_DURATION_ADD,DATA)*/
/*****************************************************************************************************/
/*Register Default Value*/
#define L3G4200D_WHO_AM_I_DEFAULT 0xD3U
/*****************************************************************************************************/
/*Operational Modes Management*/
#define u8PWR_DN_MOD 0x00U
#define u8SLEEP_MOD  0x08U
#define u8NORMAL_MOD 0x0fU
#define u8MOD_CLR_MASK 0xf0U
#define u8TO_SLEEP_MODE_DELAY_MS 0x05U
/*****************************************************************************************************/
/*CTRL_REG4 Initialization*/
#define u8BLOCK_DATA_UPDATE 0x00U
#define	u8BIG_LITTLE_ENDIAN 0x00U
#define u8SELF_TEST_MODE    0x00U
#define u8SPI_MODE          0x00U
#define u8CTRL_REG4_INIT (u8BLOCK_DATA_UPDATE | u8BIG_LITTLE_ENDIAN | u8SELF_TEST_MODE | u8SPI_MODE)
/*****************************************************************************************************/
/*Reading Operation*/
#define u8NEW_DATA_AVAILABEL_X_MASK 0x01U
#define u8NEW_DATA_AVAILABEL_Y_MASK 0x02U
#define u8NEW_DATA_AVAILABEL_Z_MASK 0x04U
#define u8DATA_OVER_WRITTEN_MASK    0x80U
#define u8AXIS_IS_ENABLED           0x00U
#define u8AXIS_IS_DISABLED          0x01U
/*****************************************************************************************************/
/*Filters Configuration*/
#define u8FILTERS_CLR_MASK 0xE0U
/*****************************************************************************************************/
/*Self Axis Movement Config*/
#if (u8SELF_AXIS_MOV == ON)
#define u8INT1_ENABLE 0x80U
#endif
/*****************************************************************************************************/
                                        /*Static Variables*/
/*****************************************************************************************************/
static u8 GYHD_u8LastPowerMode = u8PWR_DN_MOD;		
static u8 GYHD_u8TimeOutFlag;
static u8 GYHD_u8StartTimeoutFlag;	
/*static u8 GYHD_u8FullScale;	*/
/*****************************************************************************************************/
                                 /*Private functions declarations*/
/*****************************************************************************************************/
static u8 u8CheckTimeOut(u8 u8TimeInMs);
static void vidManageOperationModes(u8 u8NewMode);
static u8 udtSelfTest(void);
static void vidInitCtrlReg4(void);
static void vidCheckForNewData(u8* pu8Status, u8 u8AxisMask);
static u16 u16ReadAxisData(u8 AxisMask);
static u8 u8CheckAxisIsEnabled(u8 u8AxisMask);
static void vidInitFilters(void);
#if (u8SELF_AXIS_MOV == ON)
static void vidInitSelectiveAxisMovement(void);
#endif
/*****************************************************************************************************/
                                      /* Global Services */
/*****************************************************************************************************/
u8 GYHD_Init(void)
{
	u8 RetVal;
	/*GYHD_INIT_SLAVE_SELECT();*/
	DIO_InitPortDirection(PB,1U<<4U,1U<<4U);
    DIO_WritePort(PB,1U<<4U,1U<<4U);

	SPI_vidInit();
	GYHD_u8TimeOutFlag = 0U;
	GYHD_u8StartTimeoutFlag = 0U;
	/*To Do: Initialized Module Registers with the Specific values*/
	/*Wait for Entering Power Down Mode*/
	do 
	{
	 /* u8START_TIME_OUT_MS(250,&GYHD_u8TimeOutFlag);	*/
	  _delay_ms(250U);
	  *(&GYHD_u8TimeOutFlag) = 0x01U;

	}while (GYHD_u8TimeOutFlag == 0U);
	GYHD_u8TimeOutFlag = 0U;
	
		/*Perform Sensor Self Test*/
	RetVal = udtSelfTest();
	if(RetVal == (u8)OK)
	{
		/*Init Power Mode*/
	vidManageOperationModes(u8PWR_DN_MOD);
	GYHD_u8LastPowerMode = u8PWR_DN_MOD;
	
	/*Init FullScale, Data Order, Spi Mode and block data update*/
	vidInitCtrlReg4();
	
	/*Init Filters*/
	vidInitFilters();
	}
	else
	{
		/* Self Test Failed, a Problem in Communication Found*/
	}
	return RetVal;
}
/*****************************************************************************************************/
void GYHD_WakeUpModule(u8* u8WakeUpStatePtr)
{
	vidManageOperationModes(u8NORMAL_MOD);
	if((GYHD_u8LastPowerMode == u8NORMAL_MOD) && (GYHD_u8StartTimeoutFlag == 0U))
	{
		*u8WakeUpStatePtr = u8WAKEUP_DN;
	}
	else
	{
		*u8WakeUpStatePtr = u8WAKEUP_IN_PROGRESS;
	}
	
}
/*****************************************************************************************************/
void GYHD_SleepModule(void)
{
   vidManageOperationModes(u8SLEEP_MOD);	
}
/*****************************************************************************************************/
u8 GYHD_ReadXDirection(u16* pu16Data,u8* pu8ReadingStatus)
{
	u8 RetVar = (u8)NOK;
	u8 u8IsEnabled;
	u8 u8AxisMask2 = u8NEW_DATA_AVAILABEL_X_MASK;
	/*Check if Axis was enabled*/
	u8IsEnabled = u8CheckAxisIsEnabled(u8AxisMask2);
	if(u8IsEnabled == u8AXIS_IS_ENABLED)
	{
		/*Check for Data Updated*/
		vidCheckForNewData(pu8ReadingStatus,u8AxisMask2);
		if(*pu8ReadingStatus == u8READING_OK)
		{
			/*Read Data*/
			*pu16Data = u16ReadAxisData(u8AxisMask2);
			RetVar = (u8)OK;
		}
		else
		{
			/*Data overwritten and Pointer Initialized*/
		}
	}
	else
	{            
		/*Axis Disabled*/
		*pu8ReadingStatus = u8AXIS_DISABLED;
	}
	
	return RetVar;
}
/*****************************************************************************************************/
u8 GYHD_ReadYDirection(u16* pu16Data,u8* pu8ReadingStatus)
{
	u8 RetVar3 = (u8)NOK;
	u8 u8IsEnabled2=0U;
	u8 u8AxisMask3 = u8NEW_DATA_AVAILABEL_Y_MASK;
	/*Check if Axis was enabled*/
	u8IsEnabled2 = u8CheckAxisIsEnabled(u8AxisMask3);
	if(u8IsEnabled2 == u8AXIS_IS_ENABLED)
	{
		/*Check for Data Updated*/
		vidCheckForNewData(pu8ReadingStatus,u8AxisMask3);
		if(*pu8ReadingStatus == u8READING_OK)
		{
			/*Read Data*/
			*pu16Data = u16ReadAxisData(u8AxisMask3);
			RetVar3 = (u8)OK;
		}
		else
		{
			/*Data overwritten and Pointer Initialized*/
		}
	}
	else
	{            
		/*Axis Disabled*/
		*pu8ReadingStatus = u8AXIS_DISABLED;
	}
	
	return RetVar3;
}
/*****************************************************************************************************/
u8 GYHD_ReadZDirection(u16* pu16Data,u8* pu8ReadingStatus)
{
	u8 RetVar4 = (u8)NOK;
	u8 u8IsEnabled4=0U;
	u8 u8AxisMask5 = u8NEW_DATA_AVAILABEL_Z_MASK;
	/*Check if Axis was enabled*/
	u8IsEnabled4 = u8CheckAxisIsEnabled(u8AxisMask5);
	if(u8IsEnabled4 == u8AXIS_IS_ENABLED)
	{
		/*Check for Data Updated*/
		vidCheckForNewData(pu8ReadingStatus,u8AxisMask5);
		if(*pu8ReadingStatus == u8READING_OK)
		{
			/*Read Data*/
			*pu16Data = u16ReadAxisData(u8AxisMask5);
			RetVar4 = (u8)OK;
		}
		else
		{
			/*Data overwritten and Pointer Initialized*/
		}
	}
	else
	{            
		/*Axis Disabled*/
		*pu8ReadingStatus = u8AXIS_DISABLED;
	}
	
	return RetVar4;
}
/*****************************************************************************************************/
                              /*Private functions definitions*/
/*****************************************************************************************************/
static void vidCheckForNewData(u8* pu8Status, u8 u8AxisMask)
{
	u8 u8Status;
	/*L3G4200D_READ_STATUS_REG(&u8Status);*/
	/*L3G4200D_REG_READ(STATUS_REG_ADD,DATA_PTR);*/

     DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(STATUS_REG_ADD|0x80U);
    u8Status = SPI_u8TransfereByte(0x00U);

    DIO_WritePort(PB,(1U<<4U),(1U<<4U));
	/*Check for Data updated*/
	if((u8Status & u8AxisMask) == u8AxisMask)
	{
		/*Check for Data Overwritten*/
		if ((u8Status & u8DATA_OVER_WRITTEN_MASK) == u8DATA_OVER_WRITTEN_MASK)
		{
			*pu8Status = u8DATA_OVERWRITTEN;
		}
		else
		{
			*pu8Status = u8READING_OK;
		}
	}
	else
	{
		*pu8Status = u8NO_DATA_UPDATED;
	}
	
}
/*****************************************************************************************************/
static u8 u8CheckAxisIsEnabled(u8 u8AxisMask)
{
	u8 u8IsEnabled6;
	u8 u8RegValue;
	/*Read Control Register*/
	/*L3G4200D_READ_CTRL_REG1(&u8RegValue);*/
	/*L3G4200D_REG_READ(CTRL_REG1_ADD,&u8RegValue);*/

	 DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(CTRL_REG1_ADD|0x80U);
    u8RegValue = SPI_u8TransfereByte(0x00U);
   /* GYHD_DEACTIVATE_SLAVE_SELECT();*/
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));
	/*Check for access Status*/
	if((u8RegValue & u8AxisMask) == u8AxisMask)
	{
		u8IsEnabled6 = u8AXIS_IS_ENABLED;
	}
	else
	{
		u8IsEnabled6 = u8AXIS_IS_DISABLED;
	}
	return u8IsEnabled6;
}
/*****************************************************************************************************/
static u16 u16ReadAxisData(u8 AxisMask)
{
	u16 u16AxisData;
	u8 u8RegData1;
	u8 u8RegData2;
	switch(AxisMask)
	{
		case u8NEW_DATA_AVAILABEL_X_MASK:
		{
			/*L3G4200D_READ_OUT_X_L(&u8RegData1);*/
			/*L3G4200D_REG_READ(OUT_X_L_ADD,&u8RegData1);*/

			 DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
            SPI_u8TransfereByte(OUT_X_L_ADD|0x80U);
            u8RegData1 = SPI_u8TransfereByte(0x00U);
            DIO_WritePort(PB,(1U<<4U),(1U<<4U));

			/*L3G4200D_READ_OUT_X_H(&u8RegData2);*/
			/*L3G4200D_REG_READ(OUT_X_H_ADD,DATA_PTR);*/

             DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
            SPI_u8TransfereByte(OUT_X_H_ADD|0x80U);
            u8RegData2 = SPI_u8TransfereByte(0x00U);
            DIO_WritePort(PB,(1U<<4U),(1U<<4U));

			u16AxisData = (((u16)u8RegData1) | ((u16)((u16)u8RegData2<<8U)));

	        break;
		}
		case u8NEW_DATA_AVAILABEL_Y_MASK:
		{
			/*L3G4200D_READ_OUT_Y_L(&u8RegData1);*/
			/*L3G4200D_REG_READ(OUT_Y_L_ADD,DATA_PTR);*/

             DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
            SPI_u8TransfereByte(OUT_Y_L_ADD|0x80U);
            u8RegData1 = SPI_u8TransfereByte(0x00U);
            DIO_WritePort(PB,(1U<<4U),(1U<<4U));

			/*L3G4200D_READ_OUT_Y_H(&u8RegData2);*/
			/*L3G4200D_REG_READ(OUT_Y_H_ADD,&u8RegData2);*/

             DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
            SPI_u8TransfereByte(OUT_Y_H_ADD|0x80U);
            u8RegData2 = SPI_u8TransfereByte(0x00U);
           /* GYHD_DEACTIVATE_SLAVE_SELECT();*/
            DIO_WritePort(PB,(1U<<4U),(1U<<4U));

			u16AxisData = (((u16)u8RegData1) | ((u16)((u16)u8RegData2<<8U)));

	        break;
		}
		case u8NEW_DATA_AVAILABEL_Z_MASK:
		{
			/*L3G4200D_READ_OUT_Z_L(&u8RegData1);*/
			/*L3G4200D_REG_READ(OUT_Z_L_ADD,&u8RegData1);*/

             DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
            SPI_u8TransfereByte(OUT_Z_L_ADD|0x80U);
            u8RegData1 = SPI_u8TransfereByte(0x00U);
           /* GYHD_DEACTIVATE_SLAVE_SELECT();*/
            DIO_WritePort(PB,(1U<<4U),(1U<<4U));

			/*L3G4200D_READ_OUT_Z_H(&u8RegData2);*/
			/*L3G4200D_REG_READ(OUT_Z_H_ADD,&u8RegData2);*/
			DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
            SPI_u8TransfereByte(OUT_Z_H_ADD|0x80U);
            u8RegData2 = SPI_u8TransfereByte(0x00U);
            /* GYHD_DEACTIVATE_SLAVE_SELECT();*/
            DIO_WritePort(PB,(1U<<4U),(1U<<4U));

            DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
            SPI_u8TransfereByte(OUT_Z_L_ADD|0x80U);
            u8RegData1 = SPI_u8TransfereByte(0x00U);
            /*GYHD_DEACTIVATE_SLAVE_SELECT();*/
            DIO_WritePort(PB,(1U<<4U),(1U<<4U));

			u16AxisData = (u16)(((u16)u8RegData1) | ((u16)((u16)u8RegData2<<8u)));
	        break;
		}
		default:
		    u16AxisData = 0x00U;
		break;
	}
	return u16AxisData;
}

/*****************************************************************************************************/
static void vidInitCtrlReg4(void)
{
    u8 GYHD_u8FullScale;
	u8 u8InitParams = 0x00U;
	GYHD_u8FullScale = L3G4200D_ConfigParam.u8FullScaleValue;
	u8InitParams|= ((GYHD_u8FullScale) | (u8CTRL_REG4_INIT));
	/*L3G4200D_WRITE_CTRL_REG4(u8InitParams);*/
	/*L3G4200D_REG_WRITE(CTRL_REG4_ADD,DATA);*/
	 DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
	SPI_u8TransfereByte(CTRL_REG4_ADD);
	SPI_u8TransfereByte(u8InitParams);
	/*GYHD_DEACTIVATE_SLAVE_SELECT();*/
	DIO_WritePort(PB,(1U<<4U),(1U<<4U));
}	
/*****************************************************************************************************/
static void vidManageOperationModes(u8 u8NewMode)
{
	u8 u8RegData;
	switch(u8NewMode)
	{
		case u8PWR_DN_MOD:
		{
			/*Clear the Power Mode bits in the register and write the mode values*/
		    /*L3G4200D_READ_CTRL_REG1(&u8RegData);*/
		  /*  L3G4200D_REG_READ(CTRL_REG1_ADD,&u8RegData);*/

		     DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
		    SPI_u8TransfereByte(CTRL_REG1_ADD|0x80U);
		    u8RegData = SPI_u8TransfereByte(0x00U);
		    /*GYHD_DEACTIVATE_SLAVE_SELECT();*/
		    DIO_WritePort(PB,(1U<<4U),(1U<<4U));

			u8RegData &= u8MOD_CLR_MASK;
			u8RegData |= u8NewMode;
			/*L3G4200D_WRITE_CTRL_REG1(u8RegData);*/
			/*L3G4200D_REG_WRITE(CTRL_REG1_ADD,u8RegData);*/

			 DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
			SPI_u8TransfereByte(CTRL_REG1_ADD);
			SPI_u8TransfereByte(u8RegData);
			/*GYHD_DEACTIVATE_SLAVE_SELECT();*/
			DIO_WritePort(PB,(1U<<4U),(1U<<4U));

			GYHD_u8LastPowerMode = u8PWR_DN_MOD;

	        break;
		}
		
		case u8SLEEP_MOD:
		{
			/*Clear the Power Mode bits in the register and write the mode values*/
			/*L3G4200D_READ_CTRL_REG1(&u8RegData);*/
			 /* L3G4200D_REG_READ(CTRL_REG1_ADD,&u8RegData);*/

	           DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
	            SPI_u8TransfereByte(CTRL_REG1_ADD|0x80U);
	            u8RegData = SPI_u8TransfereByte(0x00U);
	           /* GYHD_DEACTIVATE_SLAVE_SELECT();*/
	            DIO_WritePort(PB,(1U<<4U),(1U<<4U));


			u8RegData &= u8MOD_CLR_MASK;
			u8RegData |= u8NewMode;
			/*L3G4200D_WRITE_CTRL_REG1(u8RegData);*/
			/*L3G4200D_REG_WRITE(CTRL_REG1_ADD,u8RegData);*/
             DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
            SPI_u8TransfereByte(CTRL_REG1_ADD);
            SPI_u8TransfereByte(u8RegData);
           /* GYHD_DEACTIVATE_SLAVE_SELECT();*/
            DIO_WritePort(PB,(1U<<4U),(1U<<4U));

			GYHD_u8LastPowerMode = u8SLEEP_MOD;			

	        break;
		}
		
		case u8NORMAL_MOD:
		{
			/*Wait for 250ms*/
			if(GYHD_u8StartTimeoutFlag == 0U)
			{
				/*Clear axis activation bits*/
			u8NewMode &= 0xF8U;
			/*Write Activation signals*/
			u8NewMode |= ((L3G4200D_ConfigParam.strAxisActivation.u8XAxisActivation) | (L3G4200D_ConfigParam.strAxisActivation.u8YAxisActivation) | (L3G4200D_ConfigParam.strAxisActivation.u8ZAxisActivation));
			GYHD_u8TimeOutFlag = 0U;
			/*Clear the Power Mode bits in the register and write the mode values*/
		  /*  L3G4200D_READ_CTRL_REG1(&u8RegData);*/
		   /* L3G4200D_REG_READ(CTRL_REG1_ADD,&u8RegData);*/

             DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
            SPI_u8TransfereByte(CTRL_REG1_ADD|0x80U);
            u8RegData = SPI_u8TransfereByte(0x00U);
           /* GYHD_DEACTIVATE_SLAVE_SELECT();*/
            DIO_WritePort(PB,(1U<<4U),(1U<<4U));

			u8RegData &= u8MOD_CLR_MASK;
			u8RegData |= u8NewMode;
			/*L3G4200D_WRITE_CTRL_REG1(u8RegData);*/
			/*L3G4200D_REG_WRITE(CTRL_REG1_ADD,u8RegData);*/

             DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
            SPI_u8TransfereByte(CTRL_REG1_ADD);
            SPI_u8TransfereByte(u8RegData);
            /*GYHD_DEACTIVATE_SLAVE_SELECT();*/
            DIO_WritePort(PB,(1U<<4U),(1U<<4U));

			}
			else
			{
				if(GYHD_u8LastPowerMode == u8SLEEP_MOD)
				{
					GYHD_u8TimeOutFlag = u8CheckTimeOut(u8TO_SLEEP_MODE_DELAY_MS);
				}
				else
				{
					GYHD_u8TimeOutFlag = u8CheckTimeOut(250U);
				}
				
				if(GYHD_u8TimeOutFlag == 1U)
				{
					GYHD_u8StartTimeoutFlag = 0U;
					GYHD_u8LastPowerMode = u8NORMAL_MOD;
				}
				else
				{
					/* Do nothing: The timeout didn't pass*/
				}	
			}	
			break;
		}
		default:
		    break;
	}
}
/*****************************************************************************************************/
static u8 u8CheckTimeOut(u8 u8TimeInMs)
{
	u8 u8TimeOutFlag;
	/*u8START_TIME_OUT_MS(u8TO_SLEEP_MODE_DELAY_MS,&u8TimeOutFlag);*/
	_delay_ms(u8TO_SLEEP_MODE_DELAY_MS);
	*(&u8TimeOutFlag) = 0x01U;

	GYHD_u8StartTimeoutFlag = 1U;
	return u8TimeOutFlag;
}
/*****************************************************************************************************/
static u8 udtSelfTest(void)
{
	u8 RetVal6;
	u8 RegData;
	/*Read the Who AM I? Register*/
	/*L3G4200D_REG_READ(WHO_AM_I_REG_ADD,&RegData)*/

 /*   DIO_WritePort(PB,(u8)COMPLEMENT,(u8)1<<(u8)4);*/

    DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(WHO_AM_I_REG_ADD|0x80U);
    RegData = SPI_u8TransfereByte(0x00U);
    /*GYHD_DEACTIVATE_SLAVE_SELECT();*/
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));

	if(RegData == L3G4200D_WHO_AM_I_DEFAULT)
	{
		RetVal6 = (u8)OK;
	}
	else
	{
		RetVal6 = (u8)NOK;
	}
	return RetVal6;
}
/*****************************************************************************************************/
static void vidInitFilters(void)
{
	u8 u8RegData8;
	/*L3G4200D_READ_CTRL_REG5(&u8RegData);*/
	/*L3G4200D_REG_READ(CTRL_REG5_ADD,&u8RegData8);*/

	DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(CTRL_REG5_ADD|0x80U);
    u8RegData8 = SPI_u8TransfereByte(0x00U);
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));

    DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(CTRL_REG5_ADD|0x80U);
    u8RegData8 = SPI_u8TransfereByte(0x00U);
    /*GYHD_DEACTIVATE_SLAVE_SELECT();*/
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));

	u8RegData8 &= u8FILTERS_CLR_MASK;
	u8RegData8 |= (L3G4200D_ConfigParam.strFilterCfg.u8FilterOnData | L3G4200D_ConfigParam.strFilterCfg.u8FilterOnInterrupt);
	/*L3G4200D_WRITE_CTRL_REG5(u8RegData);*/
	/*L3G4200D_REG_WRITE(CTRL_REG5_ADD,u8RegData);*/
     DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(CTRL_REG5_ADD);
    SPI_u8TransfereByte(u8RegData8);
    /*GYHD_DEACTIVATE_SLAVE_SELECT();*/
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));
}
/*****************************************************************************************************/
#if(u8SELF_AXIS_MOV == ON)
static void vidInitSelectiveAxisMovement(void)
{
	u8 u8RegData3;
	u8 u8Threshold;
	/*Enable Interrupt 1*/
	/*L3G4200D_WRITE_CTRL_REG5(u8INT1_ENABLE);*/
	/*L3G4200D_REG_WRITE(CTRL_REG5_ADD,u8INT1_ENABLE);*/
     DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(CTRL_REG5_ADD);
    SPI_u8TransfereByte(u8INT1_ENABLE);
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));

	/*Write threshold values for each axis*/
	u8Threshold = (u8)L3G4200D_ConfigParam.strIntCfg.u16ThresholdX;
	/*L3G4200D_WRITE_INT1_TSH_XL(u8Threshold);*/
	/*L3G4200D_REG_WRITE(INT1_TSH_XL_ADD,u8Threshold);*/

     DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(INT1_TSH_XL_ADD);
    SPI_u8TransfereByte(u8Threshold);
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));

	u8Threshold = (u8)((u16)(L3G4200D_ConfigParam.strIntCfg.u16ThresholdX>>8U));
	/*L3G4200D_WRITE_INT1_TSH_XH(u8Threshold);*/
	/*L3G4200D_REG_WRITE(INT1_TSH_XH_ADD,u8Threshold);*/

     DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(INT1_TSH_XH_ADD);
    SPI_u8TransfereByte(u8Threshold);
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));
	
	u8Threshold = (u8)L3G4200D_ConfigParam.strIntCfg.u16ThresholdY;
	/*L3G4200D_WRITE_INT1_TSH_YL(u8Threshold);*/
	/*L3G4200D_REG_WRITE(INT1_TSH_YL_ADD,u8Threshold);*/



     DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(INT1_TSH_YL_ADD);
    SPI_u8TransfereByte(u8Threshold);
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));

	u8Threshold = (u8)((u16)(L3G4200D_ConfigParam.strIntCfg.u16ThresholdY>>8U));
	/*L3G4200D_WRITE_INT1_TSH_YH(u8Threshold);*/
	/*L3G4200D_REG_WRITE(INT1_TSH_YH_ADD,u8Threshold);*/
	
     DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(INT1_TSH_YL_ADD);
    SPI_u8TransfereByte(u8Threshold);
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));

	u8Threshold = (u8)L3G4200D_ConfigParam.strIntCfg.u16ThresholdZ;
	/*L3G4200D_WRITE_INT1_TSH_ZL(u8Threshold);*/
	/*L3G4200D_REG_WRITE(INT1_TSH_ZL_ADD,u8Threshold);*/
     DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(INT1_TSH_ZL_ADD);
    SPI_u8TransfereByte(u8Threshold);
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));
	u8Threshold = (u8)((u16)(L3G4200D_ConfigParam.strIntCfg.u16ThresholdZ>>8U));
	/*L3G4200D_WRITE_INT1_TSH_ZH(u8Threshold);*/
	/*L3G4200D_REG_WRITE(INT1_TSH_ZH_ADD,DATA);*/

    DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);

    SPI_u8TransfereByte(INT1_TSH_ZH_ADD);
    SPI_u8TransfereByte(u8Threshold);
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));
	
	/*Write Interrupt Duration*/
	/*L3G4200D_WRITE_INT1_DURATION(L3G4200D_ConfigParam.strIntCfg.u8IntDuration);*/
	/*L3G4200D_REG_WRITE(INT1_DURATION_ADD,L3G4200D_ConfigParam.strIntCfg.u8IntDuration);*/


    DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(INT1_DURATION_ADD);
    SPI_u8TransfereByte(L3G4200D_ConfigParam.strIntCfg.u8IntDuration);
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));

	/*Write Interrupt Activation*/
	u8RegData3 = (((u8)L3G4200D_ConfigParam.strIntCfg.u8XAxisInterrupt) | ((u8)L3G4200D_ConfigParam.strIntCfg.u16ThresholdY) | ((u8)L3G4200D_ConfigParam.strIntCfg.u16ThresholdZ));
	/*L3G4200D_WRITE_INT1_CFG(u8RegData);*/
	/*L3G4200D_REG_WRITE(INT1_CFG_ADD,u8RegData);*/

    DIO_WritePort(PB,(((u8)(~data1))<<4u),(1u)<<4u);
    SPI_u8TransfereByte(INT1_CFG_ADD);
    SPI_u8TransfereByte(u8RegData3);
    DIO_WritePort(PB,(1U<<4U),(1U<<4U));
}
#endif
