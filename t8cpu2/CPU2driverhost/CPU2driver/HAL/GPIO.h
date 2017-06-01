/*
 * GPIO.h
 *
 * Created: 1/13/2016 4:30:08 AM
 *  Author: Chriva
 */ 


#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

///< Hope you don't mind, J.
#define SETBIT(x, y) 		(x |= (y))
#define CHECKBIT(x, y) 		(((x) & (y)) == (y))

#ifdef AVR
#include <avr/io.h>



///< Reminder. Used 1864 bytes of ram before the HAL conversion
///< Really tried to keep mem usage down.. but 12 bytes wasted :/
//volatile uint8_t *dirports[]={ &DDRB,  &DDRC,  &DDRD  };
//volatile uint8_t *pinports[]={ &PORTB, &PORTC, &PORTD };

#endif




#ifdef STM32F10X_MD
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#define GPIO_Mode_AF		GPIO_Mode_AF_PP
#define GPIO_Mode_AN		GPIO_Mode_IN_FLOATING
#endif



#if defined(STM32F10X_MD) || defined(STM32F30X)

//Some lazy defines..
#define FIOSET ODR
#define FIOPIN IDR
#define FIOCLR BRR
#endif

#ifdef STM32F30X
//Yet more of those.. ;)
#define GPIO_Mode_Out_PP		GPIO_Mode_OUT
#define GPIO_Mode_IPD			GPIO_Mode_IN
#define GPIO_Mode_IPU			GPIO_Mode_IN
#define GPIO_Mode_IN_FLOATING	GPIO_Mode_IN
#endif //STM32F30X

#if defined(STM32F10X_MD) || defined(STM32F30X)
uint32_t PINs[] = {
		GPIO_Pin_0,
		GPIO_Pin_1,
		GPIO_Pin_2,
		GPIO_Pin_3,
		GPIO_Pin_4,
		GPIO_Pin_5,
		GPIO_Pin_6,
		GPIO_Pin_7,
		GPIO_Pin_8,
		GPIO_Pin_9,
		GPIO_Pin_10,
		GPIO_Pin_11,
		GPIO_Pin_12,
		GPIO_Pin_13,
		GPIO_Pin_14,
		GPIO_Pin_15,
		GPIO_Pin_All,
};
#endif


#ifdef _LPC23XX_

#include "../../NXP/lpc17xx_lib/lpc17xx_pinsel.h"
#include "../../NXP/lpc17xx_lib/lpc17xx_gpio.h"

#define GPIO_BADDR		0x3FFFC000UL
#define GPIO_ADDRJMP	0x20

#endif




#if defined(STM32F30X) || defined(STM32F10X_MD)

#if defined(STM32F30X)
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"

#define GPIO_BADDR		AHB2PERIPH_BASE
#endif

#ifdef STM32F10X_MD
#include "stm32f10x_conf.h"
#include "stm32f10x.h"

#define GPIO_BADDR	GPIOA_BASE
#endif


GPIO_InitTypeDef GPIO_InitStructure;


#define GPIO_ADDRJMP	0x400

#endif









#endif /* GPIO_H_ */
