/*
 * IRQ.c
 *
 * Created: 1/13/2016 3:07:42 AM
 *  Author: Chriva
 */ 

#include <stdint.h>
#include "IRQ.h"
#include "HAL.h"

#ifdef STM32F103RB
#include "stm32f10x.h"
#endif
void EnableWDT(){
	#ifdef AVR
	wdt_enable(WDTO_15MS);
	#endif
}

void DisableWDT(){
	#ifdef AVR
	MCUSR = 0;
	wdt_disable();
	#endif
	
	
	
}

void DisableInt(uint8_t intr){

#ifdef STM32F103RB
	///< For gods sake fix this!

	if(intr==CANRXINT) UnInstallINT(1, CAN_INT_RX0_PRI);
	if(intr==UARTRXINT){

		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
		UnInstallINT(0, USART_PRIO);}
#endif

	#ifdef AVR
	if(intr==UARTRXINT) UCSR0B &= ~(1<<RXCIE0); ///< Disable RX interrupt
	if(intr==CANRXINT){ PCMSK0 &= ~(1<<PCINT1); }
	#endif
}


void EnableInt(uint8_t intr){

#ifdef STM32F103RB
	///< For gods sake fix this!
	if(intr==CANRXINT) InstallINT(1, CAN_INT_RX0_PRI);
	if(intr==UARTRXINT){

		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		InstallINT(0, USART_PRIO);


	}

#endif
	#ifdef AVR
	if(intr==UARTRXINT) UCSR0B |=  (1<<RXCIE0);
	if(intr==CANRXINT){
		PCMSK0 = (1<<PCINT1); ///< Select PCINT1 (PB1) as interrupt source
		PCICR |= (1<<PCINT0); ///< Select bank0 (PCINT0, 1, 2..)
	}
	
	
	
	
	
	#endif

	
}

void EnableGlobalInt(){/*
#ifdef STM32F103RB

	ClearTimerINT(KHZTMR);
	ClearTimerINT(LED_TIMEOUT);
	Install_Timer(1, KHZTMR, KHZTMR_PRIO, 0);
	Install_Timer(LED_TIMEOUT, LED_TIMER, LED_INT_PRI, 1);
#endif

*/
	#ifdef AVR
	sei();
	#endif
	
}


void DisableGlobalInt(){
	#ifdef AVR
	cli();
	#endif
}
