#include "common.h"
#include "HAL/HAL.h"
#include "BDM.h"
#include <avr/io.h>
#include "HAL/hd44780/hd44780.h"



/*
 * Anyprep.c
 *
 * Created: 2016-04-29 02:17:02
 *  Author: Dev
 */ 
#include <avr/io.h>
#include "BDM.h"
#include "common.h"
#include "HAL/HAL.h"


 uint8_t cpu2_testseq=0;



///< 4 MHz
uint16_t ClockSet=(
	1	<<15 | // X (Divide by 2=0)
	6	<<12 | // W (Multiplier) ///< It did NOT like 7 so even 6 is probably a bit too much.
	//0	<< 8 | // Y (Divider)
	0	<< 7 //| // EDIV (Divide by 16=1)
	//0	<< 5 | // LOSCD
);



///0 Reserved/undef
///1 User data
///2 User program data
///3 Undef/reserved
///4 Undef/reserved
///5 Supervisor data
///6 Supervisor program data
///7 CPU
uint8_t DFC = 0x05;
uint8_t SFC = 0x05;



void PrepT(){

	BDM_DEL=50;
	
	lcd_clrscr();
	lcd_puts("Bdm init", 0);

	Exec_WriteCMD(   0,      0, W_SREG_BDM+0x0e, 0, SFC); ///< Source Function Register (SFC)
	Exec_WriteCMD(   0,      0, W_SREG_BDM+0x0f, 0, DFC); ///< Destination Function Register (DFC)


	lcd_clrscr();
	lcd_puts("SFC_DFC", 0);


	 
	///< Is it Trionic 8 CPU1?
	Exec_ReadCMD(0xFF, 0xFA08, READ16_BDM);

	lcd_clrscr();
	lcd_puts("Check 1", 0);


	// If given the wrong response, assume T5 or MCP ( Maybe T7 too? )
	if(bdmresp != 0xA908){ 
		// Do another check. Is it MCP?
		Exec_ReadCMD(0xFF, 0xFA04, READ16_BDM);
		 

		lcd_clrscr();
		lcd_puts("Check 2", 0);


		// If given the wrong response, assume T5
		if(bdmresp != 0x3008) {
			ClockSet=0x7F00;	// 16,67
			lcd_clrscr();
			lcd_puts("T5", 0);
			// ClockSet=0xD300; // 20,9 mhz Overclock.
		}else { cpu2_testseq=1;
			lcd_clrscr();
			lcd_puts("MCP", 0);
		}
		
		// ShowAddr(0, bdmresp);
		Exec_WriteCMD(0xFF, 0xFA04, WRITE16_BDM,   0, ClockSet);

	}
	 

	///< ..
	if( cpu2_testseq ){

		UBRR0 = 0;
		UBRR0L = 0;
		UBRR0H = 0;
		UCSR0A |= (1 << U2X0);

	}

	UBRR0H = 0;
	UBRR0L = 0;
	
	 
	///< Couldn't get reliable answers by checking the locked-flag. Delay will have to do.
	sleep(100); 
 
	 
	Exec_WriteCMD(0xFF, 0xFA21, WRITE8_BDM , 0, 0);		///< Kill watchdog

}
