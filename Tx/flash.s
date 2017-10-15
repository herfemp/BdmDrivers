# Universal bdm driver

# Known bugs / eventual problems:
# 28Fxxx:
# If format/write compare indicates a word as "not correct" only once, the optional "verify as regular data" step will be skipped.
# The chance of this happening is astronomically small (and it's optional) so I'll leave it as is atm.












# Usage:
# Setup TPURAM or SRAM to 0x100000 ( Preferably the first since it's way faster )
# Initialize CSPAR and all that stuff to base flash @ address 0
# Initialize clock
# if Trionic 5, 16,7 MHz is a must when equipped with original flash (Delays are calibrated for that)
# If it has toggle/Atmel flash just go for 20 MHz. Motorola overengineered the crap out of these so no need to chickenshit on 16 MHz ECU's
# Trionic 7 is locked to an external clock of 16 MHZ.
# Trionic 8 can be run @ 32 MHz

#######
# Init:

# Upload driver to 0x100400
# Store 3 in register D0, (This tells the driver to initialize stuff)
# Set PC to 0x100400
# Start it

# When it enters bdm again:
# Read D0, If it's 1 everything is OK, if not 1 _ABORT_!
# D7 contains manufacturer and device ID
# D3 contains extended ID (Trionic 7 and 8)
# D6 contains which type of flash (1 Trionic 5 stock flash, 2 toggle flash, 3 Atmel)
# A1 contains size of flash (Store this value now)

# Really useful on Trionic 5 since you only have to make sure selected file is equal in size or smaller
# Set up a loop that divides size by file size to flash everything
# (You can also bump the clock here if D6 is higher than 1 on Trionic 5)

########
# Erase:

# Store 2 in register D0, (This tells the driver to format flash)
# Set PC to 0x100400
# Start it

# When it enters bdm again:
# Read D0, If it's 1 everything is OK, if not 1 _ABORT_!
# A0 contains last address that was worked on (Only useful if something went wrong and you want to know where)

########
# Write:

# If previous command went ok you already have the right value in D0
# Write 0 to A0

# -:"loop":-
# Upload 1024 bytes starting from 0x100000
# Set PC to 0x100400
# Start it

# When it enters bdm again:
# Read D0.
# If 1, repeat loop. ( A0 autoincrements and D0 already is 1 )
# If 0, something went wrong. _ABORT_

# One last word about toggle-flash:
# No, I'm _NOT_ going to implement the damn error-toggle used by AMD since I'd need another method on T7/T8!
# Just use a timer to detect if the driver gets stuck. Nothing can be done anyway..

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

	movea.l #0x100800,%sp /* Reset stack pointer */
	
	cmpi.b  #1, %d0
	beq.b   WriteBuffer	
	cmpi.b  #2, %d0
	beq.b   FormatFlash	
	cmpi.b  #3, %d0
	beq.b   Syscfg
    cmpi.b  #4, %d0 /* Special case: Init MCP */
    beq.w   InitMCP
    bra.b   NiceTry

WriteBuffer:

    # Store buffer location
    # Store number of WORDS to write
    movea.l #0x100000, %a1
    move.w  #512     , %d1
    
    cmpi.w  #1       , %d6
    beq.w   WriteBufferOLD
    cmpi.w  #2       , %d6
    beq.w   WriteBufferNEW
    cmpi.w  #3       , %d6
    beq.w   WriteBufferAtmel
    cmpi.w  #82      , %d6
    beq.w   WriteBufferMCP
    bra.b   NiceTry
    
FormatFlash:

    # Start from address 0
    suba.l  %a0      , %a0
    
    cmpi.w  #1       , %d6
    beq.w   FormatFlashOLD
    cmpi.w  #2       , %d6
    beq.w   FormatFlashNEW
    cmpi.w  #3       , %d6
    beq.w   FormatFlashAtmel
    cmpi.w  #82      , %d6
    beq.w   FormatFlashMCP
NiceTry:
    bsr.b   Delay
    clr.l   %d0
bgnd

