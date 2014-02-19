# Src and includes for all the libraries


############# Core #############

STM32_CONF =	lib/Conf
STM32_CORE =	lib/Core/stm32
CMSIS =			lib/Core/cmsis


############# STDPERIPH #############

STDPERIPH_DIR = lib/StdPeriph

STDPERIPH_SRC =	misc.c \
				stm32f4xx_dma.c \
				stm32f4xx_rcc.c \
				stm32f4xx_adc.c \
				stm32f4xx_exti.c \
				stm32f4xx_rng.c \
				stm32f4xx_can.c \
				stm32f4xx_flash.c \
				stm32f4xx_rtc.c \
				stm32f4xx_crc.c \
				stm32f4xx_fsmc.c \
				stm32f4xx_sdio.c \
				stm32f4xx_cryp_aes.c \
				stm32f4xx_gpio.c \
				stm32f4xx_spi.c \
				stm32f4xx_cryp.c \
				stm32f4xx_hash.c \
				stm32f4xx_syscfg.c \
				stm32f4xx_cryp_des.c \
				stm32f4xx_hash_md5.c \
				stm32f4xx_tim.c \
				stm32f4xx_cryp_tdes.c \
				stm32f4xx_hash_sha1.c \
				stm32f4xx_usart.c \
				stm32f4xx_dac.c \
				stm32f4xx_i2c.c \
				stm32f4xx_wwdg.c \
				stm32f4xx_dbgmcu.c \
				stm32f4xx_iwdg.c \
				stm32f4xx_dcmi.c \
				stm32f4xx_pwr.c


############# USB_OTG #############

USB_OTG_PATH = lib/USB_OTG

USB_OTG_SRC = 	usb_core.c \
				usb_dcd.c \
				usb_dcd_int.c


############# USB_DEVICE_CORE #############

USB_DEVICE_CORE_PATH = lib/USB_Device/Core

USB_DEVICE_CORE_SRC =	usbd_core.c \
						usbd_ioreq.c \
						usbd_req.c


############# USB_DEVICE_CLASS_CDC #############

USB_DEVICE_CLASS_CDC_PATH = lib/USB_Device/Class/cdc

USB_DEVICE_CLASS_CDC_SRC = 	usbd_cdc_core.c








