#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
typedef struct
{
	uint8_t Buf[32];	//���ڽ��ջ�����
	uint8_t Over;			//���ڽ��ܼ��
	uint8_t Len;				//���ڽ��ܳ���
}UartBufTypedef;


//��ʼ��
void USART1_Initialise(u32 bound);
void USART2_Initialise(u32 bound);
void USART3_Initialise(u32 bound);
//���ڽ�������
void USART_RxFuncConfig(USART_TypeDef* USARTx , void (* func)(uint8_t data));
//�ж�
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
//����
void USART_SendString(USART_TypeDef* USARTx,uint8_t * Send);
#endif

