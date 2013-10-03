#include "contiki.h"

#include "dev/slip.h"
#include "stm32f4xx_conf.h"

// Function pointers
static int (*uart_input_handler)(unsigned char c) = 0;

void slip_arch_init(unsigned long baudrate) {
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	// IO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_SetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

	// Conf
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART6, &USART_InitStructure);
	
	// Enable the USART6 Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// Enable the USARTx Receive interrupt: this interrupt is generated when the
	// USARTx receive data register is not empty
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	
	// Enable
	USART_Cmd(USART6, ENABLE);
	
	// Set the function pointer for the receive function
	uart_input_handler = slip_input_byte;
}

void slip_arch_writeb(unsigned char c) {
	while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET){};
	USART_SendData(USART6, c);
}

void USART6_IRQHandler() {
	if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET) {
		uint8_t rec_byte = USART_ReceiveData(USART6);
		
		if(uart_input_handler != 0) {
			uart_input_handler(rec_byte);
		}
	}
}

