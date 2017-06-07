#include "../common.h"
#include "bdm.h"
#include "regdef.h"

//< Mayhem..
uint16_t FMCR = (
0 << 15 | // stop
0 << 14 | // VEN

0 << 11 | // RLCK Lock set ram base
1 << 10 | // DLY1
0 <<  8   // RASP 0 urestricted, 1 unrestricted program, 2 sup prog and data, 3 sup prog
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


///< 0 = Discrete output
///< 1 = Alternate Function
///< 2 = Chip Select  8-bit port
///< 3 = Chip Select 16-bit port
uint16_t CSPAR0=(
3 << 12 | //CSPA0[6]
3 << 10 | //CSPA0[5]
3 <<  8 | //CSPA0[4]
3 <<  6 | //CSPA0[3]
3 <<  4 | //CSPA0[2]
3 <<  2 | //CSPA0[1]
3		  //CSBOOT (Can only be 2 or 3?)
);

//< Mayhem..
///< 0 = Discrete output
///< 1 = Alternate Function
///< 2 = Chip Select  8-bit port
///< 3 = Chip Select 16-bit port
uint16_t CSPAR0_t7=(
2 << 12 | //CSPA0[6] CS5
2 << 10 | //CSPA0[5] CS4
2 <<  8 | //CSPA0[4] CS3

//< Flash
3 <<  6 | //CSPA0[3] CS2
3 <<  4 | //CSPA0[2] CS1
3 <<  2 | //CSPA0[1] CS0 // can only be 16 bit port (3) or alternate function (2).
3		  //CSBOOT (Can only be 2 or 3?)
);

///< Bsize / addr pin =
///0    2K Addr 23:11
///1    8K 13
///2   16K 14
///3   64K 16
///4  128K 17
///5  256K 18
///6  512K 19
///7 1024K / 1M
///Start Address 23-11 (bit 15-3) ///< Blocksize
 uint16_t CSBARBT = (
 ///Start Address 23-11 (bit 15-3)
 7 ///< Blocksize
 );

///< Bsize / addr pin =
///0    2K Addr 23:11
///1    8K 13
///2   16K 14
///3   64K 16
///4  128K 17
///5  256K 18
///6  512K 19
///7 1024K / 1M
///Start Address 23-11 (bit 15-3) ///< Blocksize
uint16_t CSBARBT_t7 = ( 8<<8 | 7 );



 ///< Chip-Select Option Register Boot ROM
 uint16_t CSORBT = (
 0<<15 | /// Mode: 0 Async, 1 Sync
 3<<13 | /// Byte: 0 Disable, 1 Low byte, 2 High Byte, 3 Both bytes. (Setting only useful when 16 bit port io is selected in pin assign reg.)
 1<<11 | /// RW: 0 Reserved, 1 Read only, 2 write only, 3 read and write. //default: 1
 0<<10 | /// STRB: 0 Addr, 1 Data strobe
 0<< 6 | /// DSACK:  no. of wait states in async mode, 14=Fast termination, 15=external dsack
 3<< 4 | /// SPACE:  0 = CPU, 1 = User, 2 = Supervisor, 3 = Supervisor/User
 0<< 1 | /// IPL:  Interrupt priority level. 0 = any, 1-7=level n
 0       /// AVEC: 0 External Int vec enable, 1 autovector enable
 );



void PrepTrionic82(){
	
	Exec_WriteCMD(0xFF, 0xfb04, WRITE16_BDM,   0, 8);
	Exec_WriteCMD(0xFF, 0xfb06, WRITE16_BDM,   0, 0);
	Exec_WriteCMD(0xFF, 0xfb00, WRITE16_BDM,   0, 0x800);
}

///< Set up SRAM. -Since the manual can't decide if it has 8 ,16, 32 or anything in between (or more) K of ram I can't complete this function. 
 ///< ..It has at least 32K btw. 
 void PrepTrionic81(){
	 
	///< Stop SRAM
	Exec_WriteCMD(0xFF, FASRAM_Base, WRITE16_BDM,   0, 0x8400);
	 
	///< Set up addr, "Main" SRAM 8Kbytes, or 32K.. (Stupid Manual.)
	// Exec_WriteCMD(0xFF, 0xfb04, WRITE16_BDM,   0, RambaseH);
	Exec_WriteCMD(0xFF, 0xfb04, WRITE16_BDM,   0, 0x1000); // Haven't checked this yet. base sram @0
	Exec_WriteCMD(0xFF, 0xfb06, WRITE16_BDM,   0, 0);
	// .. Set up the 512B blocks?

	///< Start SRAM
	Exec_WriteCMD(0xFF, FASRAM_Base, WRITE16_BDM,   0, FMCR);
	 

	///< Stop DPTRAM
	/*Exec_WriteCMD(0xFF, DPTMCR_Base, WRITE16_BDM,   0, 0x8000);
	///< Start DPTRAM
	Exec_WriteCMD(0xFF, DPTMCR_Base, WRITE16_BDM,   0, 0);
	Exec_WriteCMD(0xFF, 0xF684, WRITE16_BDM,   0, 0xff20); */

}

void PrepTrionic5(){
		 ///< T5 stuff.
		 Exec_WriteCMD(0xFF, 0xFA44, WRITE16_BDM,   0, CSPAR0);  ///< Set up CS pins.
		 Exec_WriteCMD(0xFF, 0xFA48, WRITE16_BDM,   0, CSBARBT); ///< CS Base Addr
		 Exec_WriteCMD(   0,      0, FILL16_BDM,    0, CSORBT);  ///< Chip-Select Option Register Boot ROM
		 
		 
		 Exec_WriteCMD(0xFF, 0xFA50, WRITE16_BDM,   0, 0x7);	 ///< CSBAR1, BS1 M, base @ 0 ? Addr 23:11 = 0
		 Exec_WriteCMD(   0,      0, FILL16_BDM,    0, 0x3030);  ///< CSOR 1
		 Exec_WriteCMD(   0,      0, FILL16_BDM,    0, 0x0007);  ///< CSBAR2, BS1 M,
		 Exec_WriteCMD(   0,      0, FILL16_BDM,    0, 0x5030);  ///< CSOR 2

		 ///< Disabled since I'm playing with newer flash. (Which released the magic smoke even though it was N/C.. ops! ;) )
		 // Exec_WriteCMD(0xFF, 0xFC14, WRITE16_BDM,   0, 0x40); ///< Set PQS6 to high(vpp)
		 // Exec_WriteCMD(0xFF, 0xFC17, WRITE8_BDM,    0, 0x40); ///< Set PQS6 to output
		 
		 sleep(50);
		 Exec_WriteCMD(0xFF, 0xFB04, WRITE16_BDM,   0, 0x1000); ///< Link DPTRAM to Addr 0x10000


}

void PrepTrionic7(){

/*	Exec_WriteCMD(0xFF, 0xFA44, WRITE16_BDM,   0, 0);
	Exec_WriteCMD(0xFF, 0xFA46, WRITE16_BDM,   0, 0);

	Exec_WriteCMD(0xFF, 0xFA16, WRITE16_BDM,   0, 0); // PEPAR
	Exec_WriteCMD(0xFF, 0xFA1E, WRITE16_BDM,   0, 0); // PFPAR
	Exec_WriteCMD(0xFF, 0xFA14, WRITE16_BDM,   0, 0xFFFF); // DDRE
	Exec_WriteCMD(0xFF, 0xFA1C, WRITE16_BDM,   0, 0xFFFF); // DDRF

	Exec_WriteCMD(0xFF, 0xFC16, WRITE8_BDM,   0, 0); // PQSPAR
	Exec_WriteCMD(0xFF, 0xFC17, WRITE8_BDM,   0, 0xFF); // DDRQS

	Exec_WriteCMD(0xFF, 0xFC0A, WRITE16_BDM,   0, 0); // SCCR1


	// Enable sram? Weird one..
	Exec_WriteCMD(0xFF, 0xFB40, WRITE16_BDM,   0, 0x8000);
	// Exec_WriteCMD(0xFF, 0xFB44, WRITE32_BDM,   0xFFFF, 0xD000); //0xFFFF D000

	Exec_WriteCMD(0xFF, 0xFB44, WRITE16_BDM,   0, 0xFFFF); //0xFFFF D000
	Exec_WriteCMD(0xFF, 0xFB46, WRITE16_BDM,   0, 0xD000); //0xFFFF D000
	Exec_WriteCMD(0xFF, 0xFB40, WRITE16_BDM,   0, 0);	
	
	printf("T7 Test.. \n\r");

	do{ 
		Exec_WriteCMD(0xFF, 0xFA10, WRITE16_BDM,   0, 0xFFFF); // PE0	
		Exec_WriteCMD(0xFF, 0xFA12, WRITE16_BDM,   0, 0xFFFF); // PE1
		Exec_WriteCMD(0xFF, 0xFA18, WRITE16_BDM,   0, 0xFFFF); // PF0
		Exec_WriteCMD(0xFF, 0xFA1A, WRITE16_BDM,   0, 0xFFFF); // PF1
		Exec_WriteCMD(0xFF, 0xFC14, WRITE16_BDM,   0, 0xFFFF); // Pq

		Exec_WriteCMD(0xFF, 0xFA41, WRITE8_BDM,   0, 0xFF); // c	
		sleep(250);


		Exec_WriteCMD(0xFF, 0xFA10, WRITE16_BDM,   0, 0); // PE0
		Exec_WriteCMD(0xFF, 0xFA12, WRITE16_BDM,   0, 0); // PE1
		Exec_WriteCMD(0xFF, 0xFA18, WRITE16_BDM,   0, 0); // PF0
		Exec_WriteCMD(0xFF, 0xFA1A, WRITE16_BDM,   0, 0); // PF1
		Exec_WriteCMD(0xFF, 0xFC14, WRITE16_BDM,   0, 0); // Pq


		Exec_WriteCMD(0xFF, 0xFA41, WRITE8_BDM,   0, 0); // c	
		sleep(250);
	}while(1);


	*/


	Exec_WriteCMD(0xFF, 0xFA44, WRITE16_BDM,   0, 0x2FFF); // CSPAR0 (CSPA0[6] Set as 8 bit, all other 16 bit) (Set port c2, 1, 0?)
	
	// Enable Addr[19] line.. WHY??
	Exec_WriteCMD(0xFF, 0xFA46, WRITE16_BDM,   0, 0x0001); // CSPAR1 (CSPA1[0] Alt. function )
	
	Exec_WriteCMD(0xFF, 0xFA48, WRITE16_BDM,   0, 0x0006); // CSBARBT
	Exec_WriteCMD(0xFF, 0xFA4A, WRITE16_BDM,   0, 0x6BB0); // CSORBT

	Exec_WriteCMD(0xFF, 0xFA50, WRITE16_BDM,   0, 0x0006); // CSBAR1
	Exec_WriteCMD(0xFF, 0xFA52, WRITE16_BDM,   0, 0x1030); // CSOR1 

	// All are set to Supervisor/user space
	// Base: 0xF0
	// Mode: Asynchronous
	// B/S : 64K
	Exec_WriteCMD(0xFF, 0xFA4C, WRITE16_BDM,   0, 0xF003); // CSBAR0
	Exec_WriteCMD(0xFF, 0xFA4E, WRITE16_BDM,   0, 0x6830); // CSOR0 // Both bytes (16 bit), R/W Set to 01 Read only
	Exec_WriteCMD(0xFF, 0xFA58, WRITE16_BDM,   0, 0xF003); // CSBAR3 
	Exec_WriteCMD(0xFF, 0xFA5A, WRITE16_BDM,   0, 0x5030); // CSOR3 // Upper byte, R/W set to 10 Write only
	Exec_WriteCMD(0xFF, 0xFA5C, WRITE16_BDM,   0, 0xF003); // CSBAR4 
	Exec_WriteCMD(0xFF, 0xFA5E, WRITE16_BDM,   0, 0x3030); // CSOR4 // Lower byte, R/W set to 10 Write only

	// Set up Canctrl
	Exec_WriteCMD(0xFF, 0xFA60, WRITE16_BDM,   0, 0xFF00); // CSBAR5
	Exec_WriteCMD(0xFF, 0xFA62, WRITE16_BDM,   0, 0x7BF0); // CSOR5

	// Goes to??
	Exec_WriteCMD(0xFF, 0xFA70, WRITE16_BDM,   0, 0xFFF8); // CSBAR9
	Exec_WriteCMD(0xFF, 0xFA72, WRITE16_BDM,   0, 0x2BC7); // CSOR9

	Exec_WriteCMD(0xFF, 0xFB04, WRITE16_BDM,   0, 0xFFE8); // 0xFF E800
	Exec_WriteCMD(0xFF, 0xFB00, WRITE16_BDM,   0, 0); // Enable dptram

	// /Init
	Exec_WriteCMD(0xFF, 0xFA41, WRITE8_BDM,   0, 0x1F);

	Exec_WriteCMD(0xFF, 0xFA17, WRITE8_BDM,   0, 0x11);
	Exec_WriteCMD(0xFF, 0xFA13, WRITE8_BDM,   0, 0);
	Exec_WriteCMD(0xFF, 0xFA15, WRITE8_BDM,   0, 0xFE);

	// PFPAR Pf[3] as Int, others regular io
	Exec_WriteCMD(0xFF, 0xFA1F, WRITE8_BDM,   0, 0x8);
	// Pf all pins low
	Exec_WriteCMD(0xFF, 0xFA1B, WRITE8_BDM,   0, 0);

	// DDRF PortF[0, 2 and 7] as output
	Exec_WriteCMD(0xFF, 0xFA1D, WRITE8_BDM,   0, 0x85);
	// /Init

	Exec_ReadCMD(0xFF, 0xFA1B, READ8_BDM);
	// printf("Andfunc1: %04X\n\r", bdmresp16);
	bdmresp16 &= 0x20;
	// printf("Andfunc2: %04X\n\r", bdmresp16);
	// Software would stop here if result is 0 (It is not, obv)

	Exec_ReadCMD(0xFF, 0xFA1B, READ8_BDM);
	bdmresp16 |= 5;
	Exec_WriteCMD(0xFF, 0xFA1B, WRITE8_BDM,   0, 0xFF);

	// Latch up chip enable for SRAM (Wo-motherfawking-whoo!! )
	Exec_WriteCMD(0xFF, 0xF706, WRITE16_BDM,   0, 0x1000);



	Exec_WriteCMD(0xFF, 0xFB04, WRITE16_BDM,   0, 0x1000); ///< Link DPTRAM to Addr 0x10000

}

void PrepT(){

	Systype = 0;

	uint16_t ClockSet;
	lcd_clrscr();
	lcd_puts("Bdm init", 0);

	Exec_WriteCMD_s(   0,      0, W_SREG_BDM+0x0e, 0, SFC); ///< Source Function Register (SFC)
	Exec_WriteCMD_s(   0,      0, W_SREG_BDM+0x0f, 0, DFC); ///< Destination Function Register (DFC)

	lcd_clrscr();
	lcd_puts("SFC_DFC", 0);
 
	///< Is it Trionic 8 CPU1?
	Exec_ReadCMD_s(0xFF, 0xFA08, READ16_BDM);

	lcd_clrscr();
	lcd_puts("Check 1", 0);

	// Trionic 8, CPU1 (Main)
	if(bdmresp == 0xA908){
		Systype = 3;
		// Multi, max 7 (x9) (28 Mhz) ///Divider, Max 7, 0 div by 2, 1 4, 2 8...
		Exec_WriteCMD_s(0xFF, 0xFA08, WRITE16_BDM,   0, ( 5<<12 |  0<<8 ));

	// If given the wrong response, assume T5, T7 or MCP
	}else{
	
		// Do another check. Is it MCP?
		Exec_ReadCMD_s(0xFF, 0xFA04, READ16_BDM);
		
		if(bdmresp == 0x3008){       // Trionic 8 CPU2 (MCP)
					Systype = 4;
			ClockSet = MCPClock;
		}else if(bdmresp == 0x3F08){ // Trionic 5
			Systype = 1;
			ClockSet=0x7F00; // 16,67
			//ClockSet=0xD300; // 20,9 mhz Overclock.
		}else{                       // Trionic 7	
		Systype = 2;
		// ClockSet = 0x3F0F; // ..
		// Meh.. crank everything to fawking eleven!
		ClockSet = (
		0 << 15 | // W - Frequency Control (VCO)
		0 << 14 | // X - Frequency Control Bit (Prescale)
		0 <<  8 | // Y - Frequency Control (Counter)
		0 <<  7 | // EDIV
		// y = 63: TBT
		// Y = 00: 5826 mS
		// Y = 32: 5826 mS
		0 <<  2 | // RSTEN
		1 <<  1 | // STSIM
		1);       // STEXT

		}
		Exec_WriteCMD_s(0xFF, 0xFA04, WRITE16_BDM,   0, ClockSet);
	}

	///< Couldn't get reliable answers by checking the locked-flag. Delay will have to do.
	sleep(100);

	lcd_clrscr();
	//showval_(bdmresp16);
	// sleep(2000);
	if(Systype == 4){       // MCP
		PrepTrionic82();
		lcd_puts("MCP", 0);
	}else if(Systype == 3){ // T8
		PrepTrionic81();
		lcd_puts("T8", 0);
	}else if(Systype == 2){ // T7
		PrepTrionic7();
		lcd_puts("T7", 0);
	}else if(Systype == 1){ // T5
		PrepTrionic5();
		lcd_puts("T5", 0);
	}else{                  // Unknown
		lcd_puts("Unk. System!", 0);
		while(1){};
	}
	// while(1){};
	//UBRR0 = 0;
	UBRR0H = 0; // 4 bits

	UBRR0L = 7; // 8 bits
	//UBRR0H = 0;
	UCSR0A |= (1 << U2X0);

	// Kinda unstable but stupidly fast!
//	UBRR0L = 1; // 8 bits
//	UCSR0A |= (1 << U2X0);


	if(Systype > 2){
		UBRR0L = 0;
		UCSR0A |= (1 << U2X0);
	}


/*	UBRR0H = 0;
	UBRR0L = 0;*/
	

 
	if(Systype != 3)
		Exec_WriteCMD(0xFF, 0xFA21, WRITE8_BDM , 0, 0);		///< Kill watchdog

	// while(1){};
}
