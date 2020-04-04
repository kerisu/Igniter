/**
  ******************************************************************************
  * @file    main.c
  * @author  �Ŷ�
  * @version V1.0.0
  * @date    2020-1-3
  * @brief   �����������������
  ******************************************************************************
  */
/*include file ---------------------------------------------------------------*/
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "pulser.h"
#include "quantity.h"
#include "stdio.h"

/*Golbal data space ----------------------------------------------------------*/

#define UART_COM_STAT   '<'	//����ͨ�ſ�ʼ�ֽ�
#define UART_COM_END		'>'	//����ͨ�Ž����ֽ�

UartBufTypedef g_uartComData = {.Over = 0 ,.Len = 0};//���ڽ��ջ�����

uint8_t g_FAState = 0;		//��ѹ����
uint8_t g_FFstate = 0;		//������


//�����ֽ���д�뵽������
void UsartRxToBuf(uint8_t data)
{
	//��һ����Ϣ�������
	if(g_uartComData.Over == 0)
	{
		//��ʼ
		if(data == UART_COM_STAT )
		{
			//��������
			g_uartComData.Len = 0;
		}
		//����
		else if(data == UART_COM_END)
		{
			//���ս���
			g_uartComData.Over = 1;
		}
		//����
		else
		{
			//д������
			g_uartComData.Buf[g_uartComData.Len] = data;
			//�ƶ����
			g_uartComData.Len = (g_uartComData.Len + 1) % 32;
		}
	}
}

/*Battery -------------------------------------------------------------------*/
//��õ�����ѹ  0 ��������� 1 ������������
float getBatteryPackVoltage(uint8_t ch)
{
	switch (ch)
	{
		case 0:
		{
			//��ȡADCͨ��ֵ
			uint16_t adcval = get_adc(ADC_Channel_0);
			//����ADCͨ����ѹ
			float vol = 3.3 * adcval / 4096;
			//��ѹ�ȼ��������ѹ
			vol = vol * (15000 + 750) / 750;
			//��������
			vol = vol * 1.0;
			return vol;
			break;
		}
		case 1:
		{
			//��ȡADCͨ��ֵ
			uint16_t adcval = get_adc(ADC_Channel_1);
			//����ADCͨ����ѹ
			float vol = 3.3 * adcval / 4096;
			//��ѹ�ȼ��������ѹ
			vol = vol * (1500 + 750) / 750;
			//��������
			vol = vol * 1.0296296;
			return vol;
			break;
		}
	}
	return 0;
}

//���ص�ѹֵ
void Adc_BatteryVoltage(void)
{
	uint8_t str[50];
	
	//��ʼ
	str[0] = '<';
	
	//��ʶ
	str[1] = 'V';
	
	//���Ƶ�����ѹ
	float ctr = getBatteryPackVoltage(0);
	
	str[2] = (uint8_t)(ctr / 10  ) % 10 + 0x30;
	str[3] = (uint8_t)(ctr       ) % 10 + 0x30;
	str[4] = (uint8_t)(ctr * 10  ) % 10 + 0x30;
	str[5] = (uint8_t)(ctr * 100 ) % 10 + 0x30;
	
	//����������ѹ
	float mtr = getBatteryPackVoltage(1);
	
	str[6] = (uint8_t)(mtr / 10  ) % 10 + 0x30;
	str[7] = (uint8_t)(mtr       ) % 10 + 0x30;
	str[8] = (uint8_t)(mtr * 10  ) % 10 + 0x30;
	str[9] = (uint8_t)(mtr * 100 ) % 10 + 0x30;
	
	//����
	str[10] = '>';
	
	str[11] = 0;
	
	USART_SendString(USART1,str);
}
/*Igniter control -----------------------------------------------------------------*/

//����������Ϣ����
void igniterFeedBack(void)
{
	uint8_t str[50];
	
	str[0] = '<';
	str[1] = 'F';
	str[2] = (g_FAState == 1) ? 'E' : 'D';
	str[3] = (g_FFstate == 1) ? 'E' : 'D';
	str[4] = '>';
	str[5] = '\0';
	
	USART_SendString(USART1,str);
	
}
//���������
void igniterAnalyze(uint8_t * cmd)
{
	//���ָ�� <FAE><FAD> �����ر���ѹ <FFE><FFD>�����رյ��
	
	//��ѹ����
	if(cmd[1] == 'A')
	{
		if(cmd[2] == 'E')
		{
			g_FAState = 1;
		}
		else if(cmd[2] == 'D')
		{
			g_FAState = 0;
		}
	}

	
	//������
	if(cmd[1] == 'F')
	{
		if(cmd[2] == 'E')
		{
			g_FFstate = 1;
		}
		else if(cmd[2] == 'D')
		{
			g_FFstate = 0;
		}
	}
	
	igniterFeedBack();
}

//��ѹ�����Ƴ���
void igniterCtrl(void)
{
	//g_FAState = 0;
	//g_FFstate = 0;
	
	if(g_FAState == 1)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_7);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	}
	
	if(g_FFstate == 1)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	}
}

//���������GPIO��ʼ��
void igniterInit(void)
{
	GPIO_InitTypeDef gpio;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed  = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&gpio);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_6 | GPIO_Pin_7);
}

/*Comhandle -----------------------------------------------------------------------*/


//��Ϣ���ս���
void RxBufAnalysis(UartBufTypedef * buf)
{
	//С�����Ա�ʹ��
	if(buf->Over == 1)
	{	
		//ͨ��Э��
		//��ʼ ��<�� ���� '>'
		//[0] ��ʶ [n] ����
		switch(buf->Buf[0])
		{
			case 'I'://�������Ϣ����
			{
				igniterFeedBack();
				break;
			}
			case 'V'://��ȡ���������
			{
				Adc_BatteryVoltage();
				break;
			}
			case 'F'://���ָ�� <FAE><FAD> �����ر���ѹ <FFE><FFD>�����رյ��
			{
				igniterAnalyze(buf->Buf);
				break;
			}
		}
		//���δ��ɱ�־λ
		buf->Over = 0;
	}
}
/**/

/*LED ------------------------------------------------------------------------*/
void led_init(void)
{
	GPIO_InitTypeDef gpio;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed  = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC,&gpio);
}

void led_running(void)
{
	static uint16_t ph,div;
	static _Bool flag;
	div++;
	GPIOC->BSRR = GPIO_Pin_13 << 16 * (ph<div);
	if(div==300)
	{
		div = RESET;
		ph += !flag;
		ph -= flag;
		flag += (ph==300);
		flag *= !(ph==0);
	}
}

/*main function -------------------------------------------------------*/
int main(void)
{		
	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	//��ʱ��ʼ��
	delay_init();

	//PWMͨ����ʼ��
	//TIM2_PWM_Init(1000-1,2-1);
	//TIM3_PWM_Init(1000-1,2-1); 
	//����ͨ�ų�ʼ��
	USART1_Initialise(115200);  
	//���ô��ڽ��պ���
	USART_RxFuncConfig(USART1,UsartRxToBuf);
	//LED��ʼ��
	led_init(); 
	//ADCת����ʼ��
	ADC_Init_quantity();
	//���������GPIO��ʼ��
	igniterInit();
	
	USART_SendString(USART1,"System Start ok\r\n");

	while (1)
	{
		//������Ϣ����
		RxBufAnalysis(&g_uartComData);
		//LED������
		led_running();
		
		
		//��ѹ�����Ƴ���
		igniterCtrl();
		
	}	 
	return 0;
}


