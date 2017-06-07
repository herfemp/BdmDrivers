/*
 * GPIO.c
 *
 * Created: 1/13/2016 3:08:45 AM
 *  Author: Chriva
 */ 

#include "GPIO.h"
#include "../common.h"


#ifdef AVR
void SetPinDir(uint8_t port, uint8_t pin, uint8_t dir){

	///< Would much, MUCH, rather do it this way but it wastes ram :/
	//if(dir)*dirports[port-1] |= (1<<pin);
	//else	 *dirports[port-1] &=~(1<<pin);
	if(dir==1){
		/***/if (port==1) DDRB |= (1<<pin);
		else if (port==2) DDRC |= (1<<pin);
		else if (port==3) DDRD |= (1<<pin);
	}else{
		/***/if (port==1) DDRB &=~(1<<pin);
		else if (port==2) DDRC &=~(1<<pin);
		else if (port==3) DDRD &=~(1<<pin);
	}if(dir==2){
		/***/if (port==1) PORTB &=~(1<<pin); 
		else if (port==2) PORTC &=~(1<<pin);
		else if (port==3) PORTD &=~(1<<pin);
	}
}


///< Wasted 2 bytes on AVR by creating this function.. 
uint8_t ReadPin(uint8_t port, uint8_t pin){

	/***/if (port==1)return PINB & _BV(pin) ? 1: 0;
	else if (port==2)return PINC & _BV(pin) ? 1: 0;
	else if (port==3)return PIND & _BV(pin) ? 1: 0;
	else return 0;

}



void WritePin(uint8_t port, uint8_t pin, uint8_t value){

	if(value){
		/***/if (port==1) PORTB |= (1<<pin);
		else if (port==2) PORTC |= (1<<pin);
		else if (port==3) PORTD |= (1<<pin);
	}else{
		/***/if (port==1) PORTB &=~(1<<pin);
		else if (port==2) PORTC &=~(1<<pin);
		else if (port==3) PORTD &=~(1<<pin);
	}
}
#else


////////////////////////////////////////////////
//Sets pin direction					     //
//It's also an oddball since it can be used //
//to set special pin functions on NPX ports//
//void PinSetDir(uint32_t portNum, uint32_t bitValue, uint8_t dir, uint8_t Special){
void SetPinDir(uint8_t port, uint8_t pin, uint8_t dir){
	uint8_t Special =0;
if (!Special){

	if(dir==1){		///< Output, push pull
#if defined(STM32F10X_MD) || defined(STM32F30X)
		GPIO_InitStructure.GPIO_Pin = (PINs[pin]);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

#ifdef STM32F30X
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
#endif
#endif

	}else if(dir==0){///< Input, no pull
#if defined(_LPC17XX_) || defined(_LPC23XX_)
		PINSEL_SetResistorMode(port, pin,	PINSEL_PINMODE_TRISTATE);
#endif
#if defined(STM32F10X_MD) || defined(STM32F30X)
		GPIO_InitStructure.GPIO_Pin = (PINs[pin]);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#ifdef STM32F30X
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
#endif
#endif

	}else if(dir==2){///< Input, pulldown
#if defined(_LPC17XX_) || defined(_LPC23XX_)
		PINSEL_SetResistorMode(port, pin,	PINSEL_PINMODE_PULLDOWN);
#endif
#if defined(STM32F10X_MD) || defined(STM32F30X)
		GPIO_InitStructure.GPIO_Pin  = (PINs[pin]);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
#ifdef STM32F30X
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
#endif
#endif

	}else if (dir==3){///< Input, pullup
#if defined(_LPC17XX_) || defined(_LPC23XX_)
		PINSEL_SetResistorMode(port, pin,	PINSEL_PINMODE_PULLUP);
#endif
#if defined(STM32F10X_MD) || defined(STM32F30X)
	    GPIO_InitStructure.GPIO_Pin = (PINs[pin]);
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
#ifdef STM32F30X
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
#endif
#endif
	}else if (dir==4){///< Input, Analouge
#if defined(STM32F10X_MD) || defined(STM32F30X)
	    GPIO_InitStructure.GPIO_Pin = (PINs[pin]);
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
#ifdef STM32F30X
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
#endif
#endif
	}
#if defined(STM32F10X_MD) || defined(STM32F30X)
	GPIO_InitStructure.GPIO_Speed = Gpio_speed;
	GPIO_Init(((GPIO_TypeDef *) (GPIO_BADDR + (port*GPIO_ADDRJMP))), &GPIO_InitStructure);
#endif
#if defined(_LPC17XX_) || defined(_LPC23XX_)
	GPIO_SetDir(port, (1 << pin), dir);
#endif
	}else{///< Special mode. This will take forever to implement on stm32 ;)
#if defined(STM32F10X_MD) || defined(STM32F30X)
		///< TODO: Implement _REAL_ special handlers.
	    GPIO_InitStructure.GPIO_Speed = Gpio_speed;
	    GPIO_InitStructure.GPIO_Pin   = (PINs[pin]);
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
		#ifdef STM32F30X
	    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
#endif
#else
		PINSEL_SetPinFunc(port, pin, Special); ///< I wish things were always this easy.
#endif
#if defined(STM32F10X_MD) || defined(STM32F30X)
		GPIO_Init(((GPIO_TypeDef *) (GPIO_BADDR + (port*GPIO_ADDRJMP))), &GPIO_InitStructure);
#endif
	}
}

////////////////////////////////////////////
//Writes to a pin						 //
void WritePin(uint8_t port, uint8_t pin, uint8_t value){
//void WritePin(uint32_t portNum, uint32_t bitValue, uint8_t bit){
#if defined(STM32F10X_MD) || defined(STM32F30X)
	if(value)SETBIT(((GPIO_TypeDef *) (GPIO_BADDR + (port*GPIO_ADDRJMP)))->FIOSET,  (PINs[pin]));
	else	 SETBIT(((GPIO_TypeDef *) (GPIO_BADDR + (port*GPIO_ADDRJMP)))->FIOCLR,  (PINs[pin]));
#else
	if(value)SETBIT(((GPIO_TypeDef *) (GPIO_BADDR + (port*GPIO_ADDRJMP)))->FIOSET,  (1<<pin)); //TODO: Implement the same, neat, code here too.
	else	 SETBIT(((GPIO_TypeDef *) (GPIO_BADDR + (port*GPIO_ADDRJMP)))->FIOCLR,  (1<<pin));
#endif

}


////////////////////////////////////////////
//Reads a pin							 //
uint8_t ReadPin(uint32_t portNum, uint32_t bitValue){
#if defined(STM32F10X_MD) || defined(STM32F30X)
	return CHECKBIT(((GPIO_TypeDef *) (GPIO_BADDR + (portNum*GPIO_ADDRJMP)))->FIOPIN,  (PINs[bitValue]));
#else
	return CHECKBIT(((GPIO_TypeDef *) (GPIO_BADDR + (portNum*GPIO_ADDRJMP)))->FIOPIN,  (1<<bitValue)); //TODO: Implement the same, neat, code here too.
#endif
}

#endif


















