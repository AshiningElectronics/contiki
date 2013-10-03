/*
 * Author  : Benjamin Vedder
 * Created : 2013-05-31
 */

#include "contiki.h"
#include "dev/leds.h"
#include "stm32f4xx_conf.h"

static int green_on = 0;
static int yellow_on = 0;
static int red_on = 0;

void leds_arch_init(void) {
	GPIO_InitTypeDef  GPIO_InitStructure;

	// Periph clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// Configure PA2 and PA3 in output pushpull mode
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

unsigned char leds_arch_get(void) {
	return (green_on ? 0 : LEDS_GREEN)
		| (yellow_on ? 0 : LEDS_YELLOW)
		| (red_on ? 0 : LEDS_RED);
}

void leds_arch_set(unsigned char leds) {
	if(leds & LEDS_GREEN) {
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
		green_on = 1;
	} else {
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		green_on = 0;
	}
	
	if(leds & LEDS_YELLOW) {
		//GPIO_SetBits(GPIOD, GPIO_Pin_13);
		yellow_on = 1;
	} else {
		//GPIO_ResetBits(GPIOD, GPIO_Pin_13);
		yellow_on = 0;
	}
	
	if(leds & LEDS_RED) {
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
		red_on = 1;
	} else {
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		red_on = 0;
	}
}

