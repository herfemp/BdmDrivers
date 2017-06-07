#include "common.h"
 

///< Convert binary numbers into ascii chars.
void something(uint16_t input){

	printnumber[4] = input%10;
	input = input/10;
	 
	printnumber[3] = input%10;
	input = input/10;
	printnumber[2] = input%10;
	input = input/10;
	printnumber[1] = input%10;
	input = input/10;

	printnumber[0] = input%10;
	printnumber[0] = printnumber[0] | 0x30;
	printnumber[1] = printnumber[1] | 0x30;
	printnumber[2] = printnumber[2] | 0x30;
	printnumber[3] = printnumber[3] | 0x30;
	printnumber[4] = printnumber[4] | 0x30;
}

void showval(uint16_t val){
	 
	something(val);
	lcd_goto(0x40);
	lcd_puts("Time", 0);
	lcd_goto(0x45);
	lcd_puts(printnumber,5);
}



// Stupid function
void clrprintlcd(const char *s){

	lcd_clrscr();
	lcd_puts(s, 0);
}

uint8_t nibbletetoascii(uint8_t ch){

	if((ch&0xF)>=0x0&&(ch&0xF)<=0x09) return ((ch&0xF)+0x30);
	else                              return ((ch&0xF)+0x37);
}

void ShowAddr(uint8_t Had, uint16_t Lad){

	char ADDR_LCD[5];
	ADDR_LCD[0]=nibbletetoascii(Had);
	ADDR_LCD[1]=nibbletetoascii(Lad>>12&0xF);
	ADDR_LCD[2]=nibbletetoascii(Lad>>8&0xF);
	ADDR_LCD[3]=nibbletetoascii(Lad>>4&0xF);
	ADDR_LCD[4]=nibbletetoascii(Lad&0xF);
	lcd_goto(5);
	lcd_puts(ADDR_LCD,5);
}