#include "sys/energest.h"
#include "sys/rtimer.h"
#include "stm32f4xx_conf.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static uint32_t time_msb = 0;  // Most significant bits of the current time.

// time of the next rtimer event. Initially is set to the max value.
static rtimer_clock_t next_rtimer_time = 0;

void rtimer_arch_ovf_isr(void) {
	time_msb++;
	rtimer_clock_t now =  ((rtimer_clock_t)time_msb << 16) | TIM7->CNT;
	rtimer_clock_t clock_to_wait = next_rtimer_time - now;

	if(clock_to_wait <= 0x10000 && clock_to_wait > 0){ // We must set now the Timer Compare Register.
		TIM_SetCompare1(TIM7, (uint16_t)clock_to_wait);
		TIM_ITConfig(TIM7, TIM_IT_CC1, ENABLE); // Compare 1 interrupt enable.
	}
}

void rtimer_arch_comp_isr(void) {
	TIM_ITConfig(TIM7, TIM_IT_CC1, DISABLE); // Disable the next compare interrupt

	PRINTF("\r\nCompare event %4x\r\n", (unsigned int)TIM7->CNT);
	ENERGEST_ON(ENERGEST_TYPE_IRQ);
	rtimer_run_next();
	ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}

void rtimer_arch_init(void) {
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	uint16_t PrescalerValue = 0;

	// ------------- Timer7 ------------- //
	// Compute the prescaler value
	// TIM7 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	PrescalerValue = (uint16_t) ((168000000 / 2) / RTIMER_ARCH_SECOND) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

	// Prescaler configuration
	TIM_PrescalerConfig(TIM7, PrescalerValue, TIM_PSCReloadMode_Immediate);

	// Disable ARR buffering
	TIM_ARRPreloadConfig(TIM7, DISABLE);
	
	// Output Compare Timing Mode configuration: Channel1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0xFFFF;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM7, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM7, TIM_OCPreload_Disable);

	// Interrupt generation
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

	// TIM7 enable counter
	TIM_Cmd(TIM7, ENABLE);

	// NVIC
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	PRINTF("rtimer_arch_init done\r\n");
}

rtimer_clock_t rtimer_arch_now(void) {
	return ((rtimer_clock_t)time_msb << 16) | TIM7->CNT;
}

void rtimer_arch_schedule(rtimer_clock_t t) {
	PRINTF("rtimer_arch_schedule time %4x\r\n", /*((uint32_t*)&t)+1,*/(unsigned int)t);

	next_rtimer_time = t;

	rtimer_clock_t now = rtimer_arch_now();

	rtimer_clock_t clock_to_wait = t - now;
	
	PRINTF("now %2x\r\n", (unsigned int)TIM7->CNT);
	PRINTF("clock_to_wait %4x\r\n", (unsigned int)clock_to_wait);

	if(clock_to_wait <= 0x10000) { // We must set now the Timer Compare Register.
		TIM_SetCompare1(TIM7, (uint16_t)now + (uint16_t)clock_to_wait);
		TIM_ITConfig(TIM7, TIM_IT_CC1, ENABLE); // Compare 1 interrupt enable.
	}
	// else compare register will be set at overflow interrupt closer to the rtimer event.
}

