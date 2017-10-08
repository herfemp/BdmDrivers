
	movea.l #0x100800,%sp /* Reset stack pointer */
	
	cmp.b   #1, %d0
	beq     WriteBuffer	
	cmp.b   #2, %d0
	beq     FormatFlash	
	cmp.b   #3, %d0
	beq     Syscfg
    bra.w   NiceTry

	/* Wrong registry val */
    
    
 /*   
	moveq     #50, %d0
	move.l    %d0, -(%sp)
	bsr DelaymS	
	move.l #0, %d0
	bgnd


.global DelaymS
DelaymS:           
	move.l  %d0, -(%sp)
	move.l  8(%sp),%d0 
	mulu.l  #1300, %d0 
mSloop:            
	sub.l #1, %d0      
	tst.l %d0          
	bne mSloop       
	move.l (%sp)+, %d0 
rts                

*/

