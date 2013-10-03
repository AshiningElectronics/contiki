/*
 * ws2811.h
 *
 *  Created on: 14 jul 2013
 *      Author: benjamin
 */

#ifndef WS2811_H_
#define WS2811_H_

#include <stdint.h>
#include "contiki.h"

// Functions
void ws2811_init(void);
void ws2811_set_led_color(int led, uint32_t color);
uint32_t ws2811_get_led_color(int led);
void ws2811_all_off();
void ws2811_set_all(uint32_t color);

#endif /* WS2811_H_ */
