#include "../common.h"
#include "../BDM/BDM.h"
#include "../BDM/regdef.h"


/* Commands:
 * 3: Init hardware
 * 2: Format flash
 * 1: Write data
 *
 * Stores result in D0
 * 1: OK
 * 0: Fail*/
uint8_t LDRDemand(uint8_t Command, const uint16_t *Addr){

	ShowAddr(1, 0);
	///< Store command in D0
	Exec_WriteCMD(0, 0, W_DREG_BDM, 0, Command);

	Exec_WriteCMD(0, 0, W_DREG_BDM+9, 4, 0); // T5 D1 = End addr

	// Set PC to start of driver
	Exec_WriteCMD(0, 0, W_SREG_BDM, Addr[0], Addr[1]);
	Exec_WriteCMD(0, 0, 0, 0, 0);
	ShiftData_s(BDM_GO);

	ShowAddr(2, 0);
	while(ReadPin(P_FRZ) )
		;
    ShowAddr(3, 0);
	MiscTime=3;
	do{ if(ReadPin(P_RST) && !ReadPin(P_FRZ)) MiscTime=3;
	}while (MiscTime);
	
	ShowAddr(4, 0);
	///< Read D0
	Exec_ReadCMD(0, 0, R_DREG_BDM);
	ShowAddr(5, 0);
	return bdmresp16&0xFF;
}


uint8_t Flash(const uint16_t *Bufst, const uint16_t *DrvStart){

	// if (ResetTarget() && StopTarget())
	{
		// PrepT();

		BenchTime=65535;
		clrprintlcd("Ul");
		UploadDRV();

		SetPinDir(P_RST, 0);

		clrprintlcd("Init");
		if(!LDRDemand(3, &DrvStart[0])) return 0;

		// Exec_ReadCMD(0, 0, R_DREG_BDM+7); // Read D7
		// ShowAddr(1, bdmresp16);

		clrprintlcd("Erase");
		if(!LDRDemand(2, &DrvStart[0])) return 0;

		clrprintlcd("Flash");
		if(!LDRWrite(&Bufst[0], &DrvStart[0])) return 0;
		
		lcd_puts("ok", 0);
		showval(65535 - BenchTime);
		
		return 1;
	}
	return 0;
}

// Write flash data
uint8_t LDRWrite(const uint16_t *Bufstart, const uint16_t *LDRAddr){

	uint8_t  Had = 0;
	uint16_t Lad = 0;
	uint8_t    i = 1;
	uint16_t buf[2];

	uint8_t Normal = Bufstart[1]&0xC;
	///< Store command in D0
	Exec_WriteCMD(0, 0, W_DREG_BDM, 0, 1);

	do{ Exec_WriteCMD(0, 0, W_DREG_BDM+8, Had, Lad); // Store addr in A0
		//if(Normal)
			Exec_WriteCMD(Bufstart[0], Bufstart[1], WRITE32_BDM, 0,0); // Ugly solution to start the fill command at the right address..
		//else
		//Exec_WriteCMD(Bufstart[0], Bufstart[1], WRITE32_BDM, Had,Lad); // Or store address on mcp 

		i--;
		do{ f_read(&Fil, &buf, 4, &bw);
			if(bw!=4) return 0;
			Exec_FillCMD_p(&buf[0]); // This one will byteswap automatically
			i++;
		}while(i);
		
		Exec_WriteCMD(0, 0, W_SREG_BDM, LDRAddr[0], LDRAddr[1]); // Set PC to start of driver
		Exec_WriteCMD(0, 0, 0, 0, 0);
		ShiftData_s(BDM_GO);
		
		MiscTime = 2;
		
		do{ if(!i){                // We have some free time, calculate crap
				while(ReadPin(P_FRZ))	;
				Lad += 1024;
				if(!Lad)     Had ++;   // Increment The high counter when the low one has overflowed to 0
				i++;
			}
			if(ReadPin(P_RST) && !ReadPin(P_FRZ)) MiscTime=2;
		}while (MiscTime);

		///< Read D0
		Exec_ReadCMD(0, 0, R_DREG_BDM);
		// Check for errors
		if(bdmresp16 != 1) return 0;
		if(Had == 4)       return 1;

	}while(1);
}