
#include "avr/pgmspace.h"
#include "driver.bin.h"
uint16_t loaderbytes = sizeof(driver_bin);

#define LDRAddrH 0x0008/* Where in ram to upload the driver    */
#define LDRAddrL 0x1C00
#define BUFAddrH 0x0008 /* Where to store address to write and data to be written */
#define BUFAddrL 0x0000

#define BufLen	1024
#define LDRSucc 1
#define LDRFail 0


 
 uint16_t CMFIMCR_Enable_shadow = (
//	0<<15 | // stop
//	0<<14 | // protect
	1<<13 | // SIE (Shadow information enable)
	0<<12 | // _boot_
	1<<11 | // _lock_ (1=disable)
	0<<10 | // Emul: emulated operation
//	0<< 8 | // ASPC 0 unrestricted data , 1 unrestricted program, 2 supervisor data, 3 supervisor program (Not allowed to change this from 0 -why?)
	3<< 6 
	/*
	0 = Minimum bus cycles = 3 clocks, 1 inserted wait states
	1 = Minimum bus cycles = 4 clocks, 2 inserted wait states
	2 = Minimum bus cycles = 5 clocks, 3 inserted wait states
	3 = Minimum bus cycles = 2 clocks, 0 inserted wait states*/
);

uint16_t CMFIMCR_Enable = (
//	0<<15 | // stop
//	0<<14 | // protect
	0<<13 | // SIE (Shadow information enable)
	0<<12 | // _boot_
	1<<11 | // _lock_ (1=disable)
	0<<10 | // Emul: emulated operation
	3<< 6 
	/*
	0 = Minimum bus cycles = 3 clocks, 1 inserted wait states
	1 = Minimum bus cycles = 4 clocks, 2 inserted wait states
	2 = Minimum bus cycles = 5 clocks, 3 inserted wait states
	3 = Minimum bus cycles = 2 clocks, 0 inserted wait states*/
);

uint16_t CMFIMCR_Stop = (
    1<<15 | // stop
//	0<<14 | // protect
	0<<13 | // SIE (Shadow information enable)
	0<<12 | // _boot_
	1<<11 | // _lock_ (1=disable)
	0<<10 | // Emul: emulated operation
//	0<< 8 | // ASPC 0 unrestricted data , 1 unrestricted program, 2 supervisor data, 3 supervisor program (Not allowed to change this from 0 -why?)
	3<< 6 
	
	//0 = Minimum bus cycles = 3 clocks, 1 inserted wait states
	//1 = Minimum bus cycles = 4 clocks, 2 inserted wait states
	//2 = Minimum bus cycles = 5 clocks, 3 inserted wait states
	//3 = Minimum bus cycles = 2 clocks, 0 inserted wait states
);







