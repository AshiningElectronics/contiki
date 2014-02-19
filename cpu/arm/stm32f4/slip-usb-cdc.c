#include "contiki.h"

#include "dev/slip.h"
#include "usbd_cdc_vcp.h"

// Private functions
static void usb_cdc_input_handler(unsigned char c);

// Processes
PROCESS(slip_input_process, "Slip input process");

void slip_arch_init(unsigned long ubr) {
	process_start(&slip_input_process, NULL);
	VCP_set_data_available_handler(usb_cdc_input_handler);
}

void slip_arch_writeb(unsigned char c) {
	VCP_put_char(c);
}

static void usb_cdc_input_handler(unsigned char c) {
	process_poll(&slip_input_process);
}

PROCESS_THREAD(slip_input_process, ev, data) {
	PROCESS_BEGIN();

	for(;;) {
		PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
		
		unsigned char c = 0;
		while (VCP_get_char(&c) != 0) {
			slip_input_byte(c);
		}
	}

	PROCESS_END();
	return 0;
}

