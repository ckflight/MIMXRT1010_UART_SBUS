
#include "CK_TIME.h"

uint32_t sysTickCounter = 0;

void CK_TIME_Init(void){

	sysTickCounter = 0;

	// 1ms interrupt
	SysTick->LOAD  = (uint32_t)((500000000UL / 1000) - 1UL); // set reload register
	NVIC_SetPriority (SysTick_IRQn, 0UL); 					 // set Priority for Systick Interrupt
	SysTick->VAL   = 0UL;                                    // Load the SysTick Counter Value
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; // Enable SysTick IRQ and SysTick Timer

}

uint32_t CK_TIME_GetMicroSec(void){

	uint32_t ticks ;
	uint32_t count ;

	SysTick->CTRL;

	do{
		ticks = SysTick->VAL;

		count = sysTickCounter;
	}
	while (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk);

	return count * 1000 + (SysTick->LOAD + 1 - ticks) / (500000000UL / 1000000);

}

uint32_t CK_TIME_GetMilliSec(void){

	return sysTickCounter;

}

void CK_TIME_DelayMicroSec(uint32_t usec){

	uint32_t now = CK_TIME_GetMicroSec();

	while (CK_TIME_GetMicroSec() - now < usec);

}

void CK_TIME_DelayMilliSec(uint32_t msec){

	while(msec--)CK_TIME_DelayMicroSec(1000);

}

void SysTick_Handler(void){

	sysTickCounter++;

}
