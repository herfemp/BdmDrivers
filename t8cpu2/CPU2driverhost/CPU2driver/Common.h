#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "HAL/Disk/ff.h"

uint16_t benchtime;
UINT bw;
FATFS FatFs;
FIL Fil;

uint8_t Attn;
uint16_t bdmresp;
uint16_t bdmresp16;
uint16_t bdmresp32;

extern char ADDR_LCD[];

volatile uint16_t SleepTMR;
volatile uint16_t LCDTMR;
volatile uint16_t BenchTmr;
volatile uint16_t Menutmr;

uint8_t FlashMCP();
void InitBDMpins();

uint8_t StopTarget();
uint8_t ResetTarget();
void ShiftData(uint16_t package);

void Exec_DumpCMD();
void Exec_FillCMD(uint16_t DataH, uint16_t DataL);
void Exec_ReadCMD( uint16_t AddrH, uint16_t AddrL, uint16_t cmd);
void Exec_WriteCMD(uint16_t AddrH, uint16_t AddrL, uint16_t cmd, uint16_t DataH, uint16_t DataL);
void Exec_ReadCMD_workaround( uint16_t AddrH, uint16_t AddrL, uint16_t cmd);
void Exec_WriteCMD_workaround(uint16_t AddrH, uint16_t AddrL, uint16_t cmd, uint16_t DataH, uint16_t DataL);
void Exec_ReadCMD_s( uint16_t AddrH, uint16_t AddrL, uint16_t cmd);
void Exec_WriteCMD_s(uint16_t AddrH, uint16_t AddrL, uint16_t cmd, uint16_t DataH, uint16_t DataL);

void PrepT();
void ShiftData_s(uint16_t package);
void ShowAddr(uint8_t Had, uint16_t Lad);
void timer_IRQ_init(void);
void sleep(uint16_t time);
void bootstrapmcp();
uint8_t halfbytetoascii(uint8_t ch);


#define DSPLINES  2


#define P_RST  3, 2
#define P_BKPT 3, 4
#define P_FRZ  3, 7
#define P_DSI  3, 1
#define P_DSO  3, 0


#define F_CPU 16000000UL

#define SDCARD_CS_PORT          1
#define SDCARD_CS_PIN           0



#define	MCP2515_CS_1			1,2
#define	MCP2515_INT_1			B,1



#define LCD_DB4_PORT            2
#define LCD_DB5_PORT            2
#define LCD_DB6_PORT            2
#define LCD_DB7_PORT            2
#define LCD_DB4_PIN             0
#define LCD_DB5_PIN             1
#define LCD_DB6_PIN             2
#define LCD_DB7_PIN             3

#define LCD_E_PORT              2
#define LCD_RS_PORT             3
#define LCD_E_PIN               5
#define LCD_RS_PIN              3