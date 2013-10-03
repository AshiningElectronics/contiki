#include "contiki.h"

#include "dev/slip.h"
#include "usbd_cdc_vcp.h"

// Settings
#define SLIP_OUT_BUFFER_LEN		2500

// Private functions
static void usb_cdc_input_handler(unsigned char c);

// Processes
PROCESS(slip_input_process, "Slip input process");
PROCESS(slip_output_process, "Slip output process");
PROCESS(asd_process, "Slip hack");

// Variables
static unsigned char slip_out_buffer[SLIP_OUT_BUFFER_LEN];
static int slip_out_read_ptr = 0;
static int slip_out_write_ptr = 0;

void slip_arch_init(unsigned long ubr) {
	process_start(&slip_input_process, NULL);
	process_start(&slip_output_process, NULL);
	process_start(&asd_process, NULL);
	VCP_set_data_available_handler(usb_cdc_input_handler);
}

void slip_arch_writeb(unsigned char c) {
	slip_out_buffer[slip_out_write_ptr++] = c;
	if (slip_out_write_ptr == SLIP_OUT_BUFFER_LEN) {
		slip_out_write_ptr = 0;
	}
	
	process_poll(&slip_output_process);
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

PROCESS_THREAD(slip_output_process, ev, data) {
	PROCESS_BEGIN();
	
	static unsigned char buffer[SLIP_OUT_BUFFER_LEN];
	static int len = 0;	
	
	for(;;) {
		PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
		
		len = 0;
		while(slip_out_read_ptr != slip_out_write_ptr) {
			buffer[len] = slip_out_buffer[slip_out_read_ptr++];
			if (slip_out_read_ptr == SLIP_OUT_BUFFER_LEN) {
				slip_out_read_ptr = 0;
			}
			len++;
		}
		
		VCP_send_buffer(buffer, len);
		process_poll(&asd_process);
	}

	PROCESS_END();
	return 0;
}

/*
 * This process is required to send something on the usb-cdc port in a later
 * SOF frame so that the application receives it. Otherwise, the bytes that
 * were written during the last frame will be stuck in some buffer somewhere
 * on the USB stack until something else is written. I don't know why this
 * happens...
 */
PROCESS_THREAD(asd_process, ev, data)
{
#define SLIP_END     0300
	static struct etimer timer;
	
	PROCESS_BEGIN();
	etimer_set(&timer, CLOCK_CONF_SECOND / 500);
  
	for (;;) {
		PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
		etimer_reset(&timer);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
		VCP_put_char(SLIP_END);
	}
  
	PROCESS_END();
}

