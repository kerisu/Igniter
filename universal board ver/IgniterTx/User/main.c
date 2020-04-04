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
#include "12864.h"
#include "key.h"
/*Golbal data space ----------------------------------------------------------*/

#define UART_COM_STAT   '<'	//����ͨ�ſ�ʼ�ֽ�
#define UART_COM_END		'>'	//����ͨ�Ž����ֽ�

static UartBufTypedef g_uartComData = {.Over = 0 ,.Len = 0};//���ڽ��ջ�����

uint8_t g_FAState = '-';		//��ѹ����
uint8_t g_FFState = '-';		//������

uint16_t g_Ebt = 0;				//��ص�ѹ
uint16_t g_Efr = 0;				//����ѹ

uint8_t  g_locked = 1;		//������־λ
uint32_t g_Ping = 4000;		//���ն��ӳ�


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

/*12864 LCD display -----------------------------------------------------------*/

//�ͻ���״̬��ʾ
void ClientStateDisplay(void)
{
	uint8_t str[50];
	if(g_Ping > 3000)
	{
		gui_draw_text(4,22,"---     ",0);
		gui_draw_text(57,21,"B:--.-- B:-",0);
		gui_draw_text(57,30,"F:--.-- F:-",0);
	}
	else
	{
		sprintf(str,"No.01-%02d",(g_Ping /100)%100);
		gui_draw_text(4,22,str,0);
	}
	
	g_Ping++;
}

void OptionDisplay(void)
{
	uint8_t str[50];

	//gui_draw_text(57,21,"B:--.-- B:-",0);
	//gui_draw_text(57,30,"F:--.-- F:-",0);
	
	sprintf(str,"B:%2d.%02d B:%c",g_Ebt/100,g_Ebt%100,g_FAState);
	gui_draw_text(57,21,str,0);
	
	sprintf(str,"F:%2d.%02d F:%c",g_Efr/100,g_Efr%100,g_FFState);
	gui_draw_text(57,30,str,0);
}

//�����״̬��ʾ
void IgniterStateDisplay(uint8_t * cmd)
{
	g_FAState = cmd[2];
	g_FFState = cmd[1];
	
	OptionDisplay();
}
//�������ѹ��ʾ
void IgniterVoltageDisplay(uint8_t * cmd)
{
	g_Ebt = (cmd[1] - '0') * 1000 + 
					(cmd[2] - '0') * 100	+
					(cmd[3] - '0') * 10		+
					(cmd[4] - '0') * 1		;
	
	g_Efr = (cmd[5] - '0') * 1000 + 
					(cmd[6] - '0') * 100	+
					(cmd[7] - '0') * 10		+
					(cmd[8] - '0') * 1		;
	
	OptionDisplay();
}

//����״̬��ʾ
void lockState(void)
{
	if(g_locked == 1)
	{
		gui_draw_text(73,54,"   Locked",0);
	}
	else
	{
		gui_draw_text(73,54,"!unLocked",1);
	}
}

//���״̬��ʾ
void ChargeStateDisplay(uint8_t num)
{
	if(num == 1)
	{
		gui_draw_text(57,42,"CHARGING...",0);
	}
	else if(num == 0)
	{
		gui_draw_text(57,42,"STANDBY    ",0);
	}
	else if(num == 2)
	{
		gui_draw_text(57,42,"FIRE --- !!",0);
	}
}

void Lcd_setup_display(void)
{
	gui_draw_square(0,0,127,63,1);
	gui_draw_text(1,0,"RI-SYSTEM V1.0  BY:ZD",1);
	
	gui_draw_square(0,53,127,63,1);
	gui_draw_text(79,54,"  Locked",0);
	
	gui_draw_square(2,10,53,51,1);
	gui_draw_text(4,11,"CLIENT",0);
	gui_draw_square(2,20,53,51,1);
	gui_draw_text(4,22,"---",0);
	
	gui_draw_square(55,10,125,51,1);
	gui_draw_text(57,11,"OPTION",0);
	gui_draw_square(55,20,125,51,1);
	
	gui_draw_text(57,21,"B:--.-- B:-",0);
	gui_draw_text(57,30,"F:--.-- F:-",0);
	gui_draw_text(57,42,"STANDBY    ",0);
	//gui_draw_text(57,39,"Charging...",0);
	
	//font_putString(1,17,"Test123",0);
	//LCD_Display_Update();
}

