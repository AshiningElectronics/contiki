#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include <sys/clock.h>
#include <sys/cc.h>
#include <sys/etimer.h>

static volatile clock_time_t current_clock = 0;
static volatile unsigned long current_seconds = 0;
static unsigned int second_countdown = CLOCK_SECOND;

void clock_it_handler(void) {
	current_clock++;
	if(etimer_pending() && etimer_next_expiration_time() <= current_clock) {
		etimer_request_poll();
		/* printf("%d,%d\n", clock_time(),etimer_next_expiration_time  	()); */
	}
	
	if (--second_countdown == 0) {
		current_seconds++;
		second_countdown = CLOCK_SECOND;
	}
}


void clock_init() {
	if (SysTick_Config(SystemCoreClock / CLOCK_SECOND)) {
		// Capture error
		while (1){};
	}
}

clock_time_t clock_time(void) {
	return current_clock;
}

// TODO!!!
void clock_delay(unsigned int t) {
	volatile int i, j;
	for (i = 0;i < t;i++) {
		for (j = 0;j < 500;j++) {}
	}
}

unsigned long clock_seconds(void) {
	return current_seconds;
}
