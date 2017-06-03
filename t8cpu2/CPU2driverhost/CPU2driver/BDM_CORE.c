/*
 * BDM.c
 *
 * Created: 1/17/2016 1:27:51 PM
 *  Author: Chriva
 */

#include "common.h"
#include "HAL/HAL.h"
#include "BDM.h"

#ifdef AVR
#include <avr/io.h>

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#endif

uint8_t BDM_DEL = 10;

void InitBDMpins(){

	UBRR0H = 0;
	UBRR0L = 15; /* This should be set after activating usart spi.. meh. who cares. it works */

	UCSR0C = 0;
	UCSR0B = 0;

	SetPinDir(P_RST, 0); ///< Set all of them to floating inputs
	SetPinDir(P_BKPT,0);
	SetPinDir(P_FRZ, 2); ///< Except this fella; Input, pull down
	SetPinDir(P_DSI, 0);
	SetPinDir(P_DSO, 0);
	
}



uint8_t ResetTarget(){

	InitBDMpins();

	SetPinDir(P_RST, 1);
	WritePin(P_RST,1);
	sleep(50);
	WritePin(P_RST,0);

	SetPinDir(P_RST, 0);
	///< Give it up to 500~ ms to wake up
	MiscTime=500;
	while(!ReadPin(P_RST) && MiscTime)	;

	sleep(10);
	// SetPinDir(P_RST, 0);

	return ReadPin(P_RST);
}


	
uint8_t StopTarget(){

	//ResetTarget();
	//SetPinDir(P_RST, 1);
	SetPinDir(P_BKPT,1);
	SetPinDir(P_DSI, 1);

	WritePin(P_BKPT,1);
	WritePin(P_BKPT,0);

	///< Give it up to 500~ ms to hit the brakes..
	MiscTime=500;
	while(!ReadPin(P_FRZ) &&  MiscTime)	;

	///< It appears someone is stubborn..  STOP!
	if(!ReadPin(P_FRZ)){
		WritePin(P_RST,0);
		sleep(100);
		WritePin(P_RST,1);

		MiscTime=500;
		while(!ReadPin(P_FRZ) &&  MiscTime)	;
	}
	sleep(10);
	return ReadPin(P_FRZ);
}


///< SPI may be pretty effing fast, but it has a few drawbacks. This is a fallback used during certain conditions to circumvent problems.
void ShiftData_s(uint16_t package){

	uint8_t i;

	Attn=0;
	for (i=17; i>0; i--){
		WritePin(P_BKPT, 0);
		if(i<17){
			WritePin(P_DSI, (package>>(i-1))&0x1 );
			bdmresp<<=1;  bdmresp|= ReadPin(P_DSO);
			}else{
			WritePin(P_DSI, 0);
			Attn=ReadPin(P_DSO);
		}
		WritePin(P_BKPT, 1);
		#ifndef AVR  ///< AVR does not need this loop
		for(d=0; d<2; d++){;} ///<
		#endif
	}

}


void Exec_WriteCMD_s(uint16_t AddrH, uint16_t AddrL, uint16_t cmd, uint16_t DataH, uint16_t DataL){

	ShiftData_s(cmd);
	
	if(AddrH || AddrL){
		ShiftData_s(AddrH);
		ShiftData_s(AddrL);
	}
	///< Bit 7 set; Send two payloads
	if(cmd&0x80)
		ShiftData_s(DataH);
	ShiftData_s(DataL);

	///< Fault checks, what is that? Time will tell if there is enough space left. Not gonna touch it yet..
	do{ ShiftData_s(0); }while(Attn);

}




/* Hardware-assisted functions */ 
void disenablespi(uint8_t onoff){

	if(onoff){
		UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(1<<UCPHA0)|(1<<UCPOL0);
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	}else{
		UCSR0C = 0;
		UCSR0B = 0;
	}
}

uint8_t SendRecSPI2(uint8_t dt){

	while(!(UCSR0A&(1<<UDRE0)))	;
	UDR0 = dt;
	while(!(UCSR0A&(1<<RXC0)))	;
	
	return UDR0&0xff;
}


inline void ShiftWait(){
	

	do{	PORTD &=~(1<<4 | 1<<1);				// Pull down Clock and Data out
		disenablespi(1);					// Enable SPI
		if(!(PIND & _BV(0) ? 1: 0)) break;	// Attention-bit not set, abort loop and fetch our valuable data.
		SendRecSPI2(0);						// Clock out garbage
		SendRecSPI2(0);
		disenablespi(0);					// kill SPI
	}while (1);

	bdmresp = SendRecSPI2(0) <<8;
	bdmresp+= SendRecSPI2(0);
	disenablespi(0); // kill SPI
	
}

