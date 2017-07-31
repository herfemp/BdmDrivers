#include "../common.h"
#include "../BDM/BDM.h"
#include "../BDM/regdef.h"
#include "TxDriver.bin.h"
uint16_t t5loaderbytes = sizeof(TxDriver_bin);


uint8_t UploadDRV(){

	uint16_t Location = 0;
	uint16_t buf[2];
	Exec_WriteCMD( 0x10, 0x0400-4, WRITE32_BDM, 0, 0);

	do{ buf[0] = pgm_read_word(&TxDriver_bin[Location]);
		buf[1] = pgm_read_word(&TxDriver_bin[Location+2]);
		Exec_FillCMD_p(&buf[0]); // This one will byteswap automatically
		Location+=4;
	}while(Location<t5loaderbytes);

	// This is useless atm..
	return Location >= t5loaderbytes ? 1:0;
}


/* Commands:
 * 3: Init hardware
 * 2: Format flash
 * 1: Write data
 *
 * Stores result in D0
 * 1: OK
 * 0: Fail*/
uint8_t LDRDemand(uint8_t Command, const uint16_t *Addr, uint8_t End){

	///< Store command in D0
	Exec_WriteCMD(0, 0, W_DREG_BDM, 0, Command);

	Exec_WriteCMD(0, 0, W_DREG_BDM+8, 0, 0);   // A0 = Start addr
	Exec_WriteCMD(0, 0, W_DREG_BDM+9, End, 0); // A1 = End addr


	// Set PC to start of driver
	Exec_WriteCMD(0, 0, W_SREG_BDM, Addr[0], Addr[1]);
	Exec_WriteCMD(0, 0, 0, 0, 0);
	ShiftData_s(BDM_GO);

	while(ReadPin(P_FRZ) )
		;

	MiscTime=3;
	do{ if(ReadPin(P_RST) && !ReadPin(P_FRZ)) MiscTime=3;
	}while (MiscTime);
	

	///< Read D0
	Exec_ReadCMD(0, 0, R_DREG_BDM);
	return bdmresp16&0xFF;
}

// The UGLY version! It is faster though..
uint8_t Fbuf[600];
// 9887 with tn28f010. Holy F!
// Write flash data
uint8_t LDRWrite(const uint16_t *Bufstart, const uint16_t *LDRAddr, uint8_t End){

	uint8_t  Had = 0;
	uint16_t Lad = 0;
	uint16_t   i;
	uint16_t buf[2];
	uint8_t cntr;
	uint8_t latch;

	// Prefill buffer
	f_read(&Fil, &Fbuf, 600, &bw); // Read more than 512 bytes to force ff to read two sectors
	if(bw!=600) return 0;

	///< Store command in D0
	Exec_WriteCMD(0, 0, W_DREG_BDM, 0, 1);
	Exec_WriteCMD(0, 0, W_DREG_BDM+8, Had, Lad); // Store addr in A0

	do{ Exec_WriteCMD(Bufstart[0], Bufstart[1], WRITE32_BDM, 0,0); // Ugly solution to start the fill command at the right address..

		
		for(i=0; i<600; i+=4)
			Exec_FillCMD_p((uint16_t *)&Fbuf[i]); // This one will byteswap automatically

		for(i=0; i<106; i++){ // In other words this will already be buffered..
			f_read(&Fil, &buf, 4, &bw);
			if(bw!=4) return 0;
			Exec_FillCMD_p(&buf[0]); // This one will byteswap automatically
		}

		
		Exec_WriteCMD(0, 0, W_SREG_BDM, LDRAddr[0], LDRAddr[1]); // Set PC to start of driver
		ShiftData(0);
		ShiftData_s(BDM_GO);

		latch = 0;
		do{ cntr = 0;
			if(!latch){                // We have some free time
				latch = 1;
				// while(ReadPin(P_FRZ))	;
				Lad     += 1024;
				if(!Lad) Had ++;   // Increment The high counter when the low one has overflowed to 0	
				
				if(Had < End){
					f_read(&Fil, &Fbuf, 600, &bw);
					if(bw!=600) return 0;
				}
				MiscTime = 2;
			}
			for(i=0; i<20; i++)
				if(ReadPin(P_RST) && !ReadPin(P_FRZ)) cntr ++;//MiscTime = 2;
		
		}while (/*MiscTime*/cntr);

		///< Read D0 / Check for errors
		Exec_ReadCMD(0, 0, R_DREG_BDM);
		if(bdmresp16 != 1) return 0;
		if(Had == End)     return 1;
	}while(1);
}


