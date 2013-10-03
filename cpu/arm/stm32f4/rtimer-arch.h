/**
 * \file
 *         Header file for the STM32F103-specific rtimer code
 * \author
 *         Simon Berg <ksb@users.sourceforge.net>
 */

#ifndef __RTIMER_ARCH_H__
#define __RTIMER_ARCH_H__

#include "sys/rtimer.h"
#include "contiki-conf.h"

#ifndef RTIMER_ARCH_SECOND
#define RTIMER_ARCH_SECOND 100000
#endif

// Functions
void rtimer_arch_ovf_isr(void);
void rtimer_arch_comp_isr(void);
void rtimer_arch_set(rtimer_clock_t t);
rtimer_clock_t rtimer_arch_now(void);

#endif /* __RTIMER_ARCH_H__ */
