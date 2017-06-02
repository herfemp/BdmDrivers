#include "common.h"
#include "HAL/HAL.h"

#include "BDM.h"
#include "HAL/hd44780/hd44780.h"
#include "HAL/Disk/ff.h"
#include "CPU2.h"

uint8_t msg1[20];
char ADDR_LCD[5]={0};

// Stupid function to decrease ram usage.. */
uint16_t Ldrw(uint16_t Location){

	uint16_t retval = pgm_read_word(&driver_bin[Location]);
	return ( (retval&0xFF) <<8 |  ((retval>>8)&0xFF)        );
}
// Stupid function number 2..
#define Byteswap(Input) ((Input&0xFF) <<8 |  ((Input>>8)&0xFF))
// And 3!
void clrprintlcd(const char *s){

	lcd_clrscr();
	lcd_puts(s, 0);
}


uint8_t halfbytetoascii(uint8_t ch){

	if((ch&0xF)>=0x0&&(ch&0xF)<=0x09) return ((ch&0xF)+0x30);
	else                              return ((ch&0xF)+0x37);
}


void ShowAddr(uint8_t Had, uint16_t Lad){

	ADDR_LCD[0]=halfbytetoascii(Had);
	ADDR_LCD[1]=halfbytetoascii(Lad>>12&0xF);
	ADDR_LCD[2]=halfbytetoascii(Lad>>8&0xF);
	ADDR_LCD[3]=halfbytetoascii(Lad>>4&0xF);
	ADDR_LCD[4]=halfbytetoascii(Lad&0xF);
	lcd_goto(5);
	lcd_puts(ADDR_LCD,5);
		
	LCDTMR=250;
}


void initsram_mcp(){
	
	Exec_WriteCMD(0xFF, 0xfb04, WRITE16_BDM,   0, 8); 
	Exec_WriteCMD(0xFF, 0xfb06, WRITE16_BDM,   0, 0);
	Exec_WriteCMD(0xFF, 0xfb00, WRITE16_BDM,   0, 0x800);
}


uint8_t UploadDRV(){

	uint16_t Location = 4;
	Exec_WriteCMD( LDRAddrH, LDRAddrL, WRITE32_BDM, Ldrw(0), Ldrw(2));

	do{ Exec_FillCMD( Ldrw(Location), Ldrw(Location+2));
		Location+=4;
	}while(Location<loaderbytes);

	return Location >= loaderbytes ? 1:0;
}


/* Commands:
 * 3: Init hardware
 * 2: Format flash
 * 1: Write data
 *
 * Stores result in D0
 * 1: OK
 * 0: Fail*/
uint8_t LDRDemand(uint8_t Command){

	///< _MUST_ be here.
	SetPinDir(P_RST, 0);

	///< Store command in D0
	Exec_WriteCMD(0, 0, W_DREG_BDM, 0, Command);

	// Set PC to start of driver
	Exec_WriteCMD(0, 0, W_SREG_BDM, LDRAddrH, LDRAddrL);
	Exec_WriteCMD(0, 0, 0, 0, 0);
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


uint8_t LDRWrite(){

	uint16_t Had = 0;
	uint16_t Lad = 0;
	uint16_t Len = BufLen / 4;
	uint16_t i;
	uint8_t  Inced;
	uint16_t buf[2];

	if(f_open( &Fil, "r11.bin", FA_OPEN_EXISTING | FA_READ ) == FR_OK){

		///< _MUST_ be here.
		SetPinDir(P_RST, 0);

		///< Store command in D0
		Exec_WriteCMD_s(0, 0, W_DREG_BDM, 0, 1);

		do{ Exec_WriteCMD(BUFAddrH, BUFAddrL, WRITE32_BDM, Had, Lad); //Store address in first buffer location
		
			for(i=0; i<Len; i++){

				f_read(&Fil, &buf, 4, &bw);
				if(bw!=4) return 0;
				Exec_FillCMD( Byteswap(buf[0]), Byteswap(buf[1]));
			}
			
			// Set PC to start of driver
			Exec_WriteCMD(0, 0, W_SREG_BDM, LDRAddrH, LDRAddrL);
			Exec_WriteCMD(0, 0, 0, 0, 0);
			ShiftData_s(BDM_GO);
	
			BenchTmr = 2;
			Inced    = 0;

			do{ if(!Inced){
					while(ReadPin(P_FRZ) )
						;

					Lad += 1024;
					if(!Lad)
						Had++;

					/* Shadow contains only 256 bytes so we can't use the full buffer (The loader is aware of this)*/
					if(Had == 4)
						Len = 64;

					Inced = 1;
				}
				if(ReadPin(P_RST) && !ReadPin(P_FRZ)) BenchTmr=2;
			}while (BenchTmr);
	
			///< Read D0
			Exec_ReadCMD(0, 0, R_DREG_BDM);
			// Check for errors
			if(bdmresp16 != 1)
				return 0;
					
			if(Had == 4 && Lad > 256)
				return 1;
			
		}while(1);
	}
	return 0;
}


uint8_t FlashMCP(){

	if (ResetTarget() && StopTarget()){
		
		PrepT();
		
		if(cpu2_testseq){
		
			Menutmr=65535;
			initsram_mcp();

			clrprintlcd("Prep..");
			
			if(!UploadDRV())
				return 0;
				
			if(!LDRDemand(3)) // Ask loader to configure everything
				return 0;
				
			if(!LDRDemand(2)) // Format flash
				return 0;
				
			clrprintlcd("Writing");
			if(!LDRWrite())
				return 0;
			
			f_close(&Fil);
			clrprintlcd("OK");
			benchtime = 65535 - Menutmr;
			return 1;
		}
	}
	return 0;
}


void bootstrapmcp(){

	PrepT();

	Exec_WriteCMD(0xFF, CMFIMCRAddr, WRITE16_BDM,   0, CMFIMCR_Stop);
	Exec_WriteCMD(0xFF, CMFIBAHAddr, WRITE16_BDM,   0, 0);
	Exec_WriteCMD(0xFF, CMFIBALAddr, WRITE16_BDM,   0, 0);
	Exec_WriteCMD(0xFF, CMFIMCRAddr, WRITE16_BDM,   0, CMFIMCR_Enable);

	///< Set PC to correct address.
	Exec_WriteCMD_s(0, 0, W_SREG_BDM, 0, 0x100);
	Exec_WriteCMD_s(0, 0, 0, 0, 0);
	ShiftData_s(BDM_GO);

	lcd_puts(".", 0);
}