uint8_t Flash(const uint16_t *Bufst, const uint16_t *DrvStart, uint8_t End){

	BenchTime=65535;
	clrprintlcd("Ul");
	UploadDRV();

	SetPinDir(P_RST, 0);

	clrprintlcd("Init");
	if(!LDRDemand(3, &DrvStart[0], 0)) return 0;

	// Exec_ReadCMD(0, 0, R_DREG_BDM+7); // Read D7
	// ShowAddr(1, bdmresp16);

	clrprintlcd("Erase");
	if(!LDRDemand(2, &DrvStart[0], End)) return 0;

	clrprintlcd("Flash");
	if(!LDRWrite(&Bufst[0], &DrvStart[0], End)) return 0;
		
	clrprintlcd("OK");
	showval(65535 - BenchTime);
		
	return 1;
}


// 5388
// 5031
// 4108
// 3780
// 3775
// 3611
// 3234
// 3221
// 3202

// Quick routines for Jan; Dump binary.
void ShiftWait_p(const uint8_t *data){
	
	uint8_t temp;

	do{	PORTD &=~(1<<4 | 1<<1);     // Pull down Clock and Data out
		__asm("nop");
		if(!(PIND & _BV(0))) break; // Attention-bit not set, abort loop and fetch our valuable data.

		// Delays.. I hate them
		__asm("nop");
		__asm("nop");
		__asm("nop");

		// Enable SPI
		UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(1<<UCPHA0)|(1<<UCPOL0);
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
		// Clock out 0's
		UDR0 = 0;
		while(!(UCSR0A&(1<<RXC0)))	;
		temp = UDR0;

		UDR0 = 0;
		while(!(UCSR0A&(1<<RXC0)))	;
		temp = UDR0;

		// Disable SPI
		UCSR0C = 0;
		UCSR0B = 0;

		// Delays.. I hate them
		__asm("nop");
		__asm("nop");

	}while (1);

	// Enable SPI
	UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(1<<UCPHA0)|(1<<UCPOL0);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
		
	// Clock out 0's
	UDR0 = 0;
	while(!(UCSR0A&(1<<RXC0)))	;
	// Fetch data and increment pointer
	*(uint8_t *)data++=UDR0;

	// Repeat
	UDR0 = 0;
	while(!(UCSR0A&(1<<RXC0)))	;
	*(uint8_t *)data=UDR0;

	// Disable SPI
	UCSR0C = 0;
	UCSR0B = 0;

	// Kill warning without compromising speed
	return;
	temp = temp;
}

// Let's break every rule possible to see how fast this tub can move
inline void Exec_DumpCMD_p(const uint16_t *data){

	// Pull down clock and data out
	PORTD &=~(1<<4 | 1<<1);

	// Enable SPI
	UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(1<<UCPHA0)|(1<<UCPOL0);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	// Clock out dump command
	UDR0 = DUMP32_BDM>>8&0xFF;
	while(!(UCSR0A&(1<<RXC0)))	;
	uint8_t temp = UDR0;

	UDR0 = DUMP32_BDM&0xFF;
	while(!(UCSR0A&(1<<RXC0)))	;
	// temp = UDR0;
	
	// Disable SPI
	UCSR0C = 0;
	UCSR0B = 0;

	__asm("nop");
	
	ShiftWait_p((uint8_t *)&data[0]);
	ShiftWait_p((uint8_t *)&data[1]);

	return ;
	temp = temp;
}


uint8_t DumpFlash(uint16_t SizeK){

	BenchTime = 65535;

	uint16_t checksum16 = 0;
	uint8_t  cntr       = 0;

	SizeK <<= 2; // Multiply by four; We dump in chunks of 256 bytes

	if(f_open(&Fil, "New.bin", FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
		return 0;

	// Read four bytes.
	Exec_ReadCMD_workaround(0,0, READ32_BDM);
	*(uint16_t *) &Fbuf[cntr++] = bdmresp32<<8 | bdmresp32>>8;
	*(uint16_t *) &Fbuf[++cntr] = bdmresp16<<8 | bdmresp16>>8;

	// Increment by two for every copy to the byte-array.
	cntr += 2;

	do{ // Fill buffer with 256 bytes
		do{ Exec_DumpCMD_p((uint16_t *)&Fbuf[cntr]);
			cntr +=4;
		}while(cntr);
		
		do{ checksum16 += Fbuf[cntr++]; 
		}while(cntr);

		if(f_write(&Fil, &Fbuf, 256, &bw)!=FR_OK)
			return 0;

		ShowAddr(0, SizeK);
	}while(--SizeK);

	showval(65535 - BenchTime);
	ShowAddr(1, checksum16);
	return 1;
}