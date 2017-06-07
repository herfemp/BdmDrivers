#include <stdint.h>
#include "MC68332.h"

#define BDMSUCC __asm("move.l #1, %d0\n bgnd");
#define BDMFAIL __asm("move.l #0, %d0\n bgnd");

#define RamBuf    0x80000
#define RamBufLen 0x00404  /* Address (4 bytes) + 1024 bytes */

void Syscfg();
void swsr();               /* Service watchdog unconditionally */
void Delay(uint32_t del);  /* .. */
void DelaymS(uint32_t mS);
void Delay_6uS();
void Delay_10uS();
void FormatFlash();        /* Format selected partitions */
void WriteFlash();         /* Write flash */

/* 39SF *//*
#define FIVEFIVE = 0x5555<<1;
#define TWOA  = 0x2AAA<<1;*/

/* From flash.c */
/*	__asm(
	"moveq       #0, %d7 \n"
	"movea.l    %d7, %a0 \n"
	);*/
 
	/* Confirmed working on SST! */
/*	__asm(
    "movea.l #0xAAAA, %a2    \n"
    "move.w  #0xAAAA,(%a2)   \n"
    "move.w  #0x5555,(0x5554)\n"
    "move.w  #0x9090,(%a2)   \n"
	);*/
	
	/* Confirmed working on Intel/CSI! */
	/*__asm(
	"move.w #0xFFFF, %d0 \n"
	"move.w     %d0,(%a0)\n"
	"move.w     %d0,(%a0)\n"
	"move.w #0x9090,(%a0)\n" 
	);	*/
	/* Fetch ID, Same for all i've tested.. */
/*	__asm(	
    "move.w    (%a0), %d7 \n" 
	"addq        #2 , %a0 \n"
    "move.b    (%a0), %d7 \n"
	);*/

