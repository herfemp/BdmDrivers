NAME    = T5Driver

CC      = m68k-elf-gcc
CXX     = m68k-elf-g++
AS      = m68k-elf-as

CPU_VARIANT   = mcpu32
LINKER_SCRIPT = MC68332.ld

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

all:  out/startup.o out/T5Driver.bin 

out/startup.o: startup.s
	$(AS) $(ASFLAGS)  $< -o $@

out/flash.o:   flash.c
	$(CC) $(CFLAGS) -Wall -ansi $< -c -o $@ 

out/T5Driver.bin:    out/flash.o
	$(CC) $(LDFLAGS) -o $@ out/flash.o

ifeq ($(OS),Windows_NT)
	..\utils\bin2header out\T5Driver.bin
	copy T5Driver.bin.h out\T5Driver.bin.h
	del ..\Driverhost\Driverhost\Drivers\T5\T5Driver.bin.h
	copy T5Driver.bin.h ..\Driverhost\Driverhost\Drivers\T5\T5Driver.bin.h
	del T5Driver.bin.h

else
	@printf "#ifndef LOADER_BIN_H\n" > out/T5Driver.bin.h
	@printf "#define LOADER_BIN_H\n" >> out/T5Driver.bin.h
	@printf "\n" >> out/T5Driver.bin.h
	@printf "static const " >> out/T5Driver.bin.h
	@xxd -i out/T5Driver.bin >> out/T5Driver.bin.h
	@printf "\n" >> out/T5Driver.bin.h
	@printf "#endif\n" >> out/T5Driver.bin.h
	@sed -i 's/out_//' out/T5Driver.bin.h
	@mv out/T5Driver.bin.h ../CPU1/inc
endif

.PHONY: clean
clean:

	@$(RM) $(call FixPath,out/*)
ifeq ($(OS),Windows_NT)
	del *.bin, *.srec
endif

