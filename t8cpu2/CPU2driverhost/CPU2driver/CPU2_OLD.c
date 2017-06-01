/*
 * CPU2.c
 *
 * Created: 2016-06-03 09:49:17
 *  Author: Dev
 */ 


 #include "common.h"
 #include "config.h"
 #include "HAL/HAL.h"

 #include "BDM.h"
 #include "HAL/hd44780/hd44780.h"
 #include "HAL/Disk/ff.h"
 #include "CPU2.h"
 
 uint8_t msg1[20];


 char ADDR_LCD[5]={0};

 uint8_t halfbytetoascii(uint8_t ch){

	 //l8=0;

	 if/***/((ch&0xF)>=0x0&&(ch&0xF)<=0x09) ///< Numbers
	 return ((ch&0xF)+0x30);

	 else/*if ((ch&0xF)>=0xA&&(ch&0xF)<=0xF)*/ ///< Uppercase
	 return ((ch&0xF)+0x37);
	 //else return 0;
 }




void ShowAddr(uint8_t Had, uint16_t Lad){
	if (!BenchTmr) {
		ADDR_LCD[0]=halfbytetoascii(Had);
		ADDR_LCD[1]=halfbytetoascii(Lad>>12&0xF);
		ADDR_LCD[2]=halfbytetoascii(Lad>>8&0xF);
		ADDR_LCD[3]=halfbytetoascii(Lad>>4&0xF);
		ADDR_LCD[4]=halfbytetoascii(Lad&0xF);
		lcd_goto(5);
		lcd_puts(ADDR_LCD,5);
		 
		BenchTmr=500;
}}


uint8_t programmargincheck(uint8_t Hd, uint16_t Ld){
	///< Reset verification counter..
	uint8_t Verif=16;
	uint8_t Loop;
	
	/* Read first four bytes and supply a start-address */
	Exec_ReadCMD(Hd, Ld, READ32_BDM);

	/* Check if all those bytes reads as 0 (0=successfully written) */
	if(!bdmresp32&&!bdmresp16) Verif--;
	
	/* Let the ecu auto-increment the address counter and check the rest */
	for (Loop=0; Loop<15; Loop++){
		Exec_DumpCMD();
		if(!bdmresp32&&!bdmresp16) Verif--;
	}

	return Verif;

}
uint8_t NonFF;
 void fillbufferwith64bytes(){
	uint8_t read;
	NonFF=32;
	///< Read 64 bytes
	for (read=0; read<32; read++){
		f_read(&Fil, &msg1, 2, &bw);
		if(bw!=2) while (1){ ; }
		blockbuf[read]=( msg1[1]<<8 | msg1[0] );

		/* Count number of 0xFFFF to make it possible to skip empty regions */
		if(blockbuf[read]==0xFFFF) NonFF--;
	}
 }

 inline void SendPGMPulse(uint8_t Block){

	 ///< Enable high voltage (PEEM=1, B0EM=1, SES=1, EHV=1)
	 Exec_WriteCMD(0xFF, CMFICTL2Addr, WRITE16_BDM,   0, (1<<Block| 0x33));
	 
	 ///< loop until vpp goes down
	 do{ Exec_ReadCMD(0xFF, CMFICTL1Addr, READ16_BDM); }while (bdmresp16&0x8000);
	 
	 ///< Disable High voltage (PEEM=1, B0EM=1, SES=1, EHV=0)
	 Exec_WriteCMD(0xFF, CMFICTL2Addr, WRITE16_BDM,   0, (1<<Block| 0x32));

 }


