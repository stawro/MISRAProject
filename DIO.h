﻿/*
 * DIO.h
 *
 * Created: 24/08/2015 12:51:11 ص
 *  Author: hossam
 */ 


#ifndef DIO_H_
#define DIO_H_

#include "Basic_Types.h"



#define PA 0x00U
#define PB 0x01U
#define PC 0x02U
#define PD 0x03U

void DIO_InitPortDirection(u8 PortName,u8 PortDirection,u8 PortMask);
void DIO_WritePort(u8 PortName,u8 PortData,u8 PortMask);
u8 DIO_ReadPort(u8 PortName,u8 PortMAsk);
void DIO_vidWritePortDirection(u8 COL_PORT,u8 COL_MASK, u8 VALPTR);
void DIO_vidWritePortData(u8 COL_PORT,u8 COL_MASK, u8 VALPTR);
void DIO_vidReadPortData(u8 COL_PORT,u8 COL_MASK, u8 * VALPTR);

void _delay_ms(u8 num);
void sei(void);

#endif /* DIO_H_ */
