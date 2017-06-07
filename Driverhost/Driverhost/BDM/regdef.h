#ifndef REGDEF_H_
#define REGDEF_H_


///< Trionic 8 CPU2 (MCP)
uint16_t CMFIMCR_Enable_shadow;
uint16_t CMFIMCR_Enable;
uint16_t CMFIMCR_Stop;

///< CMFIMCR
#define CMFIMCRAddr  0xF800
#define CMFITSTAddr  0xF804
#define CMFIBAHAddr  0xF808
#define CMFIBALAddr  0xF80A
#define CMFICTL1Addr 0xF80C
#define CMFICTL2Addr 0xF80E

///< External clock @ 4 MHz
#define MCPClock 0xE000
/*  1 << 15 | // X (Divide by 2=0)
    6 << 12 | // W (Multiplier) ///< It did NOT like 7 so even 6 is probably a bit too much.
    0 <<  8 | // Y (Divider)
    0 <<  7 | // EDIV (Divide by 16=1)
    0 <<  5   // LOSCD */


///< Trionic 8 CPU1 (Main)
#define DPTMCR_Base 0xF680
#define FASRAM_Base 0xF6C0
#define TPU_Base    0xF800






#endif /* REGDEF_H_ */