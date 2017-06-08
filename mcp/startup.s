
	movea.l #0x82000,%sp /* Reset stack pointer */
	
	cmp.b #1, %d0
	beq WriteBuffer	
	cmp.b #2, %d0
	beq FormatFlash	
	cmp.b #3, %d0
	beq Syscfg
	
	/* Wrong registry val */
	move.l #0, %d0
	bgnd
	
.global swsr
swsr:
	move.b	#0x55,	(0xFFFA27)
	move.b	#0xAA,	(0xFFFA27)
	rts
