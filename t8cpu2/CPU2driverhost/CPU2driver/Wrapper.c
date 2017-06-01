#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef AVR
#include <avr/io.h>

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#endif

#include "common.h"
#include "config.h"

#include "HAL/HAL.h"



void InitLEDs(){
	
//	Install_Timer(LED_TIMEOUT, LED_TIMER, LED_INT_PRI, 1);
	
	/*
#ifdef AVR
	DDRC &= ~_BV(DDC4);
	PORTC = (1 << 4);
#endif */

	EnableClk(LCD_DB4_PORT);
	EnableClk(LCD_DB7_PORT);


	
	/* Make sure mcp can't mess with spi transfers */
	SetPinDir(MCP2515_CS_1, 1);
	WritePin(MCP2515_CS_1, 1);
	

}






void sleep(uint16_t time){
		SleepTMR=time;
		do{}while(SleepTMR>0);
}


#ifdef AVR
ISR(TIMER0_COMPA_vect){
#endif
#if defined(STM32F10X_MD) || defined(STM32F30X)
void TIM2_IRQHandler(void){
#endif
	if (SleepTMR>0)SleepTMR--;
	if (LCDTMR>0)LCDTMR--;

	if (BenchTmr>0)BenchTmr--;
	if (Menutmr>0)Menutmr-- ;
	


}





