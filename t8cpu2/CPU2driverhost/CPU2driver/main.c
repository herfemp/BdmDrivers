#include "Common.h"



#include "HAL/Disk/ff.h"	
#include "HAL/hd44780/hd44780.h"

#include "HAL/HAL.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef AVR
#include <avr/io.h>
#include <avr/pgmspace.h>

#define TIMER_TOP 63

#endif

#ifdef STM32F10X_MD
#include "stm32f10x.h"
#endif

/*
#define SPI_PRESCALER 2

#ifdef	SPI_PRESCALER
#if (SPI_PRESCALER == 2) || (SPI_PRESCALER == 8) || (SPI_PRESCALER == 32) || (SPI_PRESCALER == 64)
#define	R_SPSR	(1<<SPI2X)
#define SPI_PRESCALER_ 	(SPI_PRESCALER * 2)
#else
#define	R_SPSR	0
#define	SPI_PRESCALER_	SPI_PRESCALER
#endif

#define	SPI_CLOCK_RATE_BIT0		(1<<SPR0)
#define	SPI_CLOCK_RATE_BIT1		(1<<SPR1)

#if (SPI_PRESCALER_ == 4)
#define	R_SPCR	0
#elif (SPI_PRESCALER_ == 16)
#define	R_SPCR	SPI_CLOCK_RATE_BIT0
#elif (SPI_PRESCALER_ == 64)
#define	R_SPCR	SPI_CLOCK_RATE_BIT1
#elif (SPI_PRESCALER_ == 128)
#define	R_SPCR	SPI_CLOCK_RATE_BIT1 | SPI_CLOCK_RATE_BIT0
#else
#error	 SPI_PRESCALER must be one of the values of 2^n with n = 1..7!
#endif
#else
#error	SPI_PRESCALER not defined!
#endif
*/
 uint8_t printnumber[5];

 

 
 ///< Convert binary numbers into ascii chars.
 void something(uint16_t input){

	printnumber[4] = input%10;
	input = input/10;
	
	printnumber[3] = input%10;
	input = input/10;

	printnumber[2] = input%10;
	input = input/10;

	printnumber[1] = input%10;
	input = input/10;

	printnumber[0] = input%10;

	printnumber[0] = printnumber[0] | 0x30;
	printnumber[1] = printnumber[1] | 0x30;
	printnumber[2] = printnumber[2] | 0x30;
	printnumber[3] = printnumber[3] | 0x30;
	printnumber[4] = printnumber[4] | 0x30;

 }

void showval_(uint16_t val){
	
	something(val);
	lcd_goto(0x40);
	lcd_puts("Time", 0);
	lcd_goto(0x45);
	lcd_puts(printnumber,5);
}




int main(void){
	
	
	timer_IRQ_init(); 

	InitLEDs();
	 EnableGlobalInt(); 

	InitBDMpins(); 

	

	lcd_init();
	lcd_clrscr();
	lcd_puts("Hello", 0);

	
	f_mount(&FatFs, "", 0);

	
	SPCR = (1<<SPE)|(1<<MSTR) | 0/*R_SPCR*/;
	SPSR = (1<<SPI2X)/*R_SPSR*/;

	SetPinDir(1, 5, 1);
	SetPinDir(1, 3, 1);
	SetPinDir(1, 4, 0);


	FlashMCP();
	
	DDRC &= ~_BV(DDC4);
	PORTC = (1 << 4);


	showval_(benchtime);


	while (1){
		if (!(PINC & _BV(PB4))){


			if (ResetTarget() && StopTarget()){
				
				lcd_clrscr();
				lcd_puts("Running?", 0);
				bootstrapmcp();
				
			}
		}
	






	}
	return 0;
}
void timer_IRQ_init(void) {


#ifdef AVR

	TCCR0A     = (1<<WGM01);
	TCCR0B     = (1<<CS02)|(0<<CS01)|(0<<CS00); ///< Fosc/256
	OCR0A      = TIMER_TOP;  
	TIMSK0	   = (1<<OCIE0A); 

#endif

}

