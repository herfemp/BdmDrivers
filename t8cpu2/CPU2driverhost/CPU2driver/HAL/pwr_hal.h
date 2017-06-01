#include <stdint.h>

#if defined(STM32F30X)
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#endif

#ifdef STM32F10X_MD
#include "stm32f10x.h"
#endif



uint32_t Funcs[] = {
#if defined(STM32F30X)
//AHB
		RCC_AHBPeriph_GPIOA, //0
		RCC_AHBPeriph_GPIOB, //1
		RCC_AHBPeriph_GPIOC, //2
		RCC_AHBPeriph_GPIOD, //3
		RCC_AHBPeriph_GPIOE, //4
		RCC_AHBPeriph_GPIOF, //5

		RCC_AHBPeriph_ADC12, //6
		RCC_AHBPeriph_DMA1,  //7

//APB1
		RCC_APB1Periph_TIM2, //8
		RCC_APB1Periph_TIM3, //9
		RCC_APB1Periph_TIM4, //10
		0,					 //11
		0, //TIM6 removed    //12
		RCC_APB1Periph_TIM7, //13

		RCC_APB1Periph_I2C1, //14
		RCC_APB1Periph_I2C2, //15

		RCC_APB1Periph_USB,  //16
		RCC_APB1Periph_CAN1, //17
#endif

#ifdef STM32F10X_MD

//APB2
		RCC_APB2Periph_GPIOA,//0
		RCC_APB2Periph_GPIOB,//1
		RCC_APB2Periph_GPIOC,//2
		RCC_APB2Periph_GPIOD,//3
		RCC_APB2Periph_GPIOE,//4
		RCC_APB2Periph_GPIOF,//5
		//RCC_APB2Periph_GPIOG,
		RCC_APB2Periph_ADC1, //6
		//RCC_APB2Periph_ADC2,
		//RCC_APB2Periph_ADC3,

//AHB
		RCC_AHBPeriph_DMA1,	 //7

//APB1
		RCC_APB1Periph_TIM2, //8
		RCC_APB1Periph_TIM3, //9
		RCC_APB1Periph_TIM4, //10
		0, //RCC_APB1Periph_TIM5, //11
		0, //RCC_APB1Periph_TIM6, //12
		RCC_APB1Periph_TIM7,	  //13

		RCC_APB1Periph_I2C1, //14
		RCC_APB1Periph_I2C2, //15

		RCC_APB1Periph_USB,	 //16
		RCC_APB1Periph_CAN1, //17









/*		RCC_APB2Periph_TIM1,
		RCC_APB2Periph_SPI1,
		RCC_APB2Periph_TIM8,
		RCC_APB2Periph_USART1,

		RCC_APB2Periph_TIM15,
		RCC_APB2Periph_TIM16,
		RCC_APB2Periph_TIM17,
		RCC_APB2Periph_TIM9,
		RCC_APB2Periph_TIM10,
		RCC_APB2Periph_TIM11,
		RCC_APB2Periph_AFIO,*/


#endif

};
