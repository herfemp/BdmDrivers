
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


void initsram_mcp(){
	///< Stop SRAM
	
	
	Exec_WriteCMD(0xFF, 0xfb04, WRITE16_BDM,   0, 8); 
	Exec_WriteCMD(0xFF, 0xfb06, WRITE16_BDM,   0, 0);
	Exec_WriteCMD(0xFF, 0xfb00, WRITE16_BDM,   0, 0x800);

	///< Start SRAM
	// Exec_WriteCMD(0xFF, FASRAM_Base, WRITE16_BDM,   0, FMCR);


}


// Stupid function to decrease ram usage.. */
uint16_t Ldrw(uint16_t Location){

	uint16_t retval = pgm_read_word(&driver_bin[Location]);
	return ( (retval&0xFF) <<8 |  ((retval>>8)&0xFF)        );
}
uint8_t UploadDRV(){

	lcd_puts("Drv", 0);	
	uint16_t Location = 4;
	Exec_WriteCMD( LDRAddrH, LDRAddrL, WRITE32_BDM, Ldrw(0), Ldrw(2));

	BenchTmr = 0; // Force lcd to update
	ShowAddr(LDRAddrH, Location);

	do{ Exec_FillCMD( Ldrw(Location), Ldrw(Location+2));
		Location+=4;
	}while(Location<loaderbytes);

	BenchTmr = 0; // Force lcd to update
	ShowAddr(LDRAddrH, Location);

	return Location >= loaderbytes ? 1:0;
}

/* Commands:
 * 3: Init hardware
 * 2: Format flash
 * 1: Write data
 *
 * Stores result in D0
 * 1: OK
 * 0: Fail
*/

uint8_t LDRDemand(uint8_t Command){

	///< _MUST_ be here.
	SetPinDir(P_RST, 0);

	///< Store command in D0
	Exec_WriteCMD_s(0, 0, W_DREG_BDM, 0, Command);
	// Set PC to start of driver
	Exec_WriteCMD_s(0, 0, W_SREG_BDM, LDRAddrH, LDRAddrL);

	Exec_WriteCMD_s(0, 0, 0, 0, 0);
	ShiftData_s(BDM_GO);
	while(ReadPin(P_FRZ) )
		;
			
	BenchTmr=6;
	do{ if(ReadPin(P_RST) && !ReadPin(P_FRZ)) BenchTmr=6;
	}while (BenchTmr);
		
	///< Read D0
	Exec_ReadCMD(0, 0, R_DREG_BDM);

	return bdmresp16&0xFF;
}


// Stupid function number 2.. */
uint16_t Byteswap(uint16_t Input){
	return ( (Input&0xFF) <<8 |  ((Input>>8)&0xFF)        );
}

uint8_t LDRWrite(){

	uint16_t Had = 0;
	uint16_t Lad = 0;
	uint16_t Len = BufLen;
	uint16_t i;

	uint16_t buf[2];

	if(f_open( &Fil, "r11.bin", FA_OPEN_EXISTING | FA_READ ) == FR_OK){
	
		BenchTmr = 0; // Force lcd to update
		ShowAddr(0, 0);


		///< _MUST_ be here.
		SetPinDir(P_RST, 0);

		///< Store command in D0
		Exec_WriteCMD_s(0, 0, W_DREG_BDM, 0, 1);

		do{ Exec_WriteCMD(BUFAddrH, BUFAddrL, WRITE32_BDM, Had, Lad); //Store address in first buffer location
		
			/* Shadow contains only 256 bytes so we can't use the full buffer (The loader is aware of this)*/
			if(Had == 4 && Lad == 0)
				Len = 256;

			for(i=0; i<Len; i+=4){
				f_read(&Fil, &buf, 4, &bw);

				if(bw!=4)
					return 0;

				Exec_FillCMD( Byteswap(buf[0]), Byteswap(buf[1]));
			}
			
			// Set PC to start of driver
			Exec_WriteCMD(0, 0, W_SREG_BDM, LDRAddrH, LDRAddrL);

			Exec_WriteCMD(0, 0, 0, 0, 0);
			ShiftData_s(BDM_GO);
			while(ReadPin(P_FRZ) )
				;
	
			BenchTmr=3;
			do{ if(ReadPin(P_RST) && !ReadPin(P_FRZ)) BenchTmr=3;
			}while (BenchTmr);
	
			///< Read D0
			Exec_ReadCMD(0, 0, R_DREG_BDM);
		
			// Check for errors
			if(bdmresp16 != 1)
				return 0;
			
			Lad += 1024;
			if(Lad == 0)
				Had++;
			
			ShowAddr(Had, Lad);
			if(Had == 4 && Lad > 256)
				return 1;
			
		}while(1);

	}

	return 0;
}









void FlashMCP(){




	if (ResetTarget() && StopTarget()){
		
		PrepT();
		

		/* Yes it is. */
		if(cpu2_testseq){

			sleep(50);
			
			lcd_clrscr();
			lcd_puts("Openf", 0);
			
			/* Check if file exist on card before starting */
			/*if( f_open( &Fil, "r1.1.bin", FA_OPEN_EXISTING | FA_READ ) == FR_OK)*/{
				lcd_clrscr();
				lcd_puts("Fopen", 0);

				
				Menutmr=65535;
				lcd_clrscr();
				initsram_mcp();

				if(UploadDRV() == 1){
					lcd_goto(0);
					lcd_puts("DrvOK", 0);
					/*Exec_ReadCMD(LDRAddrH, LDRAddrL, READ16_BDM);
					BenchTmr = 0;
					ShowAddr(3, bdmresp16);*/
				}

				lcd_goto(0);
				lcd_puts("Ini..", 0);
				if(LDRDemand(3)== 1){
					lcd_goto(0);
					lcd_puts("IniOK", 0);
				}

				lcd_goto(0);
				lcd_puts("For..", 0);
				if(LDRDemand(2) == 1){
					lcd_goto(0);
					lcd_puts("ForOK", 0);
				}
				
				lcd_goto(0);
				lcd_puts("Wri..", 0);
				if(LDRWrite() == 1){
					lcd_goto(0);
					lcd_puts("WriOK", 0);
				}





				benchtime = 65535 - Menutmr;
			}
		}
		f_close(&Fil);


		
		
		}else{
		//printf("fail!\n\r");
		while(1){}
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






