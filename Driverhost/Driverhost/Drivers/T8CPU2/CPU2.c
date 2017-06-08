#include "../../common.h"
#include "../../BDM/BDM.h"
#include "../../BDM/regdef.h"
#include "CPU2.h"


uint8_t UploadDRVMCP(){

	uint16_t Location = 0;
	uint16_t buf[2];
	Exec_WriteCMD( LDRAddrH, LDRAddrL-4, WRITE32_BDM, 0, 0);

	do{ buf[0] = pgm_read_word(&driver_bin[Location]);
		buf[1] = pgm_read_word(&driver_bin[Location+2]);
		Exec_FillCMD_p(&buf[0]); // This one will byteswap automatically
		Location+=4;
	}while(Location<mcploaderbytes);

	// This is useless atm..
	return Location >= mcploaderbytes ? 1:0;
}


// Write flash data
uint8_t LDRWriteMCP(){

	uint16_t Had = 0;
	uint16_t Lad = 0;
	uint16_t Len = BufLen / 4;
	uint16_t i;
	uint8_t  Latch;
	uint16_t buf[2];

	if(f_open( &Fil, "r11.bin", FA_OPEN_EXISTING | FA_READ ) == FR_OK){

		///< _MUST_ be here.
		SetPinDir(P_RST, 0);

		///< Store command in D0
		Exec_WriteCMD_s(0, 0, W_DREG_BDM, 0, 1);

		do{ Exec_WriteCMD(BUFAddrH, BUFAddrL, WRITE32_BDM, Had, Lad); // Store address in first buffer location
			
			for(i=0; i<Len; i++){

				f_read(&Fil, &buf, 4, &bw);
				if(bw!=4) return 0;
				Exec_FillCMD_p(&buf[0]); // This one will byteswap automatically
			}
			
			Exec_WriteCMD(0, 0, W_SREG_BDM, LDRAddrH, LDRAddrL); // Set PC to start of driver
			Exec_WriteCMD(0, 0, 0, 0, 0);
			ShiftData_s(BDM_GO);
	
			MiscTime = 2;
			Latch    = 0;

			do{ if(!Latch){                // We have some free time, calculate crap
					while(ReadPin(P_FRZ))	;

					Lad += 1024;
					if(!Lad)     Had ++;   // Increment The high counter when the low one has overflowed to 0
					if(Had == 4) Len = 64; // Shadow-region; decrease size
					Latch = 1;
				}

				if(ReadPin(P_RST) && !ReadPin(P_FRZ)) MiscTime=2;
			}while (MiscTime);
	
			///< Read D0
			Exec_ReadCMD(0, 0, R_DREG_BDM);

			// Check for errors
			if(bdmresp16 != 1)          return 0;
			if(Had == 4 && Lad > 256)   return 1;
			
		}while(1);
	}
	return 0;
}


uint8_t FlashMCP(){

		
	uint16_t DrvStart[2] = {LDRAddrH, LDRAddrL-4};
	clrprintlcd("Prep..");
	BenchTime=65535;
		
	if(!UploadDRVMCP())
		return 0;
				
	if(!LDRDemand(3,&DrvStart[0], 0)) // Ask loader to configure everything
		return 0;
				
	if(!LDRDemand(2,&DrvStart[0], 0)) // Format flash
		return 0;
				
	clrprintlcd("Writing");
	if(!LDRWriteMCP())
		return 0;
			
	clrprintlcd("OK");
	showval(65535 - BenchTime);	
	return 1;
}





