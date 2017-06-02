#include "inc/common.h"
 

 /* * * * * * * * * * * * * * * */
/* * *"Internal" functions * * */

uint16_t CMFIMCR_Enable = (

	0<<13 | 
	1<<12 | 
	1<<11 | 
	3<< 6 
/*	0= Minimum bus cycles = 3 clocks, 1 inserted wait states
	1 = Minimum bus cycles = 4 clocks, 2 inserted wait states
	2 = Minimum bus cycles = 5 clocks, 3 inserted wait states
	3 = Minimum bus cycles = 2 clocks, 0 inserted wait states*/
);



uint16_t CMFIMCR_Stop = (
    1<<15 | /* stop */
	0<<13 | /* SIE (Shadow information enable)*/
	1<<12 | /* boot*/
	1<<11 | /* lock (1=disable)*/
	3<< 6 
/*	0= Minimum bus cycles = 3 clocks, 1 inserted wait states
	1 = Minimum bus cycles = 4 clocks, 2 inserted wait states
	2 = Minimum bus cycles = 5 clocks, 3 inserted wait states
	3 = Minimum bus cycles = 2 clocks, 0 inserted wait states*/
);


/* I know, I know.. */
void Delay(uint32_t del){
	
	while (del--)
		swsr();

}


void Syscfg(){

	swsr();
	__asm(
	"move.w  #0x8000,(0xFFF200) /* Not needed when in bdm.. */\n"
	"move.b  #0,     (0xFFFA21) /* Disable watchdog; Fix!   */\n"
	"move.b  #0,     (0xFFFC07) /* Disable spi-ints         */\n"
	"move.w  #0x8000,(0xFFF400) /* Disable adc completely   */\n"
	"move.w  #0x8000,(0xFFFE00) /* Disable tpu completely   */\n"
	"move.b  #0,     (0xFFFA2D) /* Disable portf ints       */\n");	
	
	/* CPU @ 32 mhz (Significantly faster than stock value of 0xD084)*/
	*(uint16_t *) (SYNCR) = (1<<15|7<<12); /* X (0=Divide by 2), W (Multiplier), Y (Divider), EDIV, LOSCD */
	
	do{ swsr();
	}while( (*(uint16_t volatile *)(SYNCR))&0x8);
	
	/* Stop cmf, if started, and configure some stuff + address */
	*(uint16_t *) (CMFIMCR) = CMFIMCR_Stop;
	*(uint32_t *) (CMFIBAR) = 0;
	*(uint16_t *) (CMFIMCR) = CMFIMCR_Enable;
	
	/* Disable shadow */
	*(uint16_t *) (CMFIMCR) &= 0xDFFF;
		
	Delay(50000);
	BDMSUCC;
}


/* Verify written data */
uint8_t MargainRead(uint32_t Addr){

	uint32_t Start = Addr;
	uint32_t End   = Addr + 64;

	while(Start<End){
		if(*(uint16_t *) (Start) > 0)
			return 0;

		Start+=2;
	}

	return 1;
}

/* Used by the write routine, put in a separate routine to save space and unclutter that function */
uint8_t Writesec(uint32_t Addr){

	uint16_t Block;
	uint16_t times = 0;

	do{ Block = (( 1 << (Addr>>15) ) << 8);
		/* Enable high voltage (PEEM=1, B0EM=1, SES=1, EHV=1) */
		*(uint16_t *) (CMFICTL2) = (Block | 0x33);
		
		/* Wait for vpp to go down */
		do{ swsr();
		}while( ( ( *(uint16_t volatile *) (CMFICTL1) )&0x8000));

		/* Disable High voltage (PEEM=1, B0EM=1, SES=1, EHV=0) */
		*(uint16_t *) (CMFICTL2) = (Block | 0x32);
			
		/* Perform a light margin read first to save time */
		if((*(uint16_t *) (Addr) == 0) && (*(uint16_t *) (Addr + 0x20) == 0)){

			/* Perform full read */
			if(MargainRead(Addr) == 1)
				return 1;	
		}

	}while(times++ < 48000);
	
	return 0;
}




