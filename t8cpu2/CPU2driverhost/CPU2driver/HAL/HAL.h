/*
 * HAL.h
 *
 * Created: 1/13/2016 3:07:07 AM
 *  Author: Chriva
 */ 
#include <stdint.h>


#ifndef HAL_H_
#define HAL_H_

#define UARTRXINT 10
#define CANRXINT  11


///< Interrupts
void DisableInt(uint8_t intr);
void EnableInt(uint8_t intr);
void EnableGlobalInt();
void DisableGlobalInt();
void EnableWDT();
void DisableWDT();

///< GPIO
void SetPinDir(uint8_t port, uint8_t pin, uint8_t dir);
uint8_t ReadPin(uint8_t port, uint8_t pin);
void WritePin(uint8_t port, uint8_t pin, uint8_t value);

///< UART
void UARTWaitReturn(uint8_t Retval);
uint8_t UARTTimedReturn();

///< CAN
uint8_t CANMsgInBuffer(uint8_t Interface);
#ifndef AVR
void InitCAN(uint8_t interface, uint64_t BTR);
#else
void InitCAN(uint8_t interface, float BTR);
#endif
void SetCANFilter(uint8_t interface, uint8_t filterbank);


///< SPI
uint8_t SendRecSPI(uint8_t dt);
void InitSPI();






#if !defined(STM32F10X_MD) && !defined(STM32F30X) && !defined(AVR)
#include "../../NXP/lpc17xx_lib/lpc17xx_pinsel.h"
#include "../../NXP/lpc17xx_lib/lpc17xx_clkpwr.h"
#include "../../NXP/lpc17xx_lib/lpc17xx_pinsel.h"
#include "../../NXP/lpc17xx_lib/lpc17xx_gpio.h"
#include "../../NXP/lpc17xx_lib/lpc17xx_timer.h"
#include "../../NXP/lpc17xx_lib/lpc17xx_i2c.h"
#include "../../NXP/lpc17xx_lib/lpc17xx_adc.h"
#include "../../NXP/lpc17xx_lib/lpc17xx_ssp.h"
#endif

#if defined(STM32F30X)
#include "stm32f30x_adc.h"
#endif

#ifdef _LPC23XX_
#include "../../NXP/ARM7/ARM7_lib/ARM7_lib.h"
//Various Interrupt handlers for LPC23XX
void TIMER0_IRQHandler (void)  __attribute__ ((interrupt("IRQ")));
void TIMER1_IRQHandler (void)  __attribute__ ((interrupt("IRQ")));
void TIMER2_IRQHandler (void)  __attribute__ ((interrupt("IRQ")));
void TIMER3_IRQHandler (void)  __attribute__ ((interrupt("IRQ")));
void USBHwISR 		   (void)  __attribute__ ((interrupt("IRQ")));
void CAN_IRQHandler    (void)  __attribute__ ((interrupt("IRQ")));

#endif

#ifdef STM32F10X_MD






#endif









// More interrupt related functions
void InstallINT(uint32_t INT, uint32_t prio);
void UnInstallINT(uint32_t INT, uint32_t prio);
void ClearINT(uint32_t INT);
void EnableINT(uint32_t INT);
void DisableINT(uint32_t INT);


// GPIO / Clocks
void EnableClk(uint8_t Func);
void SetClkDivider(uint8_t Func, uint8_t divider);
void PinSetDir(uint32_t portNum, uint32_t bitValue, uint8_t dir, uint8_t Special);
//uint8_t ReadPin(uint32_t portNum, uint32_t bitValue);
//void WritePin(uint32_t portNum, uint32_t bitValue, uint8_t bit);
void WritePort(uint32_t portNum, uint32_t bitValue, uint32_t port);

// Timer related
void Install_Timer(uint32_t ms, uint32_t timer, uint32_t prio, uint8_t mtchstop);
void ResetTimer(uint32_t Timer);
void StartTimer(uint32_t Timer);
void StopTimer(uint32_t Timer);
void ClearTimerINT(uint32_t INT);
void DWT_Init();



// USB
#ifdef _LPC17XX_
#define USB_DV_INT_EN	USB->USBDevIntEn
#define USB_DEV_INT		USB->USBDevIntSt
#define USB_EP_INT		USB->USBEpIntSt
#define USB_EP_INT_		CLRUSB->USBEpIntClr
#define USB_INT_CLR   	USB->USBDevIntClr

#define USB_CMD_CD		USB->USBCmdCode
#define USB_CMD_DAT		USB->USBCmdData
#define USB_RLZ_EP		USB->USBReEP
#define USB_EP_INDX		USB->USBEpInd
#define USB_MAX_SZ		USB->USBMaxPSize
#define USB_ENDP_EN		USB->USBEpIntEn
#define USB_DV_CTRL		USB->USBCtrl
#define USB_RXP_LEN		USB->USBRxPLen
#define USB_TXP_LEN		USB->USBTxPLen
#define USB_IO_RXD		USB->USBRxData
#define USB_IO_TXD		USB->USBTxData

#define USB_PCON		SC->PCONP

#define USB_INT_PRIO	USB->USBDevIntPr
#define USB_EP_PRIO		USB->USBEpIntPri
#define USB_CLK_CTRL	OTG_CLK_CTRL
#define USB_CLK_STAT	OTG_CLK_STAT
#endif

#ifdef _LPC23XX_
#define USB_DV_INT_EN	DEV_INT_EN
#define USB_DEV_INT		DEV_INT_STAT
#define USB_EP_INT		EP_INT_STAT
#define USB_EP_INT_CLR	EP_INT_CLR
#define USB_INT_CLR		DEV_INT_CLR

#define USB_CMD_CD		CMD_CODE
#define USB_CMD_DAT		CMD_DATA
#define USB_RLZ_EP		REALIZE_EP
#define USB_EP_INDX		EP_INDEX
#define USB_MAX_SZ		MAXPACKET_SIZE
#define USB_ENDP_EN		EP_INT_EN
#define USB_DV_CTRL		USB_CTRL
#define USB_RXP_LEN		RX_PLENGTH
#define USB_TXP_LEN		TX_PLENGTH
#define USB_IO_RXD		RX_DATA
#define USB_IO_TXD		TX_DATA

#define USB_PCON		PCONP

#define USB_INT_PRIO	DEV_INT_PRIO
#define USB_EP_PRIO		EP_INT_PRIO
#define USB_CLK_CTRL	USBClkCtrl
#define USB_CLK_STAT	USBClkSt

#endif

// CAN


// ADC
void HAL_InitADC(uint32_t FirstChan, uint8_t numchan, uint32_t Freq);
void SetADCDivider(uint8_t divider);
void HAL_ADC_CMD(uint32_t port, uint8_t chan, uint8_t mode);
void HAL_ADC_StartConv(uint32_t port, uint8_t mode);
void HAL_ADC_StopConv(uint32_t port);
uint8_t HAL_ADC_DNE(uint32_t port, uint8_t chan);
uint32_t HAL_GET_ADC_DTA(uint8_t port, uint8_t chan);

// I2C
void I2C_Setup();
uint16_t I2C_ByteRead(uint8_t RegAddr, uint8_t* Buffer, uint8_t nBytes);
uint16_t I2C_ByteWrite(uint8_t RegAddr, const uint8_t* Buffer, uint8_t nBytes);
/*
bool UNIPRF_WriteEEPROM(uint8_t addr, uint32_t src_len, const BYTE* src);
bool UNIPRF_ReadEEPROM(BYTE* dst, uint32_t dst_len, uint8_t addr);*/




















#endif /* HAL_H_ */