Syscfg: 

    # This abomination requires further work..    
    moveq.l #1       , %d0  /* Presume result to be ok   */
    
    moveq.l #0x40    , %d1  /* Used for H/W on Trionic 5 */
    movea.l #0xFFFC14, %a0

    move.w  #0x5555  , %d4  /* Used by L/W flsh routines */
    movea.l #0xAAAA  , %a2
    movea.l #0x5554  , %a6

    suba.l  %a5      , %a5  /* Reset pointer to addr 0   */

    # Make a copy of address 0 and Send ID CMD for L/W flash
    move.w  (%a5)    , %d3
    move.w  %a2      ,(%a2)
    move.w  %d4      ,(%a6)
    move.w  #0x9090  ,(%a2)
    
    bsr.b   Delay

    # Make a new copy of address 0 and compare
    move.w  (%a5)+   , %d7
    cmp.w   %d3      , %d7
    beq.b   TryHW            /* Try H/W if same val is read */
    move.b  (%a5)    , %d7   /* Copy dev ID                 */
    move.w  (%a5)    , %d3   /* Store a full copy of addr 2 */
    move.w  %a2      ,(%a2)  /* Reset flash                 */
    move.w  %d4      ,(%a6)
    move.w  #0xF0F0  ,(%a2)
    bsr.b   Delay
    bsr.b   Delay
    bsr.b   Delay
    bsr.b   Delay
    bsr.b   Delay
    moveq.l #2       , %d6   /* Ind toggle-flash */
    bra.w   LWFlash 

Delay:
	move.w  #0x1800  , %d2
Dloop:
	dbra    %d2,     Dloop   /* %d2 becomes 0xFFFF */
rts

    # Same data read, trying H/W flash
TryHW:
    moveq.l #1       , %d6   /* Ind old flash         */
    move.w  %d1      ,(%a0)+ /* Latching up H/W       */
    or.w    %d1      ,(%a0)
    bsr.b   Delay
    move.w  %d2      ,-(%a5) /* Reset flash           */
    move.w  %d2      ,(%a5)
    bsr.b   Delay
    move.w  #0x9090  ,(%a5)  /* Send ID CMD, H/W flsh */
    move.w  (%a5)+   , %d7   /* Make a new copy n cmp */
    cmp.w   %d3      , %d7
    beq.w   UnkFlash
    move.b  (%a5)    , %d7   /* Cpy dev id / ntr read */
    clr.w   -(%a5)

# # # H/W flash # # # # # # # # #

    lea     HVT      , %a0   /* Address of id table */
    moveq.l #3       , %d3   /* 3 sizes     */
    moveq.l #2       , %d5
   
NextSize:
    moveq   #1       , %d2
HVtstL:
    cmp.b   (%a0)+   , %d7
    beq.w   ID_Match
    dbra    %d2, HVtstL
    
    lsl.w   #1       , %d5   /* double size */
    subq.b  #1       , %d3
    bne.b   NextSize
    bra.b   UnkFlash

# # # L/W flash # # # # # # # # #    
    
LWFlash:
    moveq.l #8       , %d5   /* Prepare size as 0x80000  */
    move.w  %d7      , %d1   /* Store another copy of ID */
    lsr.w   %d5      , %d1   /* Shift down manuf ID      */

# Class 29 flash  
    cmpi.b  #0x01    , %d1   /* AMD         */
    beq.b   Class29
    cmpi.b  #0x20    , %d1   /* ST          */
    beq.b   Class29 
    cmpi.b  #0x1C    , %d1   /* EON         */
    beq.b   Class29 
    cmpi.w  #0x37A4  , %d7   /* AMIC    010 */
    beq.b   Size128

# Class 39 flash
    cmpi.w  #0xDAA1  , %d7   /* Winbond 010 */
    beq.b   Size128
    cmpi.w  #0x9D1C  , %d7   /* PMC     010 */
    beq.b   Size128
    cmpi.w  #0x9D4D  , %d7   /* PMC     020 */
    beq.b   Size256
    cmpi.w  #0xBFB4  , %d7   /* SST     512 */
    beq.b   Size64
    cmpi.w  #0xBFB5  , %d7   /* SST     010 */
    beq.b   Size128
    cmpi.w  #0xBFB6  , %d7   /* SST     020 */
    beq.b   Size256
    cmpi.w  #0x0022  , %d7   /* AMD, T7/T8  */    
    beq.b   Unicorns

