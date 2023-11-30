// Serial
#include "stm32f10x.h"                  // Device header
#include "string.h"

int main(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//USART2 TX RX
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//USART2 ST-LINK USB
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	

	//USART_ClockInitTypeDef USART_ClockInitStructure; 
	
	USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
 	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);

	// Enable the USART2 TX Interrupt 
	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// Enable the USART2 RX Interrupt
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE );
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	while(1) {}
}

unsigned char character, state = 0;
unsigned char pressed_character = 0;
int i = 0;

void USART2_IRQHandler() {
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET) {
		if (state == 0) {
			USART_SendData(USART2, 'a');
		}
		USART_ClearITPendingBit(USART2, USART_IT_TC);
	}
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		character = (unsigned char) USART_ReceiveData(USART2); 
		if (pressed_character == 0) {
			state = 1;
			i = 0;
			pressed_character = character;
		} else {
			if (pressed_character == character) {
				state = 0;
				pressed_character = 0;
			} else {
				state = 2;
			}
		}
	}
	
	if (state == 0) {
			USART_SendData(USART2, 'a');
	} else if (state == 1) {
		if (i < 10) {
			USART_SendData(USART2, pressed_character);
			i++;
		}
	}
}
