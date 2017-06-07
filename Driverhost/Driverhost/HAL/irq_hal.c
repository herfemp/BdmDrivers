//-----------------------------------------------------------------------------
//	CAN/BDM adapter firmware
//	(C) Janis Silins, 2010
//	$id$
//-----------------------------------------------------------------------------
//Further hacked and dashed, most certainly also ruined, by Christian Ivarsson.


#include "../common.h"
#include "HAL.h"
#include "irq_hal.h"




//////////////////////////////////////////////////////////////////////////////////////
//Use any of these with EXTREME caution. Check table in irq_hal.h before using them//
//Seriously -I mean it! ;) 														  //



//Clears an interrupt
void ClearINT(uint32_t INT){
#ifdef _LPC17XX_
	NVIC_ClearPendingIRQ(Interrupts[INT]);
#endif
#ifdef _LPC23XX_
	VICVectAddr = 0 << VICs[INT];
#endif
}

//Installs / activates an interrupt source.
//On lpc23xx it also takes care of linking those interrupts to their corresponding handlers.
void InstallINT(uint32_t INT, uint32_t prio){
#ifdef _LPC17XX_
	NVIC_EnableIRQ(Interrupts[INT]);
#endif
#ifdef _LPC23XX_
	install_irq((VICs[INT]),   (void *)(Handlers[INT])    , prio);
#endif
#if defined(STM32F10X_MD) || defined(STM32F30X)
	NVIC_InitStructure.NVIC_IRQChannel = Interrupts[INT];
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = prio;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}


//Un-Installs / de-activates an interrupt source.
//On lpc23xx it also takes care of linking those interrupts to their corresponding handlers.
void UnInstallINT(uint32_t INT, uint32_t prio){
#ifdef _LPC17XX_
	//NVIC_EnableIRQ(Interrupts[INT]);
#endif
#ifdef _LPC23XX_
	//install_irq((VICs[INT]),   (void *)(Handlers[INT])    , prio);
#endif
#if defined(STM32F10X_MD) || defined(STM32F30X)
	NVIC_InitStructure.NVIC_IRQChannel = Interrupts[INT];
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = prio;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}


//Disables an interrupt.
void DisableINT(uint32_t INT){
#ifdef _LPC17XX_
	NVIC_DisableIRQ(Interrupts[INT]);
#endif
#ifdef _LPC23XX_
	VICIntEnable = 0 << VICs[INT];
#endif
	ClearINT(INT);

}


//Interrupt handlers. Used only on STM32

#if defined(STM32F10X_MD) || defined(STM32F30X)

void NMI_Handler(void){/*UNIPRF_LEDOn(CATASTROPHIC);*/}
void HardFault_Handler(void){while (1){/*UNIPRF_LEDOn(CATASTROPHIC);*/ }}
void MemManage_Handler(void){while (1){/*UNIPRF_LEDOn(CATASTROPHIC);*/}}
void BusFault_Handler(void){while (1){/*UNIPRF_LEDOn(CATASTROPHIC);*/}}
void UsageFault_Handler(void){while (1){/*UNIPRF_LEDOn(CATASTROPHIC);*/}}
void SVC_Handler(void){/*UNIPRF_LEDOn(CATASTROPHIC);*/}
void DebugMon_Handler(void){/*UNIPRF_LEDOn(CATASTROPHIC);*/}
void PendSV_Handler(void){/*UNIPRF_LEDOn(CATASTROPHIC);*/}
void SysTick_Handler(void){}

#ifdef STM32F30X
void USB_LP_IRQHandler(void){
#endif
#ifdef STM32F10X_MD
//void USB_LP_CAN1_RX0_IRQHandler(void){
#endif
//	USB_Istr();
//}


#ifdef STM32F30X
void USB_HP_IRQHandler(void){
#endif
#ifdef STM32F10X_MD
void USB_HP_CAN1_TX_IRQHandler(void){
#endif
//	CTR_HP();
}


#ifdef STM32F30X
void USB_LP_CAN1_RX0_IRQHandler(void){
	while (1){UNIPRF_LEDOn(CATASTROPHIC);}

	//CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
	//CAN_ClearITPendingBit(CAN1, CAN_IT_FF1);
	//CAN_ClearITPendingBit(CAN1, CAN_IT_FOV0);
	//CAN_ClearITPendingBit(CAN1, CAN_IT_FOV1);

/*	CAN_ClearITPendingBit(CAN1, CAN_IT_TME);
	CAN_ClearITPendingBit(CAN1, CAN_IT_WKU);
	CAN_ClearITPendingBit(CAN1, CAN_IT_SLK);
	CAN_ClearITPendingBit(CAN1, CAN_IT_EWG);

	CAN_ClearITPendingBit(CAN1, CAN_IT_EPV);
	CAN_ClearITPendingBit(CAN1, CAN_IT_BOF);
	CAN_ClearITPendingBit(CAN1, CAN_IT_LEC);
	CAN_ClearITPendingBit(CAN1, CAN_IT_ERR);*/

}

void CAN1_RX1_IRQHandler(void){
	while (1){UNIPRF_LEDOn(CATASTROPHIC);}

	//CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
	//CAN_ClearITPendingBit(CAN1, CAN_IT_FF1);
	//CAN_ClearITPendingBit(CAN1, CAN_IT_FOV0);
	//CAN_ClearITPendingBit(CAN1, CAN_IT_FOV1);

/*	CAN_ClearITPendingBit(CAN1, CAN_IT_TME);
	CAN_ClearITPendingBit(CAN1, CAN_IT_WKU);
	CAN_ClearITPendingBit(CAN1, CAN_IT_SLK);
	CAN_ClearITPendingBit(CAN1, CAN_IT_EWG);

	CAN_ClearITPendingBit(CAN1, CAN_IT_EPV);
	CAN_ClearITPendingBit(CAN1, CAN_IT_BOF);
	CAN_ClearITPendingBit(CAN1, CAN_IT_LEC);
	CAN_ClearITPendingBit(CAN1, CAN_IT_ERR);*/
}
#endif



#ifdef STM32F30X
void USBWakeUp_RMP_IRQHandler(void){
#endif
#ifdef STM32F10X_MD
void USBWakeUp_IRQHandler(void){
#endif
	EXTI_ClearITPendingBit(EXTI_Line18);
}

#endif //#if defined(STM32F10X_MD) || defined(STM32F30X)






