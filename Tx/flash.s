.global Syscfg

.global FormatFlashOLD 
.global FormatFlashNEW
.global FormatFlash

.global WriteBufferOLD
.global WriteBufferNEW 
.global WriteBuffer 

.global Delay_6uS	
.global Delay_10uS
.global Delay_20uS



/*        
Delay_20uS:       
	move.l %d0, -(%sp)
	move.l  #26, %d0  
us20loop:         
	sub.l #1, %d0     
	tst.l %d0         
	bne us20loop      
	move.l (%sp)+, %d0
rts
*/


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
Syscfg:
       moveq       #0x40, %d1     /* Wait 64~ ms; Also used for H/V on T5 */
       moveq          #0, %d6     /*  */
       moveq          #0, %d7    
       movea.l       %d7, %a0    
       movea.l #0xFFFC14, %a1  
       movea.l #0xFFFC17, %a2
       movea.l   #0xAAAA, %a3     /* Run ID-command for low-voltage flash */	   
/*   Experimental part   */
       move.w        %d7,(%a1)    /* Negate high-voltage */
       move.b        %d7,(%a2)   
       move.w    #0xF0F0,(%a0)    /* Reset flash */
       move.l        %d1,-(%sp)  
       bsr DelaymS                /* Do not restore sp. It will reset itself soon enough */
/* * * * * * * * * * * * */
       move.w      (%a0), %d0     /* Store a copy of address 0 */

       move.w    #0xAAAA,(%a3)   
       move.w    #0x5555,(0x5554)
       move.w    #0x9090,(%a3)   
       move.w      (%a0), %d7     /* Make another copy of address 0 */
       cmp.w         %d0, %d7     /* Identical? No: We have our id. Yes: Continue */
       bne.s IDfound           
       moveq          #1, %d6     /* Indicate Old routine. Dangerous as F. I have no control of which registers C use */
       move.w        %d1,(%a1)    /* Latch up high-voltage */
       move.b        %d1,(%a2)   
       move.l        %d1, -(%sp) 
       bsr.w DelaymS             
       move.w    #0xFFFF,(%a0)    /* Reset flash */
       move.w        %d0,(%a0)   
       move.w    #0x9090,(%a0)    /* Send ID-command */
       move.w      (%a0), %d7     /* Make a copy of the lower part */
IDfound:                
       addq           #2, %a0     /* Copy the higher part from one of the flash memories */
       move.b      (%a0), %d7     /* -Obviosly dangerous if the user decided to populate 2 different parts.. */
       move.l        %d1, -(%sp) 
       bsr.w DelaymS             
       moveq          #1, %d0    
       bgnd                    



/* Is it really worth checking the error bit..? */  
FormatFlashNEW:             
       move.w    #0xF0F0,(%a0)  /* Reset Flash */
       pea (100).w          
       bsr.w DelaymS          
       addq.l         #4, %sp        
/* A0 and A1 is start and end */
       movea.l   #0xAAAA, %a2  
       movea.w   #0x5554, %a3  
FormatLoopn:                
       cmpi.w    #0xFFFF,(%a0)
       beq.s   DataIdentn   
       move.w        %a2,(%a2)
       move.w    #0x5555,(%a3)
       move.w    #0x8080,(%a2)
       move.w        %a2,(%a2)
       move.w    #0x5555,(%a3)
       move.w    #0x1010,(%a2)
                            
ToggleWaitn:                 /* Add check for amd! Mask off error status in the first reads and then read again!? */
       move.w      (%a0), %d0    
       move.w      (%a0), %d1    
       cmp.w         %d1, %d0      
       bne.s   ToggleWaitn  
/*     bne.s   Checkdq5f    */
       bra.s   LoopCheckn   
/* Fix:                      */
/*Checkdq5f:                 
       andi.w    #0x2020, %d1 
       beq.s ToggleWaitn    
       move.w      (%a0), %d0    
       move.w      (%a0), %d1    
       cmp.w         %d1, %d0      
       beq.s   LoopCheckn   
       moveq          #0, %d0   Fff... 
       bgnd                 */
                            
DataIdentn:                 
       addq.l         #2, %a0       
                            
LoopCheckn:                 
       cmpa.l        %a1, %a0      
       bls.s   FormatLoopn  
       moveq          #1, %d0       
       bgnd                  /* This is how you make IDA pro go apesh!t. No rts; EVERYTHING is one huge function */

/*A0 and A1 is start and end*/    
FormatFlashOLD:             
       move.w	 #0xFFFF, %d2 
       move.w	     %d2,(%a0)
       move.w	     %d2,(%a0)
/*     Zero flash          */
       movea.l	     %a0, %a2  /* Make a copy of start addr */
OOResTries:                 
       moveq	     #25, %d0 