# Atmel
    moveq.l #3       , %d6   /* Change drv  */
    cmpi.w  #0x1F5D  , %d7   /* Atmel   512 */
    beq.b   Size64    
    cmpi.w  #0x1FD5  , %d7   /* Atmel   010 */
    beq.b   Size128
    cmpi.w  #0x1FDA  , %d7   /* Atmel   020 */
    beq.b   Size256
    bra.b   UnkFlash    

Unicorns:
    moveq.l #16      , %d5   /* 256 K = 1 M */
    cmpi.w  #0x2223  , %d3   /* Trionic 7   */
    beq.b   Size128
    cmpi.w  #0x2281  , %d3   /* Trionic 8   */
    beq.b   Size256
    bra.b   UnkFlash
Class29:
    cmpi.b  #0x21    , %d7
    beq.b   Size64
    cmpi.b  #0x20    , %d7
    beq.b   Size128
UnkFlash:
    clr.l   %d6 /* Make sure no driver is selected */
    clr.l   %d0 /* Indicate fault                  */
Size64:
    lsr.w   #1       , %d5
Size128:
    lsr.w   #1       , %d5
Size256:
ID_Match:
    swap    %d5
    movea.l %d5      , %a1
    clr.l   %d5
    subq.l  #1       , %d5
bgnd

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# Could never get the damn erase command to work.
# We'll use Atmel's weird page write feature instead
FormatFlashAtmel:
/*
    # Check for 29c020 since that requires a larger pagebuf
    # moveq.l #63      , %d3
    # cmpi.w  #0x1FDA  , %d7
    # bne.b   FormatLoopAt
    moveq.l #127     , %d3
    
FormatLoopAt:

    movea.l %a0      , %a3
    move.l  %d3      , %d0   
CheckE: 
    cmp.w   %d5      ,(%a3)+
    bne.b   NotIdentAtE
    dbra    %d0,      CheckE

    movea.l %a3      , %a0
    bra.b   DataIdentAtE

NotIdentAtE:

    movea.l %a0      , %a3
    move.l  %d3      , %d0 
 
# Unlock
    move.w  %a2      ,(%a2)
    move.w  %d4      ,(%a6)
    move.w  #0xA0A0  ,(%a2)
  
ErasePageAT:
    move.w  %d5      ,(%a3)+
    dbra    %d0, ErasePageAT

    # bsr.w  Delay
    # bsr.w  Delay
    # bsr.w  Delay
    
AtmelWaitE:                   	
    move.w  (%a0)    , %d0
    cmp.w   (%a0)    , %d0
    bne.b   AtmelWaitE 
     
DataIdentAtE:
    cmpa.l  %a1      , %a0      
    bcs.b   FormatLoopAt
    */
    bsr.w   Delay
    moveq.l #1, %d0
bgnd

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# Ugly code is an understatement...
WriteBufferAtmel:
  
    # I hate Atmel...
    # One datasheet states 128 bytes for 512/010
    # , another one 256. Guess which one is correct?
    
    # Check for 29c020 since that requires a larger pagebuf
    # moveq.l #63      , %d3
    # cmpi.w  #0x1FDA  , %d7
    # bne.b   WriteLoopAt
    moveq.l #127     , %d3

WriteLoopAt:

    movea.l %a0      , %a3 
    movea.l %a1      , %a4 
    move.w  %d3      , %d0

CheckLAtW: 
    cmpm.w  (%a4)+   ,(%a3)+
    bne.b   NotIdentAtW
    dbra    %d0,      CheckLAtW

    movea.l %a3      , %a0
    movea.l %a4      , %a1

    sub.w   %d3      , %d1
    subq.w  #1       , %d1      
    beq.b   WriteAtDone
    bra.b   WriteLoopAt

