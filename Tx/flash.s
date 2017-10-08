.global Syscfg
.global FormatFlash
.global WriteBuffer
.global NiceTry

Syscfg: 

    # This abomination requires further work..    
    moveq.l #1       , %d0
    moveq.l #0x40    , %d1
    move.w  #0x5555  , %d4

    movea.l #0xFFFC14, %a0
    movea.l #0xAAAA  , %a2
    suba.l  %a5      , %a5

    # Make a copy of address 0 and Send ID CMD for L/W flash
    move.w  (%a5)    , %d3
    move.w  %a2      ,(%a2)
    move.w  %d4      ,(0x5554)
    move.w  #0x9090  ,(%a2)
    
    bsr.b   Delay

    # Make a new copy of address 0 and compare
    move.w  (%a5)+   , %d7
    cmp.w   %d3      , %d7
    beq.b   TryHW          /* Try H/W if same val is read */
    move.b  (%a5)    , %d7 /* Copy dev ID                 */
    move.w  (%a5)    , %d3
    move.w  %a2      ,(%a2)
    move.w  %d4      ,(0x5554)
    move.w  #0xF0F0  ,(%a2)
    bsr.b   Delay
    bsr.b   Delay
    bsr.b   Delay
    bsr.b   Delay
    bsr.b   Delay
    moveq.l #2       , %d6 /* Ind toggle-flash */
    bra.b   DoneID 

Delay:
	move.w  #0x1800  , %d2
Dloop:
	dbra    %d2, Dloop /* %d2 becomes 0xFFFF */
rts

    # Same data read, trying H/W flash
TryHW:
    moveq.l #1       , %d6   /* Ind old flash        */
    move.w  %d1      ,(%a0)+ /* Latching up H/W      */
    or.w    %d1      ,(%a0)
    bsr.b   Delay
    move.w  %d2      ,-(%a5) /* Reset flash          */
    move.w  %d2      ,(%a5)
    bsr.b   Delay
    move.w  #0x9090  ,(%a5) /* Send ID CMD, H/W flsh */
    move.w  (%a5)+   , %d7  /* Make a new copy n cmp */
    cmp.w   %d3      , %d7
    beq.w   UnkFlash
    move.b  (%a5)    , %d7  /* Cpy dev id / ntr read */
    clr.w   -(%a5)

DoneID:
    cmpi.b  #1       , %d6
    bne.b   LWFlash

# # # H/W flash # # # # # # # # #

    lea     HVT      , %a0 /* Address of id table */
    moveq.l #3       , %d3 /* 3 sizes */
    moveq.l #2       , %d5
   
NextSize:
    moveq   #1       , %d2
HVtstL:
    cmp.b   (%a0)+   , %d7
    beq.w   ID_Match
    dbra    %d2, HVtstL
    
    lsl.w   #1       , %d5 /* double size  */
    subq.b  #1       , %d3
    bne.b   NextSize
    bra.b   UnkFlash

# # # L/W flash # # # # # # # # #    
    
LWFlash:
    moveq.l #8       , %d5 /* Prepare size as 0x80000  */
    move.w  %d7      , %d1 /* Store another copy of ID */
    lsr.w   %d5      , %d1 /* Shift down manuf ID      */
# Class 29 flash  
    cmpi.b  #0x01    , %d1 /* AMD         */
    beq.b   Class29
    cmpi.b  #0x20    , %d1 /* ST          */
    beq.b   Class29 
    cmpi.b  #0x1C    , %d1 /* EON         */
    beq.b   Class29 
    cmpi.w  #0x37A4  , %d7 /* AMIC    010 */
    beq.b   Size128

# Class 39 flash
    cmpi.w  #0xDAA1  , %d7 /* Winbond 010 */
    beq.b   Size128
    cmpi.w  #0x9D1C  , %d7 /* PMC     010 */
    beq.b   Size128
    cmpi.w  #0x9D4D  , %d7 /* PMC     020 */
    beq.b   Size256
    cmpi.w  #0xBFB4  , %d7 /* SST     512 */
    beq.b   Size64
    cmpi.w  #0xBFB5  , %d7 /* SST     010 */
    beq.b   Size128
    cmpi.w  #0xBFB6  , %d7 /* SST     020 */
    beq.b   Size256
# Atmel requires another routine
    moveq.l #3       , %d6
    cmpi.w  #0x1F5D  , %d7 /* Atmel   512 */
    beq.b   Size64    
    cmpi.w  #0x1FD5  , %d7 /* Atmel   010 */
    beq.b   Size128
    cmpi.w  #0x1FDA  , %d7 /* Atmel   020 */
    beq.b   Size256
    
    # -:hacks:-
    moveq.l #2       , %d6
    cmpi.w  #0x0022  , %d7 /* AMD, T7/T8  */
    bne.b   UnkFlash    

    moveq.l #16      , %d5
    #2281 T8
