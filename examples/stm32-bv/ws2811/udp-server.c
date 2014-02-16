#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "dev/leds.h"
#include "ws2811.h"
#include "udp-server.h"
#include "webserver-nogui.h"

#include <string.h>

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *server_conn;
static LED_STATE led_state = LED_STATE_AUTO;

PROCESS(udp_server_process, "UDP server process");
PROCESS(led_process, "LED process");
AUTOSTART_PROCESSES(&udp_server_process, &led_process, &webserver_nogui_process);

static uint32_t scale_color(uint32_t color, float scale) {
	uint32_t r = (color >> 16) & 0xFF;
	uint32_t g = (color >> 8) & 0xFF;
	uint32_t b = color & 0xFF;

	r *= scale;
	g *= scale;
	b *= scale;

	return (r << 16) | (g << 8) | b;
}

static void tcpip_handler(void) {
//	char buf[MAX_PAYLOAD_LEN];
	uint32_t color;

	if (uip_newdata()) {
		leds_toggle(LEDS_RED);

		unsigned char *data = (unsigned char *)uip_appdata;

		// Only change state if it isn't none
		if (data[0] != LED_STATE_NONE) {
			led_state = data[0];
		}

		switch (led_state) {
		case LED_STATE_ALL_OFF:
			ws2811_all_off();
			break;

		case LED_STATE_ALL_ON:
			color = (data[1] << 16) | (data[2] << 8) | data[3];
			color = scale_color(color, (float)data[4] / 100.0);
			ws2811_set_all(color);
			break;

		case LED_STATE_AUTO:
			break;

		case LED_STATE_SINGLE:
			ws2811_all_off();
			color = (data[1] << 16) | (data[2] << 8) | data[3];
			color = scale_color(color, (float)data[4] / 100.0);
			ws2811_set_led_color(data[5], color);
			break;

		default:
			break;
		}

		//		uip_udp_packet_send(server_conn, "OK!", strlen("OK!"));
		/* Restore server connection to allow data from any node */
		memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));
	}
}

static void print_local_addresses(void) {
	int i;
	uint8_t state;

	PRINTF("Server IPv6 addresses: \r\n");
	for (i = 0; i < UIP_DS6_ADDR_NB; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if (uip_ds6_if.addr_list[i].isused
				&& (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
			PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
			PRINTF("\r\n");
		}
	}
}

PROCESS_THREAD(udp_server_process, ev, data) {
#if UIP_CONF_ROUTER
	uip_ipaddr_t ipaddr;
#endif /* UIP_CONF_ROUTER */

	PROCESS_BEGIN()
	;
	PRINTF("UDP server started\r\n");

#if UIP_CONF_ROUTER
	uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
	uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
	uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
#endif /* UIP_CONF_ROUTER */

	print_local_addresses();

	server_conn = udp_new(NULL, UIP_HTONS(3001), NULL );
	udp_bind(server_conn, UIP_HTONS(3000));

	while (1) {
		PROCESS_YIELD()
				;
		if (ev == tcpip_event) {
			tcpip_handler();
		}
	}

	PROCESS_END();
}

PROCESS_THREAD(led_process, ev, data) {
	static struct etimer timer;
	static int led_index = 0;
	static int color_index = 0;
	static int state = 0;
	static float intensity = 1.0;
	static int i;

	const uint32_t colors[] = {
			0xFFFFFF,
			0xFF0000,
			0x00FF00,
			0x0000FF,
			0xFFFF00,
			0xFF00FF,
			0x00FFFF
	};

	PROCESS_BEGIN();
	ws2811_init();

	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);

	etimer_set(&timer, CLOCK_CONF_SECOND / 50);

	for (;;) {
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

		leds_toggle(LEDS_GREEN);

		if (led_state == LED_STATE_AUTO) {
			if (state != 2) {
				ws2811_set_led_color(led_index, colors[color_index]);

				uint32_t clr_color = RNG_GetRandomNumber();
				if (led_index > 0) {
					ws2811_set_led_color(led_index - 1, clr_color);
				}
			}

			switch (state) {
			case 0:
				led_index++;
				if (led_index >= WS2811_LED_NUM) {
					led_index = WS2811_LED_NUM - 1;

					state++;
				}
				break;

			case 1:
				led_index--;
				if (led_index < 0) {
					led_index = 0;
					state++;
				}
				break;

			case 2:
				if (intensity > 0.0) {
					intensity -= 0.02;
					for(i = 0;i < WS2811_LED_NUM;i++) {
						ws2811_set_led_color(i, scale_color(colors[color_index], intensity));
					}
				} else {
					intensity = 1.0;

					color_index++;
					if (color_index >= sizeof(colors) / 4) {
						color_index = 0;
					}

					i = 0;
					state++;
				}
				break;

			case 3:
				i++;
				if (i > 25) {
					state++;
				}
				break;

			default:
				break;
			}

			if (state > 3) {
				state = 0;
			}
		}

		/* reset the timer so we can wait on it again */
		etimer_reset(&timer);
	}

	PROCESS_END();
}

