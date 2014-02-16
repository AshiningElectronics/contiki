#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>

PROCESS(radio_test_process, "Radio test");
AUTOSTART_PROCESSES(&radio_test_process);

#define ON  1
#define OFF 0

#define HEADER "RTST"
#define PACKET_SIZE 20
#define PORT 9345

struct indicator {
	int onoff;
	int led;
	clock_time_t interval;
	struct etimer timer;
};
static struct etimer send_timer;
static struct indicator recv, other, flash;

static void set(struct indicator *indicator, int onoff) {
	if(indicator->onoff ^ onoff) {
		indicator->onoff = onoff;
		if(onoff) {
			leds_on(indicator->led);
		} else {
			leds_off(indicator->led);
		}
	}
	if(onoff) {
		etimer_set(&indicator->timer, indicator->interval);
	}
}

static void abc_recv(struct abc_conn *c)
{
	/* packet received */
	if(packetbuf_datalen() < PACKET_SIZE
			|| strncmp((char *)packetbuf_dataptr(), HEADER, sizeof(HEADER))) {
		/* invalid message */

	} else {
		PROCESS_CONTEXT_BEGIN(&radio_test_process);
		set(&recv, ON);
		set(&other, ((char *)packetbuf_dataptr())[sizeof(HEADER)] ? ON : OFF);

		/* synchronize the sending to keep the nodes from sending
       simultaneously */

		etimer_set(&send_timer, CLOCK_SECOND);
		etimer_adjust(&send_timer, - (int) (CLOCK_SECOND >> 1));
		PROCESS_CONTEXT_END(&radio_test_process);
	}
}
static const struct abc_callbacks abc_call = {abc_recv};
static struct abc_conn abc;

PROCESS_THREAD(radio_test_process, ev, data) {
	PROCESS_BEGIN();

	/* Initialize the indicators */
	recv.onoff = other.onoff = flash.onoff = OFF;
	recv.interval = other.interval = CLOCK_SECOND;
	flash.interval = 1;
	flash.led = LEDS_RED;
	recv.led = LEDS_GREEN;
	other.led = LEDS_BLUE;

	abc_open(&abc, PORT, &abc_call);
	etimer_set(&send_timer, CLOCK_SECOND);

	while(1) {
		PROCESS_WAIT_EVENT();
		if (ev == PROCESS_EVENT_TIMER) {
			if(data == &send_timer) {
				etimer_reset(&send_timer);

				/* send packet */
				packetbuf_copyfrom(HEADER, sizeof(HEADER));
				((char *)packetbuf_dataptr())[sizeof(HEADER)] = recv.onoff;
				/* send arbitrary data to fill the packet size */
				packetbuf_set_datalen(PACKET_SIZE);
				set(&flash, ON);
				abc_send(&abc);

			} else if(data == &other.timer) {
				set(&other, OFF);

			} else if(data == &recv.timer) {
				set(&recv, OFF);

			} else if(data == &flash.timer) {
				set(&flash, OFF);
			}
		}
	}
	PROCESS_END();
}
/*---------------------------------------------------------------------*/
