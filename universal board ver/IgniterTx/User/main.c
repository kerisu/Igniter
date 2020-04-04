/**
  ******************************************************************************
  * @file    main.c
  * @author  张东
  * @version V1.0.0
  * @date    2020-1-3
  * @brief   点火器控制器主程序
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

#define UART_COM_STAT   '<'	//串口通信开始字节
#define UART_COM_END		'>'	//串口通信结束字节

static UartBufTypedef g_uartComData = {.Over = 0 ,.Len = 0};//串口接收缓冲区

uint8_t g_FAState = '-';		//升压控制
uint8_t g_FFState = '-';		//点火控制

uint16_t g_Ebt = 0;				//电池电压
uint16_t g_Efr = 0;				//点火电压

uint8_t  g_locked = 1;		//锁定标志位
uint32_t g_Ping = 4000;		//接收端延迟


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

/*12864 LCD display -----------------------------------------------------------*/

//客户机状态显示
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

//点火器状态显示
void IgniterStateDisplay(uint8_t * cmd)
{
	g_FAState = cmd[2];
	g_FFState = cmd[1];
	
	OptionDisplay();
}
//点火器电压显示
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

//锁定状态显示
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

//充电状态显示
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

//刷新频率10Hz
void lcd_display_div(void)
{
	static uint32_t div = 0;
	div++;
	if(div > 100)//刷新频率 10Hz
	{
		div = 0;
		LCD_Display_Update();
	}
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
			case 'A'://点火器信息反馈
			{
				IgniterStateDisplay(buf->Buf);
				break;
			}
			case 'V'://电压信息显示
			{
				IgniterVoltageDisplay(buf->Buf);
				break;
			}
			
		}
		//清除未完成标志位
		buf->Over = 0;
		//消息间隔清零
		g_Ping = 0;
	}
}

//状态获取请求
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
	
	//功能键
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
			//上边沿触发
			if(keyboardhist == 0)
			{
				//解锁
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
				else//锁定 
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
		
		//起爆器充电
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
		//起爆器点火
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
	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	//延时初始化
	delay_init();

	//串口通信初始化
	USART2_Initialise(115200);  
	//配置串口接收函数
	USART_RxFuncConfig(USART2,UsartRxToBuf);
	//LED初始化
	led_init(); 
	//键盘初始化
	keyboardInit();
	//12864初始化
	Lcd_Init();
	Lcd_setup_display();
	
	USART_SendString(USART2,"System Start ok\r\n");

	while (1)
	{
		//客户机状态显示
		ClientStateDisplay();
		
		//接收消息解析
		RxBufAnalysis(&g_uartComData);
		//LED呼吸灯
		led_running();
		//显示刷新分频器
		lcd_display_div();
		
		//按键扫描
		keyScan();
		//状态获取请求
		sendStateRequest();
		
		delay_ms(1);
	}	 
	return 0;
}


