#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "HAL/Disk/ff.h"	
#include "config.h"



uint8_t RambaseH;
uint16_t RambaseL;

void bootstrapmcp();

#define U8		unsigned char

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define BOOL uint8_t

///< Address counters. Used globally so REMEMBER to reset them :)
uint8_t  Hadd;
uint16_t Ladd;
uint8_t Blocksize;
uint8_t EndAddr;
void ShowStepAddr(uint8_t Jumpsize);


uint16_t benchtime;

void BruteforceReset();


extern uint8_t MISCMSG[];
extern char Symbols[];
uint8_t waitmsg(uint16_t msgid);
uint8_t FindF_FreeDump();
uint8_t FindS_FreeDump();
uint8_t WriteBufferToFile(uint8_t Nobytes, uint8_t Start);

void ThrowOK();
void DoBDM();
void FlashMCP();
void InitBDMpins();
uint8_t Attn;
uint16_t bdmresp;
uint16_t bdmresp16;
uint16_t bdmresp32;
extern uint8_t StopTarget();
extern uint8_t ResetTarget();
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


uint8_t i8 , l8 , c8, d8, e8, ret;
uint16_t i16, l16, c16;
UINT bw;

FATFS FatFs;
FIL Fil;


void InitLEDs();
void TXLED();
void RXLED();
void LEDsOff();
void timer_IRQ_init(void);
void sleep(uint16_t time);

extern char ADDR_LCD[];

volatile uint8_t LedTimer;
volatile uint16_t SleepTMR;
volatile uint16_t LCDTMR;
volatile uint16_t BenchTmr;
volatile uint16_t Menutmr;


 uint8_t halfbytetoascii(uint8_t ch);