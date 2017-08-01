#ifndef BDM_H_
#define BDM_H_


///< BDM Commands >///

///< Run CPU
#define BDM_GO		0x0c00
///< Reset peripherals
#define BDM_RESET	0x0400

///< D/A reg
#define R_DREG_BDM	0x2180
#define R_AREG_BDM	0x2188
#define W_DREG_BDM	0x2080
#define W_AREG_BDM	0x2088
///< 0-7 = Data Reg
///< 8-F = Address Reg

///< Sysreg
#define R_SREG_BDM	0x2580
#define W_SREG_BDM	0x2480
///< 0 = PC
///< 1 = PCC?

///< 8 = Tempreg A
///< 9 = Fault Address Reg
///< A = Vector Base reg
///< B = Status reg
///< C = User Stack Pointer
///< D = Supervisor Stack Pointer
///< E = SFC
///< F = DFC



///< Misc..
#define NULL_BDM	0x0000
#define PATCH_USRC	0x0800

///< Read
#define READ8_BDM   0x1900
#define READ16_BDM  0x1940
#define READ32_BDM  0x1980
///< Read/Increment address
#define DUMP8_BDM 	0x1D00
#define DUMP16_BDM  0x1D40
#define DUMP32_BDM  0x1D80

///< Write
#define WRITE8_BDM  0x1800
#define WRITE16_BDM 0x1840
#define WRITE32_BDM 0x1880
///< Write/Increment address
#define FILL8_BDM 	0x1C00
#define FILL16_BDM  0x1C40
#define FILL32_BDM  0x1C80

#define SPINull UDR0   = 0; while(!(UCSR0A&(1<<RXC0)))	;
#define EnSPI   UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(1<<UCPHA0)|(1<<UCPOL0); \
                UCSR0B = (1<<RXEN0)|(1<<TXEN0);


#endif /* BDM_H_ */
