NAME    = MCPDriver

CC      = m68k-elf-gcc
CXX     = m68k-elf-g++
AS      = m68k-elf-as


CPU_VARIANT=mcpu32
LINKER_SCRIPT=mc68f375.ld


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


all:  out/startup.o out/driver.bin 


out/startup.o: startup.s
	$(AS) $(ASFLAGS)  $< -o $@

out/flash.o:   flash.c
	$(CC) $(CFLAGS) -Wall -ansi $< -c -o $@ 

out/driver.bin:    out/flash.o
	$(CC) $(LDFLAGS) -o $@ out/flash.o

ifeq ($(OS),Windows_NT)
	..\utils\bin2header out\driver.bin
	copy driver.bin.h out\driver.bin.h
	del driver.bin.h

else
	@printf "#ifndef LOADER_BIN_H\n" > out/driver.bin.h
	@printf "#define LOADER_BIN_H\n" >> out/driver.bin.h
	@printf "\n" >> out/driver.bin.h
	@printf "static const " >> out/driver.bin.h
	@xxd -i out/driver.bin >> out/driver.bin.h
	@printf "\n" >> out/driver.bin.h
	@printf "#endif\n" >> out/driver.bin.h
	@sed -i 's/out_//' out/driver.bin.h
	@mv out/driver.bin.h ../CPU1/inc
endif

.PHONY: clean
clean:

	@$(RM) $(call FixPath,out/*)
ifeq ($(OS),Windows_NT)
	del *.bin, *.srec
endif

