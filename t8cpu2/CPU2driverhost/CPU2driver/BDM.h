/*
 * BDM.h
 *
 * Created: 1/17/2016 4:15:38 PM
 *  Author: Chriva
 */ 


#ifndef BDM_H_
#define BDM_H_

void InitBDMpins();
void PrintPins();



 
 uint16_t CMFIMCR_Enable_shadow;


uint16_t CMFIMCR_Enable;

uint16_t CMFIMCR_Stop;







 uint8_t cpu2_testseq;

///< CMFIMCR
#define CMFIMCRAddr  0xF800
#define CMFITSTAddr  0xF804
#define CMFIBAHAddr  0xF808
#define CMFIBALAddr  0xF80A
#define CMFICTL1Addr 0xF80C
#define CMFICTL2Addr 0xF80E

uint16_t blockbuf [32];
void PrepTrionic();






///< BDM Commands >///

///< Run CPU
#define BDM_GO		0x0c00
///< Reset peripherals
#define BDM_RESET	0x0400


void Printregstats(uint8_t stage);

///< D/A reg
#define R_DREG_BDM	0x2180
#define W_DREG_BDM	0x2080
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



#ifndef AVR
extern uint8_t flashDriver[];
#endif

extern uint8_t BDM_DEL;





#endif /* BDM_H_ */
