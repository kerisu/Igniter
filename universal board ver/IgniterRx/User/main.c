/**
  ******************************************************************************
  * @file    main.c
  * @author  张东
  * @version V1.0.0
  * @date    2020-1-3
  * @brief   点火器接收器主程序
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

#define UART_COM_STAT   '<'	//串口通信开始字节
#define UART_COM_END		'>'	//串口通信结束字节

UartBufTypedef g_uartComData = {.Over = 0 ,.Len = 0};//串口接收缓冲区

uint8_t g_FAState = 0;		//升压控制
uint8_t g_FFstate = 0;		//点火控制


//串口字节流写入到缓冲区
void UsartRxToBuf(uint8_t data)
{
	//上一次消息处理结束
	if(g_uartComData.Over == 0)
	{
		//开始
		if(data == UART_COM_STAT )
		{
			//长度清零
			g_uartComData.Len = 0;
		}
		//结束
		else if(data == UART_COM_END)
		{
			//接收结束
			g_uartComData.Over = 1;
		}
		//数据
		else
		{
			//写入数据
			g_uartComData.Buf[g_uartComData.Len] = data;
			//移动光标
			g_uartComData.Len = (g_uartComData.Len + 1) % 32;
		}
	}
}

/*Battery -------------------------------------------------------------------*/
//获得电池组电压  0 点火器电容 1 主控外设电池组
float getBatteryPackVoltage(uint8_t ch)
{
	switch (ch)
	{
		case 0:
		{
			//读取ADC通道值
			uint16_t adcval = get_adc(ADC_Channel_0);
			//计算ADC通道电压
			float vol = 3.3 * adcval / 4096;
			//分压比计算电池组电压
			vol = vol * (15000 + 750) / 750;
			//电阻误差补偿
			vol = vol * 1.0;
			return vol;
			break;
		}
		case 1:
		{
			//读取ADC通道值
			uint16_t adcval = get_adc(ADC_Channel_1);
			//计算ADC通道电压
			float vol = 3.3 * adcval / 4096;
			//分压比计算电池组电压
			vol = vol * (1500 + 750) / 750;
			//电阻误差补偿
			vol = vol * 1.0296296;
			return vol;
			break;
		}
	}
	return 0;
}

//返回电压值
void Adc_BatteryVoltage(void)
{
	uint8_t str[50];
	
	//开始
	str[0] = '<';
	
	//标识
	str[1] = 'V';
	
	//控制电池组电压
	float ctr = getBatteryPackVoltage(0);
	
	str[2] = (uint8_t)(ctr / 10  ) % 10 + 0x30;
	str[3] = (uint8_t)(ctr       ) % 10 + 0x30;
	str[4] = (uint8_t)(ctr * 10  ) % 10 + 0x30;
	str[5] = (uint8_t)(ctr * 100 ) % 10 + 0x30;
	
	//动力电池组电压
	float mtr = getBatteryPackVoltage(1);
	
	str[6] = (uint8_t)(mtr / 10  ) % 10 + 0x30;
	str[7] = (uint8_t)(mtr       ) % 10 + 0x30;
	str[8] = (uint8_t)(mtr * 10  ) % 10 + 0x30;
	str[9] = (uint8_t)(mtr * 100 ) % 10 + 0x30;
	
	//结束
	str[10] = '>';
	
	str[11] = 0;
	
	USART_SendString(USART1,str);
}
/*Igniter control -----------------------------------------------------------------*/

//点火控制器信息反馈
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
//点火器控制
void igniterAnalyze(uint8_t * cmd)
{
	//点火指令 <FAE><FAD> 启动关闭升压 <FFE><FFD>启动关闭点火
	
	//升压控制
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

	
	//点火控制
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

//升压点火控制程序
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

//点火器控制GPIO初始化
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


//消息接收解析
void RxBufAnalysis(UartBufTypedef * buf)
{
	//小完整性被使能
	if(buf->Over == 1)
	{	
		//通信协议
		//开始 ‘<’ 结束 '>'
		//[0] 标识 [n] 数据
		switch(buf->Buf[0])
		{
			case 'I'://点火器信息反馈
			{
				igniterFeedBack();
				break;
			}
			case 'V'://读取电池组数据
			{
				Adc_BatteryVoltage();
				break;
			}
			case 'F'://点火指令 <FAE><FAD> 启动关闭升压 <FFE><FFD>启动关闭点火
			{
				igniterAnalyze(buf->Buf);
				break;
			}
		}
		//清除未完成标志位
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
	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	//延时初始化
	delay_init();

	//PWM通道初始化
	//TIM2_PWM_Init(1000-1,2-1);
	//TIM3_PWM_Init(1000-1,2-1); 
	//串口通信初始化
	USART1_Initialise(115200);  
	//配置串口接收函数
	USART_RxFuncConfig(USART1,UsartRxToBuf);
	//LED初始化
	led_init(); 
	//ADC转换初始化
	ADC_Init_quantity();
	//点火器控制GPIO初始化
	igniterInit();
	
	USART_SendString(USART1,"System Start ok\r\n");

	while (1)
	{
		//接收消息解析
		RxBufAnalysis(&g_uartComData);
		//LED呼吸灯
		led_running();
		
		
		//升压点火控制程序
		igniterCtrl();
		
	}	 
	return 0;
}


