#ifndef PLATFORM_CONF_H_
#define PLATFORM_CONF_H_

#include "stm32f4xx_conf.h"
#include <stdint.h>

/*
 * Definitions below are dictated by the hardware and not really
 * changeable!
 */

#define PLATFORM_HAS_LEDS   1

#ifndef PRINTF_VCP
#define PRINTF_VCP		1
#endif

#ifndef BV
#define BV(x) (1<<(x))
#endif

#define CCIF
#define CLIF

// Types for clocks and uip_stats
typedef unsigned long clock_time_t;
//typedef uint64_t clock_time_t;

typedef uint8_t   u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef int32_t s32_t;
typedef unsigned short uip_stats_t;

#define ENERGEST_TOTAL_IS_RTIMER_T
typedef uint64_t rtimer_clock_t;
#define RTIMER_CLOCK_LT(a,b)     ((signed short)((a)-(b)) < 0)

// the low-level radio driver
#define NETSTACK_CONF_RADIO   cc2520_driver

/*
 * SPI Driver
 */
void cc2520_arch_fifop_int_init(void);
void cc2520_arch_fifop_int_enable(void);
void cc2520_arch_fifop_int_disable(void);
 
#define READ_PAD(port, pad) (((port->IDR) >> (pad)) & 1)
#define SET_PAD(port, pad) (port->BSRRL = (1 << pad))
#define CLEAR_PAD(port, pad) (port->BSRRH = (1 << pad))

#define CC2520_CONF_SYMBOL_LOOP_COUNT 26050
 
#define SPI_TXBUF SPI2->DR
#define SPI_RXBUF SPI2->DR

#define SPI_WAITFOREOTx() do { while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE)); SPI_I2S_ReceiveData(SPI2); } while (0)
#define SPI_WAITFOREORx() do { while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE)); } while (0)

/*
 * Enables/disables CC2520 access to the SPI bus (not the bus).
 * (Chip Select)
 */

/* ENABLE CSn (active low) */
#define CC2520_SPI_ENABLE()     do{CLEAR_PAD(GPIOB, 12);clock_delay(1);}while(0)
//#define CC2520_SPI_ENABLE()     CLEAR_PAD(GPIOB, 12)
/* DISABLE CSn (active low) */
#define CC2520_SPI_DISABLE()    do{SET_PAD(GPIOB, 12);clock_delay(1);}while(0)
//#define CC2520_SPI_DISABLE()    SET_PAD(GPIOB, 12)

/* Pin status.CC2520 */
#define CC2520_FIFO_IS_1 (READ_PAD(GPIOC, 4))
#define CC2520_FIFOP_IS_1  (READ_PAD(GPIOC, 5))
#define CC2520_CCA_IS_1   (READ_PAD(GPIOB, 0))
#define CC2520_SFD_IS_1   (READ_PAD(GPIOB, 1))

/* The CC2520 reset pin. */
#define SET_RESET_INACTIVE()   SET_PAD(GPIOB, 11)
#define SET_RESET_ACTIVE()     CLEAR_PAD(GPIOB, 11)

/* CC2520 voltage regulator enable pin. */
#define SET_VREG_ACTIVE()
#define SET_VREG_INACTIVE()

/* CC2520 rising edge trigger for external interrupt 0 (FIFOP). */
#define CC2520_FIFOP_INT_INIT() cc2520_arch_fifop_int_init()

/* FIFOP on external interrupt C4. */
/* FIFOP on external interrupt C4. */
#define CC2520_ENABLE_FIFOP_INT() cc2520_arch_fifop_int_enable()
#define CC2520_DISABLE_FIFOP_INT() cc2520_arch_fifop_int_disable()
#define CC2520_CLEAR_FIFOP_INT()

// ??
#define splhigh() 0
#define splx(arg)

#endif /* PLATFORM_CONF_H_ */

