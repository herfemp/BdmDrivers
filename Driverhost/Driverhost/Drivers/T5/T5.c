#include "../../common.h"
#include "../../BDM/BDM.h"
#include "../../BDM/regdef.h"
#include "T5.h"


/*
void findfault(){

	uint16_t i;
	
	for(i=0; i<0x800; i+=2){

		Exec_WriteCMD(0x10, i, WRITE16_BDM, 0, i);
		Exec_ReadCMD(0x10, i, READ16_BDM);
		ShowAddr(1, bdmresp16);

		if(bdmresp16 != i){
			sleep(2000);
			// lcd_clrscr();
			lcd_puts("missmatch!", 0);
			ShowAddr(0,i);
			while(1){};
		}
	}
}*/

uint8_t UploadDRV(){

	uint16_t Location = 0;
	uint16_t buf[2];
	Exec_WriteCMD( 0x10, 0x0400-4, WRITE32_BDM, 0, 0);

	do{ buf[0] = pgm_read_word(&T5Driver_bin[Location]);
		buf[1] = pgm_read_word(&T5Driver_bin[Location+2]);
		Exec_FillCMD_p(&buf[0]); // This one will byteswap automatically
		Location+=4;
	}while(Location<t5loaderbytes);

	// This is useless atm..
	return Location >= t5loaderbytes ? 1:0;
}





