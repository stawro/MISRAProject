/*
 * L3G4200D_Lcfg.c
 *
 * Created: 27/09/2015 02:11:03 م
 *  Author: hossam
 */ 
#include "L3G4200D_Lcfg.h"
#include "Basic_Types.h"
#include "L3G4200D_Lcfg.h"
#include "L3G4200D.h"

const L3G4200D_CfgType L3G4200D_ConfigParam = {
                                               {
                                                u8FS_250
                                               },
                                               {
                                                u8X_AXIS_ACTIVE,
											    u8Y_AXIS_ACTIVE,
											    u8Z_AXIS_ACTIVE
                                               },
											   {
											    u8NO_FILTERS,
											    u8NO_FILTERS
											   }
                                               ,
											   {
											    0X2CA4U,
											    0x2CA4U,
											    0x2CA4U,
											    0x00U,
											    u8INT1_X,
											    u8INT1_Y,
											    u8INT1_Z,
											    0U
											   }
};
											   