NotIdentAtW:
    movea.l %a0      , %a3
    movea.l %a1      , %a4
    move.w  %d3      , %d0 

# Unlock
    move.w  %a2      ,(%a2)
    move.w  %d4      ,(%a6)
    move.w  #0xA0A0  ,(%a2)

WritePageAT:
    move.w  (%a4)+   ,(%a3)+
    dbra    %d0, WritePageAT

AtmelWaitW:                   	
    move.w  (%a0)    , %d0
    cmp.w   (%a0)    , %d0
    bne.b   AtmelWaitW
    bra.b   WriteLoopAt
      
WriteAtDone:
    moveq.l #1       , %d0   
bgnd

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

FormatFlashNEW:
        
    cmp.w   (%a0)    , %d5
    beq.b   DataIdentToggle
    
    move.w  %a2      ,(%a2)
    move.w  %d4      ,(%a6)
    move.w  #0x8080  ,(%a2)

    move.w  %a2      ,(%a2)
    move.w  %d4      ,(%a6)
    move.w  #0x1010  ,(%a2)

ToggleWait:
    move.w  (%a0)    , %d2
    cmp.w   (%a0)    , %d2   
    bne.b   ToggleWait
    bra.b   FormatFlashNEW
    
DataIdentToggle:
    addq.l  #2       , %a0
    cmpa.l  %a1      , %a0      
    bcs.b   FormatFlashNEW
    moveq.l #1       , %d0
bgnd

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

WriteBufferNEW:

    cmpm.w  (%a0)+   ,(%a1)+
    beq.b   ToggleIdent      
    move.w  %a2      ,(%a2)
    move.w  %d4      ,(%a6)
    move.w  #0xA0A0  ,(%a2)
    move.w  -(%a1)   ,-(%a0)

ToggleWaitW:                   	
    move.w  (%a0)    , %d0
    cmp.w   (%a0)    , %d0
    bne.b   ToggleWaitW

    # Go back for verification
    bra.b   WriteBufferNEW       

ToggleIdent:
    subq.w  #1       , %d1
    bne.b   WriteBufferNEW

    # No counter yet
    moveq   #1       , %d0
bgnd

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

WriteBufferOLD:

    moveq.l #25      , %d0

WriteLoop:
    # Is it even necessary to write this byte?
    cmpm.w  (%a0)+   ,(%a1)+
    beq.b   dataident

WriteLoopM:    
    # Send write command
    move.w  #0x4040  ,-(%a0) 
    move.w -(%a1)    ,(%a0) 
    bsr.b	Delay_10uS
    
    # Send "Write compare" command
    move.w  #0xC0C0  ,(%a0)
    bsr.b   Delay_6uS
    
    # Did it stick?
    cmpm.w  (%a0)+   ,(%a1)+
    bne.b   DecWR
    # Paranoia; Revert back to read-mode and compare once more. 
    clr.w   -(%a0)
    tst.w   -(%a1)
    bra.b   WriteLoop
DecWR:    
    # Nope. Decrement tries and try again.. if allowed
    subq.b  #1       , %d0
    bne.b   WriteLoopM
bgnd

dataident:
    subq.w  #1       , %d1
    bne.b   WriteBufferOLD
    
    moveq.l #1       , %d0
    # CAT28f plays the b*tch-game. Make sure it is in read mode..
    clr.w   (%a0)
bgnd

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
Delay_10uS:
    moveq.l #15      , %d3
us10loop:
	subq.l  #1       , %d3
	bne.b   us10loop
rts
Delay_6uS:
    moveq.l #8       , %d3
us6loop:
	subq.l  #1       , %d3
	bne.b   us6loop
rts
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
   
FormatFlashOLD:

    # Zero flash
    # Make a copy of start addr
    movea.l %a0      , %a2  
OOResTries:                 
    moveq.l #25      , %d0

OOloop:
    tst.w   (%a2)+
    beq.b   OOisOO
