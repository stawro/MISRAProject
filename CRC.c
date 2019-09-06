#include "Basic_Types.h"
/***************************************************************************************/
static u32 GetPower(u32 Base,u32 Pow);
static u16 gen_crc16( u8 *data, u16 size, u32 CRC16);
/***************************************************************************************/
void SECR_CrcPolynomialGenerate(u32* PolynomialPtr,u8 CrcLengthInBits)
{
	u32 DevisorValue;
	DevisorValue = (u32)(GetPower(2u,(u23)CrcLengthInBits)) - 1u;
	*PolynomialPtr = (rand() % DevisorValue) +0x10000u;
}
/***************************************************************************************/
void SECR_GnerateCrc(const u8* PayloadPtr,u16 PayloadLength, u16* CrcPtr, u32 CrcPoly)
{
	u16 LoopIndex;
	static u8 InternalBuffer[8];
	/*Copying data to internal buffer*/
	for (LoopIndex = 0U; LoopIndex < PayloadLength; LoopIndex ++)
	{
		InternalBuffer[LoopIndex] = *(PayloadPtr + LoopIndex);
	}
	/*perform bit wise invert on the data*/
	for (LoopIndex = 0; LoopIndex < PayloadLength; LoopIndex ++)
	{
		InternalBuffer[LoopIndex]  ^= 0xffU;
	}
	/*Generate CRC*/
	*CrcPtr = gen_crc16(InternalBuffer,PayloadLength*8,0x18005);
}
/***************************************************************************************/
static u32 GetPower(u32 Base,u32 Pow)
{
	u32 result = 1U;
	u32 LoopIndex2;
	for (LoopIndex2 = 0U; LoopIndex2 < Pow; LoopIndex2 ++)
	{
		result *= Base;
	}
	return result;
}
/***************************************************************************************/
static u16 gen_crc16( u8 *data, u16 size, u32 CRC16)
{
	u16 out = 0U;
	u16 bits_read = 0U, bit_flag;
	u16 i;
	u16 crc = 0U;
	u16 j = 0x0001U;
	/* Sanity check: */
	if(data != 0U){



	while(size > 0U)
	{
		bit_flag = out >> 15;

		/* Get next bit: */
		out <<= 1;
		out |= (*data >> bits_read) & 1U; // item a) work from the least significant bits

		/* Increment bit counter: */
		bits_read++;
		if(bits_read > 7U)
		{
			bits_read = 0U;
			data++;
			size--;
		}

		/* Cycle check: */
		if(bit_flag)
		out ^= CRC16;

	}

	/* item b) "push out" the last 16 bits*/

	for (i = 0U; i < 16U; ++i) {
		bit_flag = out >> 15;
		out <<= 1;
		if(bit_flag)
		out ^= CRC16;
	}

	/* item c) reverse the bits*/

	i = 0x8000U;

	for (; i != 0U; i >>=1U, j <<= 1U) {
		if (i & out) crc |= j;
	}


	}
	return crc;
}
/***************************************************************************************/
