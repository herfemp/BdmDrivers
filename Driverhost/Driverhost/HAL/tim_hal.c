//-----------------------------------------------------------------------------
//	CAN/BDM adapter firmware
//	(C) Janis Silins, 2010
//	$id$
//-----------------------------------------------------------------------------
//Further hacked and dashed, most certainly also ruined, by Christian Ivarsson.



#include "../common.h"
#include "HAL.h"
#include "tim_hal.h"



void Install_Timer(uint32_t ms, uint32_t timer, uint32_t prio, uint8_t mtchstop){
#ifndef AVR
#if defined(STM32F10X_MD) || defined(STM32F30X)

	TIM_ITConfig(((TIM_TypeDef *) (APB1PERIPH_BASE + ((timer-2)*0x400))), TIM_IT_Update, DISABLE);

	EnableClk(timer+6); ///< Just to use created functions
//	RCC_APB1PeriphClockCmd(1<<(timer-2), ENABLE);

	TIM_TimeBaseStructure.TIM_Prescaler = (ms-1);
	TIM_TimeBaseStructure.TIM_Period = ((SystemCoreClock) / 1000) - 1;//(ms);
	TIM_TimeBaseInit(((TIM_TypeDef *) (APB1PERIPH_BASE + ((timer-2)*0x400))), &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(((TIM_TypeDef *) (APB1PERIPH_BASE + ((timer-2)*0x400))), TIM_IT_Update);
	//TODO: Install Interrupt here? Well it actually SHOULD be initialized before activating TIM_IT
	//but that would force me to create more doubles. Keep an eye on this
	TIM_ITConfig(((TIM_TypeDef *) (APB1PERIPH_BASE + ((timer-2)*0x400))), TIM_IT_Update, ENABLE);

#else

	// configure timer
	TIM_TIMERCFG_Type tc;
	TIM_ConfigStructInit(TIM_TIMER_MODE, &tc);
	TIM_Init(((    TIM_TypeDef *)     timers[timer]), TIM_TIMER_MODE, &tc);

	// set up match register
	TIM_MATCHCFG_Type mc;
	mc.MatchChannel = 0;
	mc.IntOnMatch = ENABLE;
	mc.StopOnMatch = mtchstop;
	mc.ResetOnMatch = ENABLE;
	mc.ExtMatchOutputType = 0;

#ifdef _LPC23XX_ //Yes it actually IS running at one Mhz. This isn't right but I don't have time to spend days in the lpc23xx bible again.
	mc.MatchValue = (((1000000/1000)*ms)-1);
#else
	mc.MatchValue = (ms * CLKPWR_GetPCLK(CLKPWR_PCLKSEL_TIMER0)) / 1000;
#endif
	TIM_ConfigMatch(((    TIM_TypeDef *)     (timers[timer])), &mc);

//	InstallINT(timer, prio);
//	TIM_Cmd(((    TIM_TypeDef *)     (timers[timer])), ENABLE);// enable timer

#endif

	//Must really do something neater than this
	if(mtchstop==0) mtchstop=1;
	else 			mtchstop=0;

	InstallINT(timer, prio);
	TIM_Cmd(((    TIM_TypeDef *)     (timers[timer])), mtchstop);
#endif	
}





void ResetTimer(uint32_t Timer){
#ifndef AVR
	TIM_Cmd(((    TIM_TypeDef *)     timers[Timer]), DISABLE);
#if !defined(STM32F10X_MD) && !defined(STM32F30X)
	TIM_ResetCounter(((    TIM_TypeDef *)     (timers[Timer])));
#endif
	ClearTimerINT(Timer);
#endif
}


void StopTimer(uint32_t Timer){
#ifndef AVR
	TIM_Cmd(((    TIM_TypeDef *)     (timers[Timer])), DISABLE);
	ClearTimerINT(Timer);
#endif
}


void StartTimer(uint32_t Timer){
#ifndef AVR
	ClearTimerINT(Timer);
	TIM_Cmd(((    TIM_TypeDef *)     (timers[Timer])), ENABLE);
#endif
}



void ClearTimerINT(uint32_t INT){
#ifndef AVR
#if defined(STM32F10X_MD) || defined(STM32F30X)
	TIM_ClearITPendingBit(((TIM_TypeDef *) (timers[INT])), TIM_IT_Update);
#else
	((    TIM_TypeDef *)     (timers[INT]))->IR = 0xffffffff;
#endif
	ClearINT(INT); //Not really used by the stm ports but left here for simplicity in case it's needed later on.
#endif
}



//DWT timers. Used only on STM32
#if defined(STM32F10X_MD) || defined(STM32F30X)

void DWT_Init(void){
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)){

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}
}

uint32_t DWT_Get(void){return DWT->CYCCNT;}

__inline uint8_t DWT_Compare(int32_t tp){
	return (((int32_t)DWT_Get() - tp) < 0);
}

void DWT_Delay(uint32_t hz){
	int32_t tp = DWT_Get() + hz;
	while (DWT_Compare(tp));
}

void usleep(uint32_t us){
	DWT_Delay(us * (SystemCoreClock/1000000));
}

#endif