/* Program flash */
/* Todo: Remove that rwbuffer. Flash has its own, use that */
void WriteBuffer(){

	uint32_t Address  = *(uint32_t *)(RamBuf);
	uint32_t Buffpntr = RamBuf + 4;
	uint32_t Len      = RamBufLen;
	uint8_t  success  = 1;
	uint8_t  i;
	
	/* Regular access */
	if(Address < 0x40000){
		*(uint16_t *) (CMFIMCR) &= 0xDFFF;	
		
	/* Shadow access */
	}else{
		*(uint16_t *) (CMFIMCR) |= 0x2000;
		Len = 256;
		Address &= 0xFF;	
	}
	
	do{ /* Figure out which block to write-enable */
		uint16_t Block = ( 1 << (Address>>15) ) << 8;

		/* (PEEM=1, B0EM=1, SES=1, EHV=0) */
		*(uint16_t *)(CMFICTL2) =( Block | 0x32);
		
		/* It's safe to assume this will always be 32 atm (Each block is bound to a 64 byte boundary and we're writing words )*/ 
		uint8_t  ffblock = 32;
		
		/* Write data 
		 * It's NOT possible to ignore FF writes in case the whole block is not filled with them
		 * since they use a hardware buffer that could be filled with old data (Learned that the hard way lol)*/
		for(i=0; i<64; i+=2){
			
			if (*(uint16_t *) (Buffpntr) == 0xFFFF)
				ffblock--;
			
			*(uint16_t *) (Address + i) = *(uint16_t *) (Buffpntr);
			
			Buffpntr += 2;
		}
		
		/* skip the whole section if it's filled with ff's. (ffblock = 0)
		 * _Huge_ speedup since pulsing the flash takes quite a while */
		if(ffblock > 0)
			success = Writesec(Address);

		/* (PEEM=1, B0EM=1, SES=0, EHV=0) */
		*(uint16_t *)(CMFICTL2) =( Block | 0x30 );
		
		if(success == 0)
			BDMFAIL;

		Address += 64;

	}while(Buffpntr < (RamBuf + Len));
	
	BDMSUCC;
}


uint8_t Checkiferased(uint32_t start, uint32_t end){

	/* Make Eclipse happy */
	uint32_t i;

	for(i=start; i<end; i+=4){
		if((*(uint32_t *) (i))!=0xFFFFFFFF)
			return 0;
	}

	return 1;
}

/* Might not work or actually be needed.. */
void RebootFlash(){

	/* Stop flash */
	*(uint16_t *) (CMFIMCR) |= 0x8000;
	while(! ((*(uint16_t volatile *) (CMFIMCR))&0x8000))
		;

	/* Disable lock */
	*(uint16_t *) (CMFIMCR) |= 0x0800;

	*(uint16_t *) (CMFIMCR)  = 0x88C0;
	
	/* Start */
	*(uint16_t *) (CMFIMCR) &= 0x5FFF;
	while(((*(uint16_t volatile *) (CMFIMCR))&0x8000))
		;
}

void FormatFlash(){

	uint8_t i;
	uint16_t pm;
	uint8_t Formatmask = 0xFF;

	/* Tests: "Reboot" flash to disable write-locks if possible (Haven't seen a software that actually locks it but it shouldn't hurt to do it just in case) */
	RebootFlash();

	/* Disable shadow mode (if active) and reset addr to 0 */
	*(uint16_t *) (CMFIMCR) &= 0xDFFF;

	/* Should really use assembly for these but it's easier to tweak settings this way */
	*(uint16_t *)(CMFICTL1) = (
		4<<11 | /* sclkr (R)*/ /* 32 / _3_ = 10 Mhz */
		2<< 8 | /* clkpe (N)*/ /* 17 = 131072 */
		81		/* CLKPM (M)+ 1 */
	); /* 1007.616 mS */

	*(uint16_t *)(CMFITST) = 0;

	do{	pm = Formatmask << 8;

		/* Start ses */
		*(uint16_t *)(CMFICTL2) =( pm | 0x36 );

		Delay(16);

		/* Erase-interlock */
		*(uint16_t *)(0x10000) = 0;

		/* Trigger EHV */
		*(uint16_t *)(CMFICTL2) =( pm | 0x37 );

		/* Wait for vpp to go down */
		do{ swsr();
		}while( ( ( *(uint16_t volatile *) (CMFICTL1) )&0x8000));

		/* Negate EHV */
		*(uint16_t *)(CMFICTL2) =( pm | 0x36 );

		/* Add a tiny delay so the main function cant't attempt a verify-read too soon */
		Delay(16);

		for(i=0; i<8; i++){
			swsr();
			if((Formatmask>>i)&1){

				if(i>0)
					Formatmask -= (Checkiferased(i << 15, (i << 15)+0x8000)<<i);

				else{
					/* Partition 1 contains shadow row. check both */
					if(Checkiferased(0, 0x8000)){

						*(uint16_t *) (CMFIMCR) |= 0x2000;
						Delay(500);

						Formatmask -= Checkiferased(0, 256);

						*(uint16_t *) (CMFIMCR) &= 0xDFFF;
						Delay(500);
		}}}}

		*(uint16_t *)(CMFICTL2) = ( pm |0x34 );
		*(uint16_t *)(CMFICTL2) = ( pm |0x30 );

		Delay(500);

	}while((Formatmask>0));

	/* Return flash to normal operation */
	*(uint16_t *) (CMFIMCR) &= 0xDFFF;

	/* Pre-configure flash for programming */
	*(uint16_t *) (CMFICTL1) = (4<<11 | 0<< 8 | 16 );

	BDMSUCC;
}
