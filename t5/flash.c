#include "common.h"
 
#define FIVEFIVE 0x5555<<1
#define TWOA     0x2AAA<<1

/* Store id of flash in D7 */
/*  Requires VPP:
 * 
 *  0x31B4 - 31: CAT, B4: 28F010 (ONsemi/CSI) 
 *  0x89B4 - 89:  TN, B4: 28F010 (Intel)
 *  
 *
 *
 *
 *  Low voltage parts:
 *
 *  0xBFB6 - BF: SST, B5: 39SF010 | B6: 39SF020 | B7: 39SF040
 *  0x0120 --AMD 29f010??
 *  0x0022 - 00: AMD, 22: 29f400 / 0x2223: This should be read as a word @ address 2. Current implementation fails to ID
 *
 *
 */
 
/* This routine is royally stupid! */
__asm(
".global Syscfg\n"
"Syscfg:                 \n"
"moveq     #0x40, %d1    \n" /* Wait 64~ ms; Also used for H/V on T5 */
"moveq        #0, %d6    \n" /* Presume new routine. Dangerous as F. I have no control of which registers C use */
"moveq        #0, %d7    \n"
"movea.l     %d7, %a0    \n"
"movea.l #0xFFFC14, %a1  \n"
"movea.l #0xFFFC17, %a2  \n"
/*   Experimental part   */
"move.w      %d7,(%a1)   \n" /* Negate high-voltage */
"move.b      %d7,(%a2)   \n"
"move.w  #0xF0F0,(%a0)   \n" /* Reset flash */
"move.l      %d1,-(%sp)  \n"
"bsr DelaymS             \n" /* Do not restore sp. It will reset itself soon enough */
/* * * * * * * * * * * * */
"move.w    (%a0), %d0    \n" /* Store a copy of address 0 */
"movea.l #0xAAAA, %a3    \n" /* Run ID-command for low-voltage flash */
"move.w  #0xAAAA,(%a3)   \n"
"move.w  #0x5555,(0x5554)\n"
"move.w  #0x9090,(%a3)   \n"
"move.w    (%a0), %d7    \n" /* Make another copy of address 0 */
"cmp.w       %d0, %d7    \n" /* Identical? No: We have our id. Yes: Continue */
"bne.s IDfound           \n"
"moveq        #1, %d6    \n" /* Indicate Old routine. Dangerous as F. I have no control of which registers C use */
"move.w      %d1,(%a1)   \n" /* Latch up high-voltage */
"move.b      %d1,(%a2)   \n"
"move.l      %d1, -(%sp) \n"
"bsr DelaymS             \n"
"move.w  #0xFFFF,(%a0)   \n" /* Reset flash */
"move.w      %d0,(%a0)   \n"
"move.w  #0x9090,(%a0)   \n" /* Send ID-command */
"move.w    (%a0), %d7    \n" /* Make a copy of the lower part */
"IDfound:                \n"
"addq         #2, %a0    \n" /* Copy the higher part from one of the flash memories */
"move.b    (%a0), %d7    \n" /* -Obviosly dangerous if the user decided to populate 2 different parts.. */
"move.l      %d1, -(%sp) \n"
"bsr DelaymS             \n"
"moveq        #1, %d0    \n"
"bgnd                    \n"
);


/* Is it really worth checking the error bit..? */
__asm(
".global FormatFlashNEW      \n"
"FormatFlashNEW:             \n"
"       move.w #0xF0F0,(0).w \n" /* Reset Flash */
"       pea (100).w          \n"
"       jsr DelaymS          \n"
"       addq.l #4,%sp        \n"
/* A0 and A1 is start and end */
"       movea.l #0xAAAA,%a2  \n"
"       movea.w #0x5554,%a3  \n"
"FormatLoopn:                \n"
"       cmpi.w  #0xFFFF,(%a0)\n"
"       beq.s   DataIdentn   \n"
"       move.w      %a2,(%a2)\n"
"       move.w  #0x5555,(%a3)\n"
"       move.w  #0x8080,(%a2)\n"
"       move.w      %a2,(%a2)\n"
"       move.w  #0x5555,(%a3)\n"
"       move.w  #0x1010,(%a2)\n"
"                            \n"
"ToggleWaitn:                \n" /* Add check for amd! Mask off error status in the first reads and then read again!? */
"       move.w  (%a0),%d0    \n"
"       move.w  (%a0),%d1    \n"
"       cmp.w   %d1,%d0      \n"
"       bne.s   ToggleWaitn  \n"
/*"     bne.s   Checkdq5f    \n"*/
"       bra.s   LoopCheckn   \n"
/* Fix:                      */
/*"Checkdq5f:                \n" 
"       andi.w  #0x2020, %d1 \n"
"       beq.s ToggleWaitn    \n"
"       move.w  (%a0),%d0    \n"
"       move.w  (%a0),%d1    \n"
"       cmp.w   %d1,%d0      \n"
"       beq.s   LoopCheckn   \n"
"       moveq   #0,%d0       \n"  Fff... 
"       bgnd                 \n"*/
"                            \n"
"DataIdentn:                 \n"
"       addq.l  #2,%a0       \n"
"                            \n"
"LoopCheckn:                 \n"
"       cmpa.l  %a1,%a0      \n"
"       bls.s   FormatLoopn  \n"
"       moveq   #1,%d0       \n"
"       bgnd                 \n" /* This is how you make IDA pro go apesh!t. "No rts; EVERYTHING is one huge function */
);

/* A0 and A1 is start and end */
__asm(
".global FormatFlashOLD      \n"
"FormatFlashOLD:             \n"
"       movea.l	#0     , %a2 \n" /* Reset Flash */
"       move.w	#0xFFFF, %d2 \n"
"       move.w	%d2    ,(%a2)\n"
"       move.w	%d2    ,(%a2)\n"
/*      Zero flash           */
"       movea.l	%a0    , %a2 \n" /* Make a copy of start addr */
"OOResTries:                 \n"
"       moveq	#25    , %d0 \n"
"OOloop:                     \n"
"       move.w	(%a2)  , %d1 \n"
" beq.s OOisOO               \n"
"       move.w	#0x4040,(%a2)\n"
"       clr.w	(%a2)        \n"
" jsr   Delay_10uS           \n"
"       move.w	#0xC0C0,(%a2)\n"
" jsr   Delay_6uS            \n"
"       move.w	(%a2)  , %d1 \n"
" beq.s OOEnRead             \n"
"       subq.b	#1     , %d0 \n"
" bra.s OOloopcheck          \n"
"OOEnRead:                   \n"
"       clr.w	(%a2)        \n"
" bra.s OOloopcheck          \n"
"OOisOO:                     \n"
"       addq.l  #2     , %a2 \n"
"       cmpa.l  %a1    , %a2 \n"
" bls.s OOResTries           \n"
" bra.s LeavOO               \n"
"OOloopcheck:                \n"
"       tst.b   %d0          \n"
" bne.s OOloop               \n"
" bra.w FormatFail           \n"
"LeavOO:                     \n"
/*      Format flash         */
"       move.w	#1000  , %d0 \n"  /* Maximum number of tries */
"FFloop:                     \n"
"       cmpi.w	#0xFFFF,(%a0)\n"
" beq.s DataisFF             \n"
"       move.w	#0x2020,(%a0)\n"
"       move.w	#0x2020,(%a0)\n"
"       pea     (10).w       \n"
" bsr.w DelaymS              \n"
"       addq.l  #4     , %sp \n"
"       move.w  #0xA0A0,(%a0)\n"
" bsr.w Delay_6uS            \n"
"       cmpi.w  #0xFFFF,(%a0)\n"
" beq.s FFenRead             \n"
"       subq.w  #1     , %d0 \n"
" bra.s FFloopChck           \n"
"FFenRead:                   \n"
"       clr.w   (%a0)        \n"
" bra.s FFloop               \n"
"DataisFF:                   \n"
"       addq.l	#2     , %a0 \n"
"FFloopChck:                 \n"
"       cmpa.l	%a1    , %a0 \n"
" bhi.s EndCheck             \n"
"       tst.w	%d0          \n"
" bne.s FFloop               \n"
"EndCheck:                   \n"
"       tst.w	%d0          \n"
" bne.s FormatSucc           \n"
"FormatFail:                 \n"
"       bgnd                 \n"
"FormatSucc:                 \n"
"       moveq	#1     ,%d0  \n"
"       bgnd                 \n"
);		
		
		
__asm(
".global WriteBufferOLD       \n"
"WriteBufferOLD:              \n"
"ResTries:                    \n"
"       moveq   #25    , %d0  \n" /* Tries */
"WriteLoop:                   \n"
"       move.w  (%a1)  , %d1  \n" /* Move data of buffer */
"       cmp.w   (%a0)  , %d1  \n" /* Compare it against flash location */
"       beq     dataident     \n" /* Identical */
"       move.w  #0x4040,(%a0) \n"
"       move.w  (%a1)  ,(%a0) \n"
"       bsr	Delay_10uS        \n" /* 10 uS */
"       move.w  #0xC0C0,(%a0) \n" /* Write compare command */
"       bsr Delay_6uS         \n"
"       cmp.w   (%a0)  , %d1  \n" /* Compare actual data */
"       beq.s   ReadMode      \n"
"       subq.b  #1     , %d0  \n" /* Decrement tries if we got here */
"       bra.s   loopcheck     \n"
"ReadMode:                    \n"
"       clr.w   (%a0)         \n"
"loopcheck:                   \n"
"       tst.b   %d0           \n"
"       bne.s   WriteLoop     \n"
"       bgnd                  \n"
"dataident:                   \n"	
"       addq.l  #2     , %a0  \n"
"       addq.l  #2     , %a1  \n"
"       cmpa.l  #0x1003FF,%a1 \n"
"       bls.s   ResTries      \n"
"       moveq   #1     , %d0  \n"
"       bgnd                  \n"
);

/* Is it really worth checking the error bit..? */
__asm(
".global WriteBufferNEW        \n"
"WriteBufferNEW:               \n"
"		movea.l	  #0xAAAA, %a2 \n"
"		movea.l	  #0x5554, %a3 \n"
"       move.l    #0xAAAA, %d2 \n"
"       move.l    #0x5555, %d3 \n"
"       move.l    #0xA0A0, %d4 \n"
"ss39sfmloop:			       \n"	
"		move.w	    (%a1), %d0 \n"
"		cmp.w	    (%a0), %d0 \n"
"		beq.s ss39sfident      \n"
"		move.w        %d2,(%a2)\n"
"		move.w        %d3,(%a3)\n"
"		move.w        %d4,(%a2)\n"
"		move.w      (%a1),(%a0)\n"
"ss39sfwait:                   \n"	
"		move.w      (%a0), %d0 \n"
"		move.w      (%a0), %d1 \n"
"		cmp.w         %d1, %d0 \n"
"       bne.s ss39sfwait       \n"
/*"     bne.s Checkdq5w        \n"*/
"		bra.s ss39sfdone       \n"
/* Fix:                        */
/*"Checkdq5w:                  \n"
"       andi.w    #0x0020, %d1 \n"
"       beq.s ss39sfwait       \n"
"       move.w      (%a0), %d0 \n"
"       move.w      (%a0), %d1 \n"
"       cmp.w         %d1, %d0 \n"
"       beq.s ss39sfdone       \n"
"       andi.w    #0xFF00, %d0 \n" 
"       cmp.w     #0xFF00, %d0 \n"
"       beq.s ss39sfdone       \n"
"       moveq          #0, %d0 \n"  Fff... 
"       bgnd                   \n" */
"                              \n"
"ss39sfident:                  \n"
"		addq.l         #2, %a0 \n"
"		addq.l         #2, %a1 \n"
"ss39sfdone:                   \n"
"		cmpa.l  #0x1003FF, %a1 \n"
"		bls.s ss39sfmloop      \n"
"		moveq          #1, %d0 \n"
"		bgnd                   \n"
);

__asm(
".global FormatFlash         \n"
".global WriteBuffer         \n"
"                            \n"
"WriteBuffer:                \n"
"       movea.l #0x100000,%a1\n" /* Store buffer location */
"       cmp.w       #1, %d6  \n"
"       beq WriteBufferOLD   \n"
"       bra WriteBufferNEW   \n"
"FormatFlash:                \n"
"       subq.l #1, %a1       \n" /* .. */
"       cmp.w      #1, %d6   \n"
"       beq FormatFlashOLD   \n"
"       bra FormatFlashNEW   \n"
);
