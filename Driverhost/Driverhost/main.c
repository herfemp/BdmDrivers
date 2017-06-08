#include "Common.h"

#define TIMER_TOP 62


// Benchmarks when flashing real data from file instead of 0-fill:
// T8 main: 38,2 Secs
// T8 MCP : 17~  Secs
// T7: 17,1 Secs
// T5:
//  tn28f010 : 11,1 Secs. Kid you not ;)
// sst39sf020:  7,1 Secs (for 0 - 0x40000, ie half of the flash)
// cat28f010 : 11,3 Secs. Box 1
// cat28f010 : 11,2 Secs. Box 2

// With new write-routine:
// t5:
// tn28f010 : 9,3
//  39sf020 : 5,5 (for 0 - 0x40000, ie half of the flash) -This is getting hilarious! ..Same time @20,1 MHz so this is clearly the host that is limiting.
// cat28f010: both @ 9,2~ 
// T7: 13,2~
// T8: 29,6~

// Even more improved:
// T5:
// 39sf020  : (for 0 - 0x40000, ie half of the flash) 5,15 secs
//  tn28f010: 8,9
// cat28f010: 8,87
// T7: 12,62
// T8: 28,9

int main(void){

	timer_IRQ_init();
	EnableGlobalInt(); 
	InitBDMpins();

	lcd_init();
	lcd_clrscr();
	lcd_puts("Hello", 0);

	f_mount(&FatFs, "", 0);

	// Crank SPI to eleven
	SPCR = (1<<SPE)|(1<<MSTR) | 0;
	SPSR = (1<<SPI2X);

	SetPinDir(MCP2515_CS_1, 1);
	WritePin(MCP2515_CS_1, 1);
	SetPinDir(1, 5, 1);
	SetPinDir(1, 3, 1);
	SetPinDir(1, 4, 0);

	uint16_t Bufst[2];
	uint16_t Driv[2];
	Bufst[0] = 0xF;
	Bufst[1] = 0xFFFC;
	Driv[0]  = 0x10;
	Driv[1]  = 0x400;

	Systype = 0;
	if (ResetTarget() && StopTarget())
		PrepT();

	if(Systype == 1){

		if(f_open( &Fil, "t5.bin", FA_OPEN_EXISTING | FA_READ ) == FR_OK){
			if(!Flash(&Bufst[0], &Driv[0], 4)){
				lcd_puts("fail", 0);
				ShowAddr(1, bdmresp16);
				while(1){};
			}
		}
	}
	else if(Systype == 2){

		if(f_open( &Fil, "t7.bin", FA_OPEN_EXISTING | FA_READ ) == FR_OK){
			if(!Flash(&Bufst[0], &Driv[0], 8)){
				lcd_puts("fail", 0);
				ShowAddr(1, bdmresp16);
				while(1){};
			}
		}
	}else if (Systype == 3){

		if(f_open( &Fil, "t8.bin", FA_OPEN_EXISTING | FA_READ ) == FR_OK){
			if(!Flash(&Bufst[0], &Driv[0], 0x10)){
				lcd_puts("fail", 0);
				ShowAddr(1, bdmresp16);
				while(1){};
			}
		}else
		lcd_puts("nof", 0);
	}else if (Systype == 4){

		if(!FlashMCP()){
			lcd_puts("fail", 0);
			ShowAddr(1, bdmresp16);
			while(1){};
		}
	}
	f_close(&Fil);
	ResetTarget();


	// lcd_puts("Running?", 0);
	while (1){};

	return 0;
}

void timer_IRQ_init(void) {

	TCCR0A     = (1<<WGM01);
	TCCR0B     = (1<<CS02)|(0<<CS01)|(0<<CS00); ///< Fosc/256
	OCR0A      = TIMER_TOP;  
	TIMSK0	   = (1<<OCIE0A);
}

void sleep(uint16_t time){

	MiscTime = time;
	do{}while(MiscTime);
}

ISR(TIMER0_COMPA_vect){

		if (BenchTime) BenchTime--;
		if (MiscTime)  MiscTime--;
}