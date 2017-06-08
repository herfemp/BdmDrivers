cd out
 ..\..\utils\bin2srec  -o 0x400 TxDriver.bin > TxDriver.srec
 ..\..\avrdude\avrdude -p m2560 -c arduino -P com8 -b 115200 -U flash:w:TxDriver.srec
cd..