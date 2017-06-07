#include "pwr_hal.h"
#include "HAL.h"

////////////////////////////////////////////////////
//Enable clocks for a particular device          //
//This one is, for now, only functional on stm32//
void EnableClk(uint8_t Func){
#ifdef STM32F10X_MD
		if(Func>=0 && Func<=6)
			RCC_APB2PeriphClockCmd(Funcs[Func], ENABLE);//func 0-6
		if(Func==7)
			RCC_AHBPeriphClockCmd(Funcs[Func] , ENABLE);//func 7
		if(Func>=8)
			RCC_APB1PeriphClockCmd(Funcs[Func], ENABLE);//func 8+
#endif

#ifdef STM32F30X
	if(Func<=7){
		RCC_AHBPeriphClockCmd(Funcs[Func], ENABLE);
	}else if(Func>=8){
		RCC_APB1PeriphClockCmd(Funcs[Func], ENABLE);
	}
#endif
}
//////////////////////////////////////////////////////////
//Sets current clock Divider -For now completely broken//
//Will implement if it's actually needed			  //
void SetClkDivider(uint8_t Func, uint8_t divider){
}
