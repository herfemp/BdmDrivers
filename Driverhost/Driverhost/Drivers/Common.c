#include "../common.h"
#include "../BDM/BDM.h"
#include "../BDM/regdef.h"
#include "TxDriver.bin.h"
const uint16_t t5loaderbytes = sizeof(TxDriver_bin);


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
uint8_t LDRDemand(uint8_t cmd, const uint16_t *Addr, uint8_t End){

	Exec_WriteCMD(0, 0, W_DREG_BDM  ,   0, cmd); // Store command in D0
	Exec_WriteCMD(0, 0, W_AREG_BDM  ,   0,   0); // A0 = Start addr
	Exec_WriteCMD(0, 0, W_AREG_BDM+1, End,   0); // A1 = End addr

	// Set PC to start of driver
	Exec_WriteCMD(0, 0, W_SREG_BDM, Addr[0], Addr[1]);
	Exec_WriteCMD(0, 0, 0, 0, 0);
	ShiftData_s(BDM_GO);

	while(ReadPin(P_FRZ))  ;

	MiscTime=3;
	do{ if(ReadPin(P_RST) && !ReadPin(P_FRZ)) MiscTime=3;
	}while (MiscTime);
	
	///< Read D0
	Exec_ReadCMD(0, 0, R_DREG_BDM);
	return bdmresp16&0xFF;
}

// The UGLY version! It is faster though..
uint8_t Fbuf[600];

// Write flash data
uint8_t LDRWrite(const uint16_t *Bufstart, const uint16_t *LDRAddr, uint16_t SizeK){

	uint16_t   i;

	// Prefill buffer
	f_read(&Fil, &Fbuf, 600, &bw); // Read more than 512 bytes to force ff to read two sectors
	if(bw!=600) return 0;

	Exec_WriteCMD(0, 0, W_DREG_BDM, 0, 1); // Store command in D0
	Exec_WriteCMD(0, 0, W_AREG_BDM, 0, 0); // Store addr in A0

	do{ Exec_WriteCMD(Bufstart[0], Bufstart[1], WRITE32_BDM, 0,0); // Ugly solution to start the fill command at the right address..
	
		for(i=0; i<600; i+=4)
			Exec_FillCMD_p((uint16_t *)&Fbuf[i]); // This one will byteswap automatically

		f_read(&Fil, &Fbuf, 424, &bw);
		if(bw!=424) return 0;

		for(i=0; i<424; i+=4)
			Exec_FillCMD_p((uint16_t *)&Fbuf[i]); // This one will byteswap automatically

		Exec_WriteCMD(0, 0, W_SREG_BDM, LDRAddr[0], LDRAddr[1]); // Set PC to start of driver
		ShiftData(0);
		ShiftData_s(BDM_GO);
		
		if(SizeK > 1){
			f_read(&Fil, &Fbuf, 600, &bw);
			if(bw!=600) return 0;
		}

		// De-bounce
		for(i=8; i>0; i--)
			if(ReadPin(P_RST) && !ReadPin(P_FRZ) && i < 8) i +=2;

		///< Read D0 / Check for errors
		Exec_ReadCMD(0, 0, R_DREG_BDM);
		if(bdmresp16 != 1) return 0;

	}while(--SizeK);

	return 1;
}


uint8_t Flash(uint16_t SizeK){

	uint16_t Bufst[2] = {0x0F, 0xFFFC};
	uint16_t Drvst[2] = {0x10, 0x0400};
	
	BenchTime=65535;
	clrprintlcd("Ul");
	UploadDRV();

	SetPinDir(P_RST, 0);

	clrprintlcd("Init");
	if(!LDRDemand(3, &Drvst[0], 0)) return 0;

	// Exec_ReadCMD(0, 0, R_DREG_BDM+7); // Read D7
	// ShowAddr(1, bdmresp16);

	clrprintlcd("Erase");
	if(!LDRDemand(2, &Drvst[0], SizeK/64)) return 0;

	clrprintlcd("Flash");
	if(!LDRWrite(&Bufst[0], &Drvst[0], SizeK)) return 0;
		
	clrprintlcd("OK");

	showval(65535 - BenchTime);
		
	return 1;
}

#define nop3    __asm("nop\nnop\nnop");
#define nop2    __asm("nop\nnop");


// 5388
// 5031
// 4108
// 3780
// 3775
// 3611
// 3234
// 3221
// 3202
// 2942
// 2934
// Quick routines for Jan; Dump binary.
void ShiftWait_p(const uint8_t *data){

	do{	PORTD &=~(1<<4 | 1<<1);     // Pull down Clock and Data out

		__asm("nop");
		if(!(PIND & _BV(0))) break; // Attention-bit not set, abort loop and fetch our valuable data.
		
		EnSPI; // Enable SPI		
		nop3;  // Delays.. I hate them

		// Clock out 0's
		SPINull;
		uint8_t temp = UDR0;
		SPINull;
		temp = UDR0;
		temp = temp;

		UCSR0C = UCSR0B = 0; // Disable SPI
		nop2; // Delays.. I hate them

	}while (1);

	EnSPI; // Enable SPI
		
	// Clock out 0's and fetch both bytes
	SPINull;
	*(uint8_t *)data++=UDR0;	
	SPINull;
	*(uint8_t *)data  =UDR0;

	UCSR0C = UCSR0B = 0; // Disable SPI
}

// Let's break every rule possible to see how fast this tub can move
inline void Exec_DumpCMD_p(const uint8_t *data){

	// Pull down clock and data out
	PORTD &=~(1<<4 | 1<<1);

	// Enable SPI
	EnSPI;
	
	// Clock out dump command
	UDR0 = DUMP32_BDM>>8&0xFF;
	while(!(UCSR0A&(1<<RXC0)))	;
	uint8_t temp = UDR0;

	UDR0 = DUMP32_BDM&0xFF;
	while(!(UCSR0A&(1<<RXC0)))	;
	// temp = UDR0;
	
	// Disable SPI
	UCSR0C = UCSR0B = 0;
	
	__asm("nop");

	// We actually fetch words though we have to perform some tricks to circumvent hardware limitations
	ShiftWait_p(&data[0]);
	ShiftWait_p(&data[2]);

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
	checksum16 += Fbuf[0] + Fbuf[1] + Fbuf[2] + Fbuf[3];

	// Increment by two for every copy to the byte-array.
	cntr += 2;

	do{ // Fill buffer with 256 bytes
		do{ Exec_DumpCMD_p(&Fbuf[cntr]);
			checksum16 += Fbuf[cntr] + Fbuf[cntr+1] + Fbuf[cntr+2] + Fbuf[cntr+3];
			cntr +=4;
		}while(cntr);

		if(f_write(&Fil, &Fbuf, 256, &bw)!=FR_OK)
			return 0;

		// ShowAddr(0, SizeK);
	}while(--SizeK);

	showval(65535 - BenchTime);
	ShowAddr(1, checksum16);
	return 1;
}