NAME    = TxDriver

CC      = m68k-elf-gcc
CXX     = m68k-elf-g++
AS      = m68k-elf-as

CPU_VARIANT   = mcpu32
LINKER_SCRIPT = cfg.ld

ASFLAGS = -mcpu32 # --register-prefix-optional
CFLAGS  = -mcpu32  -O1 $(NEW_INSTRUCTIONS) -fomit-frame-pointer
LDFLAGS = -mcpu32 -nostdlib -Wl,-s -Wl,-n -T$(LINKER_SCRIPT) -Wl,-Map=$(basename $@).map

ifeq ($(OS),Windows_NT)
   RM = del /Q
   FixPath = $(subst /,\,$1)
else
   ifeq ($(shell uname), Linux)
	  RM = rm -f
	  FixPath = $1
   endif
endif

all:  out/startup.o out/TxDriver.bin 

out/startup.o: startup.s
	$(AS) $(ASFLAGS)  $< -o $@

out/flash.o: flash.s
	$(AS) $(ASFLAGS)  $< -o $@
	
out/TxDriver.bin:    out/flash.o
	$(CC) $(LDFLAGS) -o $@ out/flash.o

ifeq ($(OS),Windows_NT)
	..\utils\bin2header out\TxDriver.bin
	copy TxDriver.bin.h out\TxDriver.bin.h
	del ..\Driverhost\Driverhost\Drivers\TxDriver.bin.h
	del ..\..\Thingy2.0\BDM\TxDriver.bin.h
	copy TxDriver.bin.h ..\Driverhost\Driverhost\Drivers\TxDriver.bin.h
	copy TxDriver.bin.h ..\..\Thingy2.0\BDM\Drivers\TxDriver.bin.h
	del TxDriver.bin.h
	del out\flash.o
	del out\startup.o
	printsize out\txdriver.bin
	del out\TxDriver.bin


else
	@printf "#ifndef LOADER_BIN_H\n" > out/TxDriver.bin.h
	@printf "#define LOADER_BIN_H\n" >> out/TxDriver.bin.h
	@printf "\n" >> out/TxDriver.bin.h
	@printf "static const " >> out/TxDriver.bin.h
	@xxd -i out/TxDriver.bin >> out/TxDriver.bin.h
	@printf "\n" >> out/TxDriver.bin.h
	@printf "#endif\n" >> out/TxDriver.bin.h
	@sed -i 's/out_//' out/TxDriver.bin.h
	@mv out/TxDriver.bin.h ../CPU1/inc
endif

.PHONY: clean
clean:

	@$(RM) $(call FixPath,out/*)
ifeq ($(OS),Windows_NT)
	del *.bin, *.srec
endif