BoostOO:    
    move.w  #0x4040  ,-(%a2)
    clr.w   (%a2)        
    bsr.b   Delay_10uS  
    move.w  #0xC0C0  ,(%a2)
    bsr.b   Delay_6uS
    tst.w   (%a2)+
    bne.b   DecOO
    clr.w   -(%a2)
    bra.b   OOloop   
DecOO:    
    subq.b  #1       , %d0 
    beq.b   EndFF
    bra.b   BoostOO
OOisOO:
    cmpa.l  %a1      , %a2 
    bcs.b   OOResTries

    # -:Format flash:-
    move.w  #1000    , %d0 /* Maximum number of tries */
    move.w  #0x2020  , %d1

FFloop:                     
    cmp.w   (%a0)+   , %d5
    beq.b   DataisFF
    move.w  %d1      ,-(%a0)
    move.w  %d1      ,(%a0)

# TODO: Recalibrate with bne _.B_(!)
    # I know. Long is wasting space..
    move.l  #0x32C8  , %d3
mSloop:
    sub.l   #1       , %d3      
    tst.l   %d3          
    bne     mSloop

    move.w  #0xA0A0  ,(%a0)
    bsr.b   Delay_6uS            
    cmp.w   (%a0)    , %d5
    bne.b   DecFF
    clr.w   (%a0)        
    bra.b   FFloop
DecFF:    
    subq.w  #1       , %d0
    beq.b   EndFF
DataisFF:           
    cmpa.l	%a1      , %a0
    bcs.b   FFloop
    moveq.l #1       , %d0
   
EndFF:
bgnd

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# This code obv doesn't work yet. I'm working on it :)
# Found another bug that forced me to push this

Delay_50CK:
    moveq.l #49      , %d5
    bra.b   DelayMCP
Delay_500CK:
    move.w  #499     , %d5
DelayMCP:
    bra.b   SWSR
    dbra    %d5,  DelayMCP   /* %d5 returns to FF..   */
rts

InitMCP:
    bsr.b   SWSR             /* Service watchdog      */
    moveq.l #1       , %d0   /* Store ack             */
    moveq.l #82      , %d6   /* Set driver mode; MCP  */ /* Trionic EIGHT, cpu TWO in case you wonder about the numbering :) */
    movea.l #0x40100 , %a1   /* Store last flash addr */
    movea.l #0xFFF80C, %a4   /* Store addr of CMFICTL1*/
    movea.l #0xFFF80E, %a5   /* Store addr of CMFICTL2*/    
    movea.l #0xFFF800, %a6   /* Store addr of CMFIMCR */

    ori.w   #0x8800  ,(%a6)  /* Stop CMFI/Negate lock */
    clr.l   (0xFFF808)       /* Base CMFI @ addr 0    */
    andi.w  #0x3FFF  ,(%a6)  /* Start/Negate shadow   */
    bsr.b   Delay_500CK      /* Be nice to the host   */
bgnd    

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    
SWSR:
    move.b  #0x55    ,(0xFFFA27)
    move.b  #0xAA    ,(0xFFFA27)
rts

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# CMFIMCR:  0xFFF800
# CMFITST:  0xFFF804
# CMFIBAR:  0xFFF808
# CMFICTL1: 0xFFF80C %a4
# CMFICTL2: 0xFFF80E %a5
# SYNCR:    0xFFFA04
# SWSR:     0xFFFA27

# %d6 = Flashmode (Do not change)

WriteBufferMCP:

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

FormatFlashMCP:

    andi.w  #0x3FFF  ,(%a6)  /* Negate shadow         */
    move.w  #0x2250  ,(%a4)  /* Configure timings     */
    clr.l   (0xFFF804)       /* CMFITST               */ /* Check this! */



# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


.align 4
# Table for H/V flash:
# # # # # # # # AMD , Other guys
HVT:      .byte 0x25, 0xB8 /*  64 K (T5.2) */
HV128:    .byte 0xA7, 0xB4 /* 128 K (T5.5) */
HV256:    .byte 0x2A, 0xBD /* 256 K "T5.7" */
