/*
 * L3G4200D_Lcfg.h
 *
 * Created: 27/09/2015 02:11:15 م
 *  Author: hossam
 */ 


#ifndef L3G4200D_LCFG_H_
#define L3G4200D_LCFG_H_

    #include "Basic_Types.h"
    #include "L3G4200D_Cfg.h"
    /*Full Scale Configuration*/
    #define u8FS_250 0x00U
    #define u8FS_500 0x20U
    #define u8FS_2000 0x30U
    #define u8FULL_SCALE (u8FS_250)

    /*Axis Activation Configuration*/
    #define u8AXIS_INACTIVE 0x00U
    #define u8X_AXIS_ACTIVE 0x01U
    #define u8Y_AXIS_ACTIVE 0x02U
    #define u8Z_AXIS_ACTIVE 0x04U

    /*High Pass and low Pass filters Configurations*/
    #define u8NO_FILTERS   0x00U
    #define u8HPF_ON_DATA  0x11U
    #define u8LPF_ON_DATA  0x12U
    #define u8BOTH_ON_DATA 0x13U
    #define u8HPF_ON_INT   0x14U
    #define u8LPF_ON_INT   0x18U
    #define u8BOTH_ON_INT  0x1CU

    /*Self Axis movement detection*/
    #define u8INT_DIS 0x00U
    #define u8INT1_X  0x03U
    #define u8INT1_Y  0x0CU
    #define u8INT1_Z  0x30U

    /*Axis Activation*/
    typedef struct
    {
        u8 u8XAxisActivation;
        u8 u8YAxisActivation;
        u8 u8ZAxisActivation;
    }strAxisActivationType;

    /*Filter Configuration*/
    typedef struct
    {
        u8 u8FilterOnData;
        u8 u8FilterOnInterrupt;
    }strFilterCfgType;

    /*Interrupt Configuration type*/
    typedef struct
    {
        u16 u16ThresholdX;
        u16 u16ThresholdY;
        u16 u16ThresholdZ;
        u8  u8XAxisInterrupt;
        u8  u8YAxisInterrupt;
        u8  u8ZAxisInterrupt;
        u8  u8AOICfg;
        u8  u8IntDuration;
    }strIntCfgType;

    /*Configuration parameters type*/
    typedef struct
    {
        u8 u8FullScaleValue;
        strAxisActivationType strAxisActivation;
        strFilterCfgType      strFilterCfg;
        strIntCfgType strIntCfg;
    }L3G4200D_CfgType;



    extern const L3G4200D_CfgType L3G4200D_ConfigParam;



#endif /* L3G4200D_LCFG_H_ */
