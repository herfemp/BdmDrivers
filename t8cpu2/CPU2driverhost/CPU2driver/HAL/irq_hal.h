#if defined(STM32F10X_MD) || defined(STM32F30X)
#ifdef STM32F10X_MD
#include "stm32f10x_conf.h"
#endif

#ifdef STM32F30X
#include "stm32f30x_can.h"
#include "stm32f30x_exti.h"
#endif

//#include "usb_int.h"
//#include "usb_istr.h"
//#include "HW_defs.h"


#endif
//#include "../board/peripherial.h"


#ifdef _LPC23XX_

uint8_t VICs[] = {
		// Timers
		4,  //TIM0
		5,  //TIM1
		26, //TIM2
		27, //TIM3

		// USB
		22,
		// CAN
		23,
};
void *Handlers[] = {
		// Timers
		TIMER0_IRQHandler,
		TIMER1_IRQHandler,
		TIMER2_IRQHandler,
		TIMER3_IRQHandler,

		// USB
		USBHwISR,
		// CAN
		CAN_IRQHandler,
};

#endif

#ifdef _LPC17XX_

#error //None of these have been checked!!

uint8_t Interrupts[] = {
		// Timers
		TIMER0_IRQn,
		TIMER1_IRQn,
		TIMER2_IRQn,
		TIMER3_IRQn,

		// USB
		USB_IRQn,
		// CAN
		CAN_IRQn,
};

#endif


#if defined(STM32F10X_MD) || defined(STM32F30X)

NVIC_InitTypeDef 			NVIC_InitStructure;
uint8_t Interrupts[] = {

		USART2_IRQn,//0,
		USB_LP_CAN1_RX0_IRQn,//1,

		TIM2_IRQn,
		TIM3_IRQn,
		TIM4_IRQn,
#ifndef STM32F10X_MD
		5,
		6,
		TIM7_IRQn,
#endif
};


#endif