/* Write shadow region in block 0 */
void InitWrite_shadow(uint8_t Hd){
	
	uint16_t Ld=0;
	uint16_t Count=0;
	uint8_t Block=8;

	uint8_t i;
	//uint16_t rets=0;

	lcd_clrscr();
	lcd_puts("Shdw", 0);
	
	Exec_WriteCMD(0xFF, CMFIMCRAddr, WRITE16_BDM,   0, CMFIMCR_Enable_shadow);
	sleep(500); // (ms)

	
	for(Count=0; Count<4; Count++){
		
		/* Fill buffer with 64 bytes */
		fillbufferwith64bytes();

		// Skip if the whole region is filled with FF's
		if(NonFF){
			
			///< Prepare for flash (PEEM=1, B0EM=1, SES=1, EHV=0)
			Exec_WriteCMD(0xFF, CMFICTL2Addr, WRITE16_BDM,  0, (1<<Block|0x32));
		
		
			///< Upload 64 bytes to ecu's buffer. 
			Exec_WriteCMD(Hd,  Ld, WRITE32_BDM, blockbuf[0], blockbuf[1]);
			for(i=0; i<30; i+=2)
				Exec_FillCMD(blockbuf[i+2], blockbuf[i+3]);
		
		
			do{ SendPGMPulse(Block);
				/* Break free of this loop if all blocks verifies as written (0) */
				if(!programmargincheck(Hd, Ld)) break;
			} while (1);

			/* Could probably skip this.. (PEEM=1, B0EM=1, SES=0, EHV=0)*/
			Exec_WriteCMD(0xFF, CMFICTL2Addr, WRITE16_BDM,   0, (1<<Block|0x30));
		}
		ShowAddr(Hd, Ld);
		if(Ld<0xFFC0)Ld+=0x40;
		else { Hd++; Ld=0; }
	}
}


