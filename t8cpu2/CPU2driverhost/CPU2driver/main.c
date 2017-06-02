#include "Common.h"
#include "HAL/Disk/ff.h"	
#include "HAL/hd44780/hd44780.h"
#include "HAL/HAL.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#define TIMER_TOP 63
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
	EnableGlobalInt(); 
	InitBDMpins();

	lcd_init();
	lcd_clrscr();
	lcd_puts("Hello", 0);

	f_mount(&FatFs, "", 0);

	// Crank SPI to eleven
	SPCR = (1<<SPE)|(1<<MSTR) | 0/*R_SPCR*/;
	SPSR = (1<<SPI2X)/*R_SPSR*/;

	SetPinDir(MCP2515_CS_1, 1);
	WritePin(MCP2515_CS_1, 1);

	SetPinDir(1, 5, 1);
	SetPinDir(1, 3, 1);
	SetPinDir(1, 4, 0);

	if(!FlashMCP()){
		lcd_clrscr();
		lcd_puts("FAIL!", 0);
		while(1){}
	}
	
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

	TCCR0A     = (1<<WGM01);
	TCCR0B     = (1<<CS02)|(0<<CS01)|(0<<CS00); ///< Fosc/256
	OCR0A      = TIMER_TOP;  
	TIMSK0	   = (1<<OCIE0A); 

}

void sleep(uint16_t time){
	SleepTMR=time;
	do{}while(SleepTMR>0);
}



ISR(TIMER0_COMPA_vect){

		if (SleepTMR)SleepTMR--;
		if (LCDTMR)LCDTMR--;
		if (BenchTmr)BenchTmr--;
		if (Menutmr)Menutmr-- ;
}