inline void ShiftData(uint16_t package){

	PORTD &=~(1<<4 | 1<<1);

	Attn = PIND & _BV(0) ? 1: 0;
	
	disenablespi(1);

	bdmresp = (SendRecSPI2( package>>8&0xFF )) <<8;
	bdmresp+= (SendRecSPI2(package&0xFF));
	
	disenablespi(0);

}




inline void Exec_WriteCMD_workaround(uint16_t AddrH, uint16_t AddrL, uint16_t cmd, uint16_t DataH, uint16_t DataL){

	ShiftData(cmd);
	
	ShiftData(AddrH);
	ShiftData(AddrL);

	if(cmd&0x80)
		ShiftData(DataH);
	ShiftData(DataL);


	do{ ShiftData(0);
	}while(Attn);

}


inline void Exec_ReadCMD_workaround(uint16_t AddrH, uint16_t AddrL, uint16_t cmd){

	ShiftData(cmd);
	//if(AddrH || AddrL){
		ShiftData(AddrH);
		ShiftData(AddrL);
	//}
	if(cmd&0x80){
		do{ ShiftData(0);
		}while(Attn);
		
		bdmresp32=bdmresp;
	}

	do{ ShiftData(0);
	}while(Attn);

	bdmresp16=bdmresp;

}

inline void Exec_DumpCMD(){

	ShiftData(DUMP32_BDM);
	
	ShiftWait();
	bdmresp32=bdmresp;

	ShiftWait();
	bdmresp16=bdmresp;
}


// Very weird functions for fill32..
// This one sends bytes in the wrong order
void SendRecSPInoresp(uint16_t dt){

	while(!(UCSR0A&(1<<UDRE0)))	;
	UDR0 = dt&0xFF;
	while(!(UCSR0A&(1<<RXC0)))	;
	uint8_t temp = UDR0; // This register must be read..

	while(!( UCSR0A&(1<<UDRE0)))	;
	UDR0 = (dt>>8)&0xFF;
	while(!( UCSR0A&(1<<RXC0)))	;
	temp = UDR0; // This register must be read..

	temp = temp; // Aaaand kill that annoying warning!
}
inline void ShiftData_p(const uint16_t *package){

	PORTD &=~(1<<4 | 1<<1);
	UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(1<<UCPHA0)|(1<<UCPOL0);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	SendRecSPInoresp(*package);
	UCSR0C = 0;
	UCSR0B = 0;
}
// Store four bytes and let the ECU do the address-counting. Do not fetch response.
inline void Exec_FillCMD_p(const uint16_t *data){

	ShiftData(FILL32_BDM);
	ShiftData_p(&data[0]);
	ShiftData_p(&data[1]);
	
	uint8_t done = 0;
	do{	PORTD &=~(1<<4 | 1<<1);               // Pull down Clock and Data out
		UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(1<<UCPHA0)|(1<<UCPOL0);
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);                      // Enable SPI
		if(!(PIND & _BV(0) ? 1: 0)) done = 1; // Attention-bit not set, abort loop and fetch our valuable data.
		SendRecSPInoresp(0);                  // Clock out garbage
		UCSR0C = 0;
		UCSR0B = 0;                    // kill SPI
	}while (!done);
}

///<
inline void Exec_WriteCMD(uint16_t AddrH, uint16_t AddrL, uint16_t cmd, uint16_t DataH, uint16_t DataL){

	ShiftData(cmd);
	
	if(AddrH || AddrL){
		ShiftData(AddrH);
		ShiftData(AddrL);
	}

	///< Bit 7 set; Send two payloads
	if(cmd&0x80)
		ShiftData(DataH);
	ShiftData(DataL);

	do{ ShiftData(0);
	}while(Attn);


//	ShiftWait();


}


inline void Exec_ReadCMD(uint16_t AddrH, uint16_t AddrL, uint16_t cmd){

	ShiftData(cmd);

	if(AddrH || AddrL){
		ShiftData(AddrH);
		ShiftData(AddrL);
	}

	///< One could easily get stuck in a never ending loop here. -Worth wasting precious space?
	///< Send empty packages while the ECU is processing stuff.

	///<  Attn=0, bdmresp=0x0    Valid data transfer.
	///<  Attn=0, bdmresp=0xFFFF Command completed.
	///<  Attn=1, bdmresp=0x0    Not ready, come again. (The code is currently written to assume this is the only response)
	///<  Attn=1, bdmresp=0x1    Bus error.
	///<  Attn=1, bdmresp=0xFFFF You're doing things wrong, you buffoon! (Seen that a couple of times ;) )

	///< Bit 7 set; Receive higher part
	if(cmd&0x80){
		ShiftWait();
		bdmresp32=bdmresp;
	}

	ShiftWait();
	bdmresp16=bdmresp;
}



