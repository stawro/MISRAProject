/*
 * KEYPAD.c
 *
 * Created: 30/01/2016 06:38:37 م
 *  Author: hossam
 */ 
#include "DIO.h"
#include "KEYPAD.h"
/*Local Symbols*/
#define KPD_COL_PORT (PD)
#define KPD_ROW_PORT (PD)
#define KPD_COL_MASK 0x70U
#define KPD_ROW_MASK 0x0fU
#define KPD_COL_PIN_NUM 4u
#define KPD_ROW_PIN_NUM 0u
/**************************************************/
/*
//#define KPD_COL_INIT() DIO_vidWritePortDirection(KPD_COL_PORT,KPD_COL_MASK,0x00);
//#define KPD_ROW_INIT() DIO_vidWritePortDirection(KPD_ROW_PORT,KPD_ROW_MASK,0xff);
    //                   DIO_vidWritePortData(KPD_ROW_PORT,KPD_ROW_MASK,0x00)
//
//#define KPD_COL_READ(VALPTR) DIO_vidReadPortData(KPD_COL_PORT,KPD_COL_MASK,(VALPTR));\
//                             *(VALPTR) = (*(VALPTR)) >> KPD_COL_PIN_NUM

#define KPD_ROW_WRITE(DATA) DIO_vidWritePortData(KPD_ROW_PORT,KPD_ROW_MASK,((DATA) << KPD_ROW_PIN_NUM))
 */

void DIO_vidReadPortData(u8 COL_PORT,u8 COL_MASK, u8 * VALPTR){
    *(VALPTR) = (*(VALPTR)) >> KPD_COL_PIN_NUM;
}

void KPD_Init(void)
{
    DIO_vidWritePortDirection(KPD_COL_PORT,KPD_COL_MASK,0x00U);
    DIO_vidWritePortDirection(KPD_ROW_PORT,KPD_ROW_MASK,0xffU);
    DIO_vidWritePortData(KPD_ROW_PORT,KPD_ROW_MASK,0x00U);
	
}

void KPD_ReadVal(unsigned char* ValuePtr)
{
    static const unsigned char KeysLut[]= {'1' , '2' , '3' , '4' , '5' , '6','7' , '8' , '9','*' , '0' , '#'};
    unsigned char temp2;
    unsigned char Rowdata;
	u8 ColData;
	unsigned char LoopTermnate = 0U;
	for(Rowdata = 0U ; (Rowdata < 4U) && (LoopTermnate == 0U) ; Rowdata ++)
	{

	    temp2= ((u8)1U<<Rowdata);
	    temp2 = temp2 << ((u8)KPD_ROW_PIN_NUM);

		DIO_vidWritePortData(KPD_ROW_PORT,KPD_ROW_MASK,temp2);

		DIO_vidReadPortData(KPD_COL_PORT,KPD_COL_MASK,&ColData);

		ColData = (ColData) >> KPD_COL_PIN_NUM;

		if(ColData != 0U)
		{
			*ValuePtr = KeysLut[(Rowdata*3U) + (ColData/2U)];
			LoopTermnate = 1U;
		}
		else
		{
			*ValuePtr = (u8)'n';
		}
	}

	
	
}

