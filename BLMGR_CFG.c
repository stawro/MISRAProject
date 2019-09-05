/*
 * BLMGR_CFG.c
 *
 * Created: 28/02/2016 06:54:45 م
 *  Author: hossam
 */ 
#include "DIO.h"
#include "BLMGR_CFG.h"
BLMGR_DioPinConfig BuzzerConfig       = {PC,1U<<4U};
BLMGR_DioPinConfig BlueToothPwrConfig = {PC,1U<<5U};
BLMGR_DioPinConfig BluetoothKeyConfig = {PD,1U<<2U};
