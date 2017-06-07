cd out
 ..\..\utils\bin2srec  -o 0x400 T5Driver.bin > t5drv.srec
 ..\..\avrdude\avrdude -p m2560 -c arduino -P com8 -b 115200 -U flash:w:t5drv.srec
cd..