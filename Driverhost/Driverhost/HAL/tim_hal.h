#if defined(STM32F10X_MD) || defined(STM32F30X)
#ifdef STM32F30X
#include "stm32f30x_can.h"
#include "stm32f30x_exti.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_misc.h"
#include "stm32f30x.h"
#endif


#ifdef STM32F10X_MD
#include "stm32f10x_tim.h"
#endif





TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;


#endif


#ifdef _LPC23XX_
#include "../../NXP/ARM7/ARM7_lib/ARM7_lib.h"

uint32_t timers[] = {
		0xE0004000, //TIM0
		0xE0008000, //TIM1
		0xE0070000, //TIM2
		0xE0074000, //TIM3
};

#endif

#if defined(STM32F10X_MD) || defined(STM32F30X)
uint32_t timers[] = {
		0,
		0,
		TIM2_BASE,
		TIM3_BASE,
		TIM4_BASE,
		0,
		TIM6_BASE, //Do not use this one.
		TIM7_BASE,

};

#endif

