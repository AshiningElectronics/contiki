CONTIKI_PROJECT = udp-server
all: $(CONTIKI_PROJECT)

TARGET=stm32-bv
PRINTF_VCP=0
NODEID=7
UIP_CONF_IPV6=1
APPS=webserver

# WS2811
WS2811_ENABLE  = 1
WS2811_LED_NUM = 150
WS2811_CLK_HZ  = 800000

CONTIKI = ../../../
include $(CONTIKI)/Makefile.include