Unicorns:
    cmpi.w  #0x2281  , %d3 /* Trionic 8 */
    beq.b   Size256
    cmpi.w  #0x2223  , %d3 /* Trionic 7 */
    beq.b   Size128
    bra.b   UnkFlash
Class29:
    cmpi.b  #0x21    , %d7
    beq.b   Size64
    cmpi.b  #0x20    , %d7
    beq.b   Size128
UnkFlash:
    clr.l   %d0 /* Indicate fault */
Size64:
    lsr.w   #1       , %d5
Size128:
    lsr.w   #1       , %d5
Size256:
ID_Match:
    swap    %d5
    movea.l %d5      , %a1
bgnd

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
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
    move.w  #0x5555  ,(0x5554)
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
        
    cmpi.w  #0xFFFF  ,(%a0)
    beq.b   DataIdentToggle
    
    move.w  %a2      ,(%a2)
    move.w  #0x5555  ,(0x5554)
    move.w  #0x8080  ,(%a2)
    
    move.w  %a2      ,(%a2)
    move.w  #0x5555  ,(0x5554)
    move.w  #0x1010  ,(%a2)
    

ToggleWait:
    move.w  (%a0)    , %d2
    cmp.w   (%a0)    , %d2   
    bne.b   ToggleWait
    
DataIdentToggle:
    addq.l  #2       , %a0
    cmpa.l  %a1      , %a0      
    bcs.b   FormatFlashNEW
    
    moveq.l #1       , %d0
bgnd



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
    cmpi.w  #0xFFFF  ,(%a3)+
    bne.b   NotIdentAtE
    dbra    %d0,      CheckE

    movea.l %a3      , %a0
    bra.b   DataIdentAtE

NotIdentAtE:

    movea.l %a0      , %a3
    move.l  %d3      , %d0 
 
# Unlock
    move.w  %a2      ,(%a2)
    move.w  #0x5555  ,(0x5554)
    move.w  #0xA0A0  ,(%a2)
  
ErasePageAT:
    move.w  #0xFFFF  ,(%a3)+
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

WriteBufferNEW:

    cmpm.w  (%a0)+   ,(%a1)+
    beq.b   ToggleIdent      
    move.w  %a2      ,(%a2)
    move.w  #0x5555  ,(0x5554)
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

# # # # # # # # # # # #
# # # # # # # # # # # #
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
# # # # # # # # # # # #
# # # # # # # # # # # #
   
FormatFlashOLD:

    # Zero flash
    # Make a copy of start addr
    movea.l %a0      , %a2  
OOResTries:                 
    moveq.l #25      , %d0

OOloop:
    tst.w   (%a2)+
    beq.b   OOisOO         
    move.w  #0x4040  ,-(%a2)
    clr.w   (%a2)        
    bsr.b   Delay_10uS  
    move.w  #0xC0C0  ,(%a2)
    bsr.b   Delay_6uS
    tst.w   (%a2)
    bne.b   DecOO

    clr.w   (%a2)
    bra.b   OOloop   
DecOO:    
    subq.b  #1       , %d0 
    beq.b   EndFF
OOisOO:
    cmpa.l  %a1    , %a2 
    bcs.b   OOResTries

    # -:Format flash:-
    move.w  #1000    , %d0 /* Maximum number of tries */
    move.w  #0x2020  , %d1
    move.w  #0xFFFF  , %d2

FFloop:                     
    cmp.w   (%a0)+   , %d2
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
    cmp.w   (%a0)    , %d2
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
                         
WriteBuffer:

    # Store buffer location
    # Store number of WORDS to write
    movea.l #0x100000, %a1
    move.w  #512     , %d1
    
    cmpi.w  #1       , %d6  
    beq.w   WriteBufferOLD
    
    # Prestore a command that is used by both
    movea.l #0xAAAA  , %a2
    
    cmpi.w  #2       , %d6  
    beq.w   WriteBufferNEW  
    cmpi.w  #3       , %d6      
    beq.w   WriteBufferAtmel
    
    bra.b   NiceTry
    
FormatFlash:

    # Start from address 0
    suba.l  %a0      , %a0
    
    cmpi.w  #1       , %d6
    beq.w   FormatFlashOLD
    
    # Prestore a command that is used by both
    movea.l #0xAAAA  , %a2

    cmpi.w  #2       , %d6   
    beq.w   FormatFlashNEW      
    cmpi.w  #3       , %d6
    bra.w   FormatFlashAtmel  

NiceTry:
    bsr.w   Delay
    clr.l   %d0
bgnd

# Table for H/V flash:
# # # # # # # # AMD , Other guys
HVT:      .byte 0x25, 0xB8 /*  64 K (T5.2) */
HV128:    .byte 0xA7, 0xB4 /* 128 K (T5.5) */
HV256:    .byte 0x2A, 0xBD /* 256 K "T5.7" */
