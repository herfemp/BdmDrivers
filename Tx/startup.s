
	movea.l #0x100800,%sp /* Reset stack pointer */
	
	cmp.b #1, %d0
	beq WriteBuffer	
	cmp.b #2, %d0
	beq FormatFlash	
	cmp.b #3, %d0
	beq Syscfg
	
	/* Wrong registry val */
	moveq     #50, %d0
	move.l    %d0, -(%sp)
	bsr DelaymS	
	move.l #0, %d0
	bgnd

	
/* These are WAY off on t8! */
.global Delay_6uS	
.global Delay_10uS
.global Delay_20uS
.global DelaymS

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
             
Delay_20uS:       
	move.l %d0, -(%sp)
	move.l  #26, %d0  
us20loop:         
	sub.l #1, %d0     
	tst.l %d0         
	bne us20loop      
	move.l (%sp)+, %d0
rts 

/* Tuned @ 16,7 MHz */
/* DPTRAM is fast... */
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