//18846
 /* Write normal flash */
 void InitWrite(uint8_t Hd){
	 
	 uint16_t Ld=0;
	 uint16_t Count=0;
	 uint8_t Block=0;
	 uint8_t i;
	 //uint16_t rets=0;
	 
	 lcd_clrscr();
	 lcd_puts("Wr", 0);


	 /* These two are _NOT_ stock recommended settings, they just work better in bdm-mode */
	 Exec_WriteCMD(0xFF, CMFITSTAddr, WRITE16_BDM,  0,
		1<<11 | // NVR
		7<< 8 | // PAWS
		0<< 6 | // STE
		1<< 5   // GDB
	 );

	 Exec_WriteCMD(0xFF, CMFICTL1Addr, WRITE16_BDM,   0, ( 7<<11 | 3<< 8 | 127)); // sclkr, clkpe, clkpm
	 

	 /* Keep track of which block to enable */
	 /* Block 8 = real block 0 */
	 /* Each block contains 32K of flash, or 512 64-byte transfers */
	 for(Block=8; Block<16; Block++){
		 
		 for(Count=0; Count<512; Count++){

		 	/* Fill buffer with 64 bytes */
		 	fillbufferwith64bytes();

			// Skip if the whole region is filled with FF's
			if(NonFF){

				//rets=0;
				///< Prepare for flash (PEEM=1, B0EM=1, SES=1, EHV=0)
				Exec_WriteCMD(0xFF, CMFICTL2Addr, WRITE16_BDM,  0, (1<<Block|0x32));
			 
				/* Specify address in the first transfer and then use the fill function */
				Exec_WriteCMD_workaround(Hd,  Ld, WRITE32_BDM, blockbuf[0], blockbuf[1]);

				for(i=0; i<30; i+=2)
					Exec_FillCMD(blockbuf[i+2], blockbuf[i+3]);
				
				do{ SendPGMPulse(Block);
					
					// Break free of this loop if all blocks verifies as written (0) 
					if(!programmargincheck(Hd, Ld)) break;


					/* If we got here, this attempt was not successful. Increment try-counter and let this loop start over as long as we haven't reached the limit */
					/*if(rets<65535)*/ //rets++;
					/* else (add mechanism to notify user about a problem) */
				} while (1);
			 
				/* Could probably skip this.. (PEEM=1, B0EM=1, SES=0, EHV=0)*/
				Exec_WriteCMD(0xFF, CMFICTL2Addr, WRITE16_BDM,   0, (1<<Block|0x30));
			}

			 //ShowAddr(Hd, Ld);
			 
			 if(Ld==0xFFC0) Hd++;
			 Ld+=0x40; /* Let it roll over */
			 //if(Ld<0xFFC0)Ld+=0x40;
			 //else { Hd++; Ld=0; }
			
		 };
	 };


	 f_close(&Fil);

	 /* Write shadow-info */
	 if( f_open( &Fil, "sie.bin", FA_OPEN_EXISTING | FA_READ ) == FR_OK) InitWrite_shadow(0);
 }


 
 

 void EraseFlash(uint8_t start){

	lcd_clrscr();
	lcd_puts("Erase", 0);

	 
	//printf("-Erasing-\n\r");
	Exec_WriteCMD(0xFF, CMFICTL1Addr, WRITE16_BDM,   0, 
		4<<11 | /* sclkr (R)*/ /* 32 / _3_ = 10 Mhz */
		1<< 8 | /* clkpe (N)*/ /* 16 = 65536 */
		16		/* CLKPM (M)*/
	); /* 101,2~ ms pulse */
	 
	Exec_WriteCMD(0xFF, CMFITSTAddr, WRITE16_BDM,  0,
		1<<11 | // NVR
		7<< 8 | // PAWS
		0<< 6 | // STE
		1<< 5   // GDB
	);
	 
	 
	 Exec_WriteCMD(0xFF, CMFICTL2Addr, WRITE16_BDM,   0, 0xFF36);
	 Exec_WriteCMD(start, 0, WRITE16_BDM,  0, 0);
	 Exec_WriteCMD(0xFF, CMFICTL2Addr, WRITE16_BDM,   0, 0xFF37);
	 
	 
	 

	 do{ Exec_ReadCMD(0xFF, CMFICTL1Addr, READ16_BDM);
		 if(!SleepTMR){  SleepTMR=250; }
	 }while (bdmresp16&0x8000);

	 
	 Exec_WriteCMD(0xFF, CMFICTL2Addr, WRITE16_BDM,   0, 0xFF36);
	 

	 
	 Exec_WriteCMD(0xFF, CMFICTL2Addr, WRITE16_BDM,   0, 0xFF34);
	 sleep(100);
	 Exec_WriteCMD(0xFF, CMFICTL2Addr, WRITE16_BDM,   0, 0xFF30); /// ehv low ..
 }





 /* Stupid naming, I know. -Lazy */
 void FlashMCP(){

	uint8_t StartH=0;
	


	
	if (ResetTarget() && StopTarget()){
		
		PrepT(); 
		

		/* Yes it is. */
		if(cpu2_testseq){
			///< Enable write access, stop flash
			Exec_WriteCMD(0xFF, CMFIMCRAddr, WRITE16_BDM,   0, CMFIMCR_Stop);
		
			///< Relocate flash address @ 0x40000 (T5 anyone?)
			Exec_WriteCMD(0xFF, CMFIBAHAddr, WRITE16_BDM,   0, ((StartH/4)<<2)); 
			Exec_WriteCMD(0xFF, CMFIBALAddr, WRITE16_BDM,   0, 0); ///< Should ALWAYS be 0 on 256k parts
		
			///< Enable write acc, start flash
			Exec_WriteCMD(0xFF, CMFIMCRAddr, WRITE16_BDM,   0, CMFIMCR_Enable);
		
			sleep(50);
			
			lcd_clrscr();
			lcd_puts("Openf", 0);
			
			/* Check if file exist on card before starting */
			if( f_open( &Fil, "cpu2.bin", FA_OPEN_EXISTING | FA_READ ) == FR_OK){
				lcd_clrscr();
				lcd_puts("Fopen", 0);

				Menutmr=65535;
				EraseFlash(StartH);
				InitWrite(StartH);
				benchtime = 65535 - Menutmr;
						
			} 
			
			
		}
		f_close(&Fil);


		
	
	}else{
		//printf("fail!\n\r");
		while(1){}
	}




	if (ResetTarget() && StopTarget()){
	
		lcd_clrscr();
		lcd_puts("Running?", 0);
		bootstrapmcp();
	}
	
}


void bootstrapmcp(){



	PrepT();
	Exec_WriteCMD(0xFF, CMFIMCRAddr, WRITE16_BDM,   0, CMFIMCR_Stop);


	Exec_WriteCMD(0xFF, CMFIBAHAddr, WRITE16_BDM,   0, 0);
	Exec_WriteCMD(0xFF, CMFIBALAddr, WRITE16_BDM,   0, 0);

	///< Enable write acc, start flash
	Exec_WriteCMD(0xFF, CMFIMCRAddr, WRITE16_BDM,   0, CMFIMCR_Enable);


	///< Set PC to correct address.
	Exec_WriteCMD_s(0, 0, W_SREG_BDM, 0, 0x100);
	Exec_WriteCMD_s(0, 0, 0, 0, 0);
	ShiftData_s(BDM_GO);

	lcd_puts(".", 0);

}