OOloop:                     
       move.w      (%a2), %d1 
       beq.s OOisOO         
       move.w	 #0x4040,(%a2)
       clr.w       (%a2)        
       bsr.s   Delay_10uS     
       move.w	 #0xC0C0,(%a2)
       bsr.s   Delay_6uS      
       move.w      (%a2), %d1 
       beq.s OOEnRead       
       subq.b         #1, %d0 
       bra.s OOloopcheck    
OOEnRead:                    /* Paranoid, read it once more. Its ample fast anyway */
       clr.w  (%a2)        
OOloopcheck:                
       tst.b   %d0          
       bne.s OOloop         
       bgnd    
OOisOO:                     
       addq.l  #2     , %a2 
       cmpa.l  %a1    , %a2 
       bls.s OOResTries
     
/*      Format flash         */
       move.w	#1000  , %d0   /* Maximum number of tries */
FFloop:                     
       cmpi.w	#0xFFFF,(%a0)
       beq.s DataisFF
       move.w	#0x2020,(%a0)
       move.w	#0x2020,(%a0)
       pea     (10).w       
       bsr.w DelaymS              
       addq.l  #4     , %sp 
       move.w  #0xA0A0,(%a0)
       bsr.w Delay_6uS            
       cmpi.w  #0xFFFF,(%a0)
       beq.s FFenRead             
       subq.w  #1     , %d0 
       bra.s FFloopChck           
FFenRead:                   
       clr.w   (%a0)        
       bra.s FFloop               
DataisFF:                   
       addq.l	#2     , %a0
FFloopChck:                 
       cmpa.l	%a1    , %a0 
       bhi.s EndCheck             
       tst.w	%d0          
       bne.s FFloop               
EndCheck:                   
       tst.w	%d0          
       bne.s FormatSucc              
       bgnd                 
FormatSucc:                 
       moveq	#1     ,%d0  
       bgnd                 
		

Delay_6uS:        
	move.l %d0, -(%sp)
	move.l  #8, %d0   
us6loop:          
	sub.l #1, %d0     
	tst.l %d0         
	bne us6loop       
	move.l (%sp)+, %d0
rts 
	   
Delay_10uS:       
	move.l %d0, -(%sp)
	move.l  #13, %d0  
us10loop:         
	sub.l #1, %d0     
	tst.l %d0         
	bne us10loop      
	move.l (%sp)+, %d0
rts      
WriteBufferOLD:              
ResTries:                    
       moveq   #25    , %d0   /* Tries */
WriteLoop:                   
       move.w  (%a1)  , %d1   /* Move data of buffer */
       cmp.w   (%a0)  , %d1   /* Compare it against flash location */
       beq     dataident      /* Identical */
       move.w  #0x4040,(%a0) 
       move.w  (%a1)  ,(%a0) 
       bsr.s   Delay_10uS     /* 10 uS */
       move.w  #0xC0C0,(%a0)  /* Write compare command */
       bsr.s   Delay_6uS         
       cmp.w   (%a0)  , %d1   /* Compare actual data */
       beq.s   ReadMode      
       subq.b  #1     , %d0   /* Decrement tries if we got here */
       bra.s   loopcheck     
ReadMode:                    
       clr.w   (%a0)         
loopcheck:                   
       tst.b   %d0           
       bne.s   WriteLoop     
       bgnd                  
dataident:                   	
       addq.l  #2     , %a0 
       addq.l  #2     , %a1  
       cmpa.l  #0x1003FF,%a1 
       bls.s   ResTries      
       moveq   #1     , %d0  
       bgnd                  

/* Is it really worth checking the error bit..? */       
WriteBufferNEW:               
       movea.l   #0xAAAA, %a2 
       movea.l   #0x5554, %a3 
       move.l    #0xAAAA, %d2 
       move.l    #0x5555, %d3 
       move.l    #0xA0A0, %d4 
ss39sfmloop:			       	
       move.w	    (%a1), %d0 
       cmp.w	    (%a0), %d0 
       beq.s ss39sfident      
       move.w        %d2,(%a2)
       move.w        %d3,(%a3)
       move.w        %d4,(%a2)
       move.w      (%a1),(%a0)
ss39sfwait:                   	
       move.w      (%a0), %d0 
       move.w      (%a0), %d1 
       cmp.w         %d1, %d0 
       bne.s ss39sfwait       
       bra.s ss39sfdone       
	   
ss39sfident:                  
       addq.l         #2, %a0
       addq.l         #2, %a1 
ss39sfdone:                   
       cmpa.l  #0x1003FF, %a1 
       bls.s ss39sfmloop      
       moveq          #1, %d0 
       bgnd                   

                            
WriteBuffer:                
       movea.l #0x100000,%a1 /* Store buffer location */
       cmp.w       #1, %d6  
       beq WriteBufferOLD   
       bra WriteBufferNEW   
FormatFlash:                
       subq.l #1, %a1        /* .. */
       cmp.w       #1, %d6   
       beq FormatFlashOLD   
       bra FormatFlashNEW   

