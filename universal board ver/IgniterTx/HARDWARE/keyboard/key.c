/**
  ******************************************************************************
  * @file    key.c
  * @author  ÕÅ¶«
  * @version V1.0.0
  * @date    2020-1-4
  * @brief   ¼üÅÌÇý¶¯
  ******************************************************************************
  */
/*include file ---------------------------------------------------------------*/
#include "key.h"
#include "stm32f10x.h"
/*LED ------------------------------------------------------------------------*/
void led_init(void)
{
	//SYSLED PC13
	//LOCKLED PB5
	
	GPIO_InitTypeDef gpio;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed  = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC,&gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_5;
	
	GPIO_Init(GPIOB,&gpio);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
}

void led_running(void)
{
	static uint16_t ph,div;
	static _Bool flag;
	div++;
	GPIOC->BSRR = GPIO_Pin_13 << 16 * (ph<div);
	if(div==10)
	{
		div = RESET;
		ph += !flag;
		ph -= flag;
		flag += (ph==10);
		flag *= !(ph==0);
	}
}

void led_lock(uint8_t cmd)
{
	if(cmd)
	{
		GPIOB->BRR = GPIO_Pin_5;
	}
	else
	{
		GPIOB->BSRR = GPIO_Pin_5;
	}
}

void keyboardInit(void)
{
	//FR PA8  CG PB15
	// 1b0 2b1 3b10 4b11

	GPIO_InitTypeDef gpio;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_15 ;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	gpio.GPIO_Speed  = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_8;
	
	GPIO_Init(GPIOA,&gpio);
}

uint8_t getFireKey(void)
{
	return (PAin(8) == 0);
}

uint8_t getChargeKey(void)
{
	return (PBin(15) == 0);
}

uint8_t getKeyboad(void)
{
	if(PBin(0) == 0)
	{
		return 1;
	}
	if(PBin(1) == 0)
	{
		return 2;
	}
	if(PBin(10) == 0)
	{
		return 3;
	}
	if(PBin(11) == 0)
	{
		return 4;
	}
	return 0;
}