void test(void)
{
	static uint32_t num = 0;
	num++;
	uint8_t str[50];
	sprintf(str,"%d%d%d",getFireKey(),getChargeKey(),getKeyboad());
	gui_draw_text(4,32,str,0);
	
}

//ˢ��Ƶ��10Hz
void lcd_display_div(void)
{
	static uint32_t div = 0;
	div++;
	if(div > 100)//ˢ��Ƶ�� 10Hz
	{
		div = 0;
		LCD_Display_Update();
	}
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
			case 'A'://�������Ϣ����
			{
				IgniterStateDisplay(buf->Buf);
				break;
			}
			case 'V'://��ѹ��Ϣ��ʾ
			{
				IgniterVoltageDisplay(buf->Buf);
				break;
			}
			
		}
		//���δ��ɱ�־λ
		buf->Over = 0;
		//��Ϣ�������
		g_Ping = 0;
	}
}

//״̬��ȡ����
void sendStateRequest(void)
{
	static uint32_t divI = 0;
	divI++;
	if(divI == 400)
	{
		USART_SendString(USART2,"<I>");
	}
	if(divI >= 800)
	{
		divI = 0;
		USART_SendString(USART2,"<V>");
	}
}
/*key func ------------------------------------------------------------*/




void keyScan(void)
{
	static uint8_t keyboardhist = 0;
	static uint32_t chargediv  = 0;
	static uint32_t firediv = 0;
	
	//���ܼ�
	switch(getKeyboad())
	{
		case 1:
		{
			break;
		}
		case 2:
		{
			break;
		}
		case 3:
		{
			break;
		}
		case 4:
		{
			//�ϱ��ش���
			if(keyboardhist == 0)
			{
				//����
				if(g_locked == 1)
				{
					uint8_t num = 0;
					while(getKeyboad() == 4 && num < 200)
					{
						delay_ms(10);
						num++;
					}
					if(getKeyboad() == 4)
					{
						g_locked = 0;
						led_lock(1);
					}
				}
				else//���� 
				{
					g_locked = 1;
					led_lock(0);
				}
				lockState();
			}
			break;
		}
		default:break;
	}
	
	keyboardhist = getKeyboad();
	

	chargediv++;
	if(chargediv > 300)
	{
		chargediv = 0;
		
		//�������
		if(getChargeKey() == 1 && g_locked == 0 && g_Ping < 2000)
		{
			USART_SendString(USART2,"<FAE>");
			ChargeStateDisplay(1);
		}
		else
		{
			USART_SendString(USART2,"<FAD>");
			ChargeStateDisplay(0);
		}
		delay_ms(100);
		//�������
		if(getFireKey() == 1 && g_locked == 0 && g_Ping < 2000)
		{
			USART_SendString(USART2,"<FFE>");
			ChargeStateDisplay(2);
		}
		else
		{
			USART_SendString(USART2,"<FFD>");
			ChargeStateDisplay(0);
		}
	}
	
	
}


/*main function -------------------------------------------------------*/
int main(void)
{		
	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	//��ʱ��ʼ��
	delay_init();

	//����ͨ�ų�ʼ��
	USART2_Initialise(115200);  
	//���ô��ڽ��պ���
	USART_RxFuncConfig(USART2,UsartRxToBuf);
	//LED��ʼ��
	led_init(); 
	//���̳�ʼ��
	keyboardInit();
	//12864��ʼ��
	Lcd_Init();
	Lcd_setup_display();
	
	USART_SendString(USART2,"System Start ok\r\n");

	while (1)
	{
		//�ͻ���״̬��ʾ
		ClientStateDisplay();
		
		//������Ϣ����
		RxBufAnalysis(&g_uartComData);
		//LED������
		led_running();
		//��ʾˢ�·�Ƶ��
		lcd_display_div();
		
		//����ɨ��
		keyScan();
		//״̬��ȡ����
		sendStateRequest();
		
		delay_ms(1);
	}	 
	return 0;
}


