#include "Common.h"

#define TIMER_TOP 62

// Benchmarks when flashing real data from file instead of 0-fill:
// T5:
// 39sf020  : (for 0 - 0x40000, ie half of the flash) 4,64 secs
//  tn28f010: 8,21
// cat28f010: 8,18
//  am28f010: 8,62

//  T7: 11,37
//  T8: 22,32
// MCP: 14,1 - 37,2 Secs.

// Dump:
// T5: 2.35 Secs
// T7: 4,61
// t8: 6,92


const char *fname[] = {
	0,
	"t5.bin",
	"t7.bin",
	"t8.bin",
};

int main(void){

	timer_IRQ_init();
	EnableGlobalInt(); 
	InitBDMpins();

	lcd_init();
	lcd_clrscr();
	lcd_puts("Hello", 0);

	f_mount(&FatFs, "", 0);

	// Crank SPI to eleven
	SPCR = (1<<SPE)|(1<<MSTR);
	SPSR = (1<<SPI2X);
	// SPSR =	(1<<SPR0)|(1<<SPR1);

	// Make sure MCP2515 SPI-communication is not enabled
	SetPinDir(MCP2515_CS_1, 1);
	WritePin(MCP2515_CS_1, 1);
	
	// PortB
	SetPinDir(1, 0, 1); // CS out
	WritePin( 1, 0, 1); // CS high
	SetPinDir(1, 3, 1); // MOSI out
	SetPinDir(1, 4, 0); // MISO in
	SetPinDir(1, 5, 1); // SCK out


	// Set this one to 0 to allow for flashing
	Systype = 0;

	if(!Systype && ResetTarget() && StopTarget())
		PrepT();
	else
		Systype = 0;

	if(Systype && Systype<4){

		if(f_open( &Fil, fname[Systype], FA_OPEN_EXISTING | FA_READ ) == FR_OK){

			if(!Flash(Systype<<8)){
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
	
	sleep(2000);

	// Hack hack..
	uint8_t IndFault = 1;
	
	if (ResetTarget() && StopTarget()){
		PrepT();
	
		if(Systype && Systype != 4)
			IndFault = DumpFlash(Systype<<8)?0:1;
		else
			IndFault = 1;
	}
	f_close(&Fil);
	
	// Disable SPI to allow for regular toggling of gpio
	SPCR = 0;

	ResetTarget();
	while (1){
	
		sleep(250);
		PORTB = (PORTB&0xDF)|1<<5;

		sleep(250);
		if(IndFault) PORTB = PORTB&0xDF;
	};

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