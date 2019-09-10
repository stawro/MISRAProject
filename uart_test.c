#include "Basic_Types.h"
#include "BLMGR.h"
#include "PWM.h"
#include "GPT.h"
#include "DIO.h"
void Cyclic30ms(void);
u32 main(void)
{
	u32 Count= 0U;
	u32 start = 0U;
	u32 Count2 = 0U;



GPT_Timer30msInit(&Cyclic30ms);
	
	BLMGR_BluetoothInit();
	BLMGR_SetReceiver(ROLE_MAPP);
    
							
	PWM_Init();
	sei();
	PWM_SetSpeed(25U);

	
	while(1)
	{
		Count2 = (Count2 +1U) %20U;
		BLMGR_SetBattLevel((u8)(Count2 / 4U));
		
		_delay_ms(100U);
		Count ++;
		if(start == 0U)
		{
					if(Count > 5U)
					{
						BLMGR_StartDevice();

						start = 1U;
					}
					
		}

	    
		
		
	}
	
}
void Cyclic30ms(void)
{
    static u8 TimeoutCounter = 0U;
    TimeoutCounter ++;
    if(TimeoutCounter == 10U)
    {
        BLMGR_BluetoothStateMachine();
        TimeoutCounter = 0U;


    }
}
