# BdmDrivers

CPU2driverhost is one giant sh!t dump, you have been warned ;)
cpu2.c/h contains everything you need to understand how the driver works. The rest is emberasing  to even look at.

It resides @0x81C0 so the sram has to be configured to start @ 0x80000 (Default)

It supports three commands that you have to store in D0 before starting the loader:
3: Set up hardware for the host. 2: Format flash, 1: Write flash.

In return it will either store 1 if the task was successful or 0 if something failed in D0 before it enters bdm again.

The buffer for data to be written starts @0x80000.
First four bytes should contain address to write and the rest, 1024 bytes, is data to be written.

There is no need for you to bother with shadow-switching. just put shadow region above the regular data so that the bin looks like this:
0x00000 - 0x3FFFF: Regular data,
0x40000 - 0x400FF: Shadow data.
and start writing from 0 to 0x400FF

The loader is aware of that shadow is smaller than the buffer and will only write 256 bytes in the last transfer

Stay tuned.. I named it driverS for a reason :)

Status of T5/7/8 loader (currently named t5)
Missing dq5 status so it WILL get stuck if the flash craps out.
SPI @ 4 / MCU @ 16,67 MHz / Host @ 16 MHz.
T5 times:
TN28F010    : 10483ms
CAT28f010   : 10104ms (box 1)
CAT28f010   : 10191ms (box 2)
39sf020  0-4:  5628ms 
39sf020  0-8: 10604ms
T7 times:
29F400      : 11980ms
Got these time by filling the whole flash with 00's, the host(weak AVR) can't keep up when reading files.

