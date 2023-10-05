/*
 * timerB.c
 *
 *  Created on: 2022. 2. 9.
 *      Author: lucky
 */


/*
 * timerB.c
 *
 *  Created on: 2022. 2. 9.
 *      Author: lucky
 */


#include "timerB.h"
#include "tim.h"
extern TIM_HandleTypeDef htim4;


static timerBcb timerB_callbacks[TIMERB_CCR_NUMBER+1];
static uint16_t timerB_periods[TIMERB_CCR_NUMBER];



void timerB_init()
{
    uint16_t i;

    // stop everything
    //TBCTL = 0;
    HAL_TIM_Base_Stop(&htim4);
    HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_1);
    HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_2);
    HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_3);



//    // clear the CCR and CCTL registers, and the associated callbacks
//    for (i=0;i<TIMERB_CCR_NUMBER;i++)
//    {
//        TBCCTLx[i] = 0;
//        TBCCRx[i] = 0;
//        timerB_callbacks[i] = 0x0;
//        timerB_periods[i] = 0;
//    }

	// clear the CCR and CCTL registers, and the associated callbacks
	for (i=0;i<TIMERB_CCR_NUMBER;i++)
	{
		timerB_callbacks[i] = 0x0;
		timerB_periods[i] = 0;
	}


    // clear the overflow callback
    timerB_callbacks[TIMERB_CCR_NUMBER] = 0x0;

	HAL_TIM_Base_Start(&htim4);

}




uint16_t timerB_register_cb (uint16_t alarm, timerBcb f)
{
    if (alarm > TIMERB_CCR_NUMBER)
    {
        return 0;
    }

    timerB_callbacks[alarm] = f;

    if (alarm == TIMERB_ALARM_OVER)
    {
        // if callback is NULL, disable overflow interrupt
//        if (f == 0x0)
//        {
//            TBCTL &= ~(TBIE);
//        }
//        // if not NULL, enable OF interrupt
//        else
//        {
//            TBCTL |= TBIE;
//        }
    }
    return 1;
}

uint16_t timerB_time()
{
    return htim4.Instance->CNT;
}

void timerB_stop()
{
    // stop mode
    //TBCTL &= ~(MC0|MC1);
	HAL_TIM_Base_Stop(&htim4);
	HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_1);
	HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_2);
	HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_3);
}

uint16_t timerB_set_alarm_from_now  (uint16_t alarm, uint16_t ticks, uint16_t period)
{
    uint16_t now;
    now = timerB_time();

    if (alarm >= TIMERB_CCR_NUMBER)
    {
        return 0;
    }
    switch ( alarm )
    {
		case 0:

			TIM4->CCR1 = now + ticks;
			timerB_periods[0] = period;
			__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC1);
			break;
		case 1:
			TIM4->CCR2 = now + ticks;
			timerB_periods[1] = period;
			__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC2);
			break;
		case 2:
			TIM4->CCR3 = now + ticks;
			timerB_periods[2] = period;
			__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC3);
			break;
    }

//    TBCCTLx[alarm] = CCIE;


    return 1;
}

uint16_t timerB_set_alarm_from_time (uint16_t alarm, uint16_t ticks, uint16_t period, uint16_t ref)
{
    if (alarm >= TIMERB_CCR_NUMBER)
    {
        return 0;
    }

    //TBCCRx[alarm] = ref + ticks;
    //TBCCTLx[alarm] = CCIE;
    switch ( alarm )
    {
    	case 0:
    		__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_CC1);
			TIM4->CCR1 = ref + ticks;
			timerB_periods[0] = period;
			__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC1);
			break;
		case 1:
			__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_CC2);
			TIM4->CCR2 = ref + ticks;
			timerB_periods[1] = period;
			__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC2);
			break;
		case 2:
			__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_CC3);
			TIM4->CCR3 = ref + ticks;
			timerB_periods[2] = period;
			__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC3);
			break;
    }


    return 1;
}

uint16_t timerB_unset_alarm(uint16_t alarm)
{
    if (alarm >= TIMERB_CCR_NUMBER)
    {
        return 0;
    }

    switch ( alarm )
	{
		case 0:
			__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC1);
			__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_CC1);
			TIM4->CCR1 = 0;
			timerB_periods[alarm] = 0;
			timerB_callbacks[alarm] = 0;

			break;
		case 1:
			__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC2);
			__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_CC2);
			TIM4->CCR2 = 0;
			timerB_periods[alarm] = 0;
			timerB_callbacks[alarm] = 0;
			break;
		case 2:
			__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC3);
			__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_CC3);
			TIM4->CCR3 = 0;
			timerB_periods[alarm] = 0;
			timerB_callbacks[alarm] = 0;

			break;
	}


    return 1;
}


void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	//if ( htim->Instance != TIM4 ) return;

	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		if (timerB_periods[0])
		{
			TIM4->CCR1 += timerB_periods[0];
		}
		else
		{
			__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC1);
			TIM4->CCR1 = 0;

		}
		if (timerB_callbacks[0])
		{
 			if ( timerB_callbacks[0]() )
			{

			}
		}
	}
	else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		if (timerB_periods[1])
		{
			TIM4->CCR2 += timerB_periods[1];
		}
		else
		{
			__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC2);
			TIM4->CCR2 = 0;

		}
		if (timerB_callbacks[1])
		{
			if ( timerB_callbacks[1]() )
			{

			}
		}
	}
	else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{
		if (timerB_periods[2])
		{
			TIM4->CCR3 += timerB_periods[2];
		}
		else
		{
			__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC3);
			TIM4->CCR3 = 0;

		}
		if (timerB_callbacks[2])
		{
			if ( timerB_callbacks[2]() )
			{

			}
		}
	}
}

