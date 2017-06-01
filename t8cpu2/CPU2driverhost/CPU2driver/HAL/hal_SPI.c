/*
 * SPI.c
 *
 * Created: 1/13/2016 3:08:14 AM
 *  Author: Chriva
 */ 


#include <stdint.h>
#include <stdio.h>
#include "../config.h"

#ifdef AVR
#include <avr/io.h>
#endif
//#include "HAL.h"

#ifdef STM32F103RB
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#endif


uint8_t SendRecSPI(uint8_t dt){
#ifdef STM32F103RB
	while (SPI_I2S_GetFlagStatus(SPIz, SPI_I2S_FLAG_TXE) == RESET)	;
	SPI_I2S_SendData(SPIz, (uint8_t) dt);
	while (SPI_I2S_GetFlagStatus(SPIz, SPI_I2S_FLAG_RXNE) == RESET)	;
	return (uint8_t) SPI_I2S_ReceiveData(SPIz);
#endif
#ifdef AVR
	SPDR = dt;
	while( !( SPSR & (1<<SPIF) ) )
		;
	return SPDR;

#endif


}

#ifdef STM32F103RB



void RCC_Configuration(void);
void GPIO_Configuration(uint16_t SPIz_Mode);


SPI_InitTypeDef   SPI_InitStructure;
void inithw(){
	RCC_Configuration();

    GPIO_Configuration(SPI_Mode_Master);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPIz, &SPI_InitStructure);
    SPI_Cmd(SPIz, ENABLE);

}

#endif




void InitSPI(){

#ifdef STM32F103RB
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	inithw();

#endif

}


#ifdef STM32F103RB

void RCC_Configuration(void){
	RCC_APB2PeriphClockCmd(SPIz_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(SPIz_CLK, ENABLE);
	if(DEBUGSTTS) printf("SPI_RCC..\n\r");
}


void GPIO_Configuration(uint16_t SPIz_Mode){
	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SPIz_PIN_SCK | SPIz_PIN_MOSI;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPIz_GPIO, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SPIz_PIN_MISO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(SPIz_GPIO, &GPIO_InitStructure);
    if(DEBUGSTTS) printf("SPI_CS..\n\r");
}


#endif





