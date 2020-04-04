#include "stm32f10x.h"
#include "pulser.h"
/***********************************************************************************************************/
//	
//										TIM2 3 4 PWM�������
//
/***********************************************************************************************************/
void TIM2_PWM_Init(u16 arr,u16 psc)
{  
  GPIO_InitTypeDef         GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef        TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);// 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   											//ʹ��GPIO����ʱ��ʹ��                                                                                                                                                    
																																							  //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; 		//TIM_CH1234
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 													  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	TIM_TimeBaseStructure.TIM_Period = arr; 																		  //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ    
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 																		//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision =0; 																	//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  									//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 															//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
																																								//TIM3ͨͨ��һ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 														//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState= TIM_OutputState_Enable; 									//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; 																						//���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 											//�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  																		//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
																																							
  TIM_OCInitStructure.TIM_OutputState =TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
																																							
  TIM_OCInitStructure.TIM_OutputState =TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
																																							
  TIM_OCInitStructure.TIM_OutputState =TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

  TIM_CtrlPWMOutputs(TIM2,ENABLE);        																			//MOE �����ʹ��        
  TIM_ARRPreloadConfig(TIM2, ENABLE); 																					//ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
  TIM_Cmd(TIM2, ENABLE);  																											//ʹ��TIM2
}



void TIM3_PWM_Init(u16 arr,u16 psc)
{  
  GPIO_InitTypeDef         GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef        TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);// 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE);   //ʹ��GPIO����ʱ��ʹ��
                                                                                                                                                                
																																							  //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7 ; 											  //TIM_CH12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 														  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1 ; 											  //TIM_CH34
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  														  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = arr; 																		  //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ    
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 																		//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision =0; 																	//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  									//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 															//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
																																								//TIM3ͨͨ��һ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 														//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState= TIM_OutputState_Enable; 									//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; 																						//���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 											//�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  																		//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
																																								//TIM3ͨ����
  TIM_OCInitStructure.TIM_OutputState =TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
																																								//TIM3ͨ����
  TIM_OCInitStructure.TIM_OutputState =TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
																																								//TIM3ͨ����
  TIM_OCInitStructure.TIM_OutputState =TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

  TIM_CtrlPWMOutputs(TIM3,ENABLE);        																			//MOE �����ʹ��        
  TIM_ARRPreloadConfig(TIM3, ENABLE); 																					//ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
  TIM_Cmd(TIM3, ENABLE);  																											//ʹ��TIM3
}
void TIM4_PWM_Init(u16 arr,u16 psc)
{  
  GPIO_InitTypeDef         GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef        TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);												
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  												//ʹ��GPIO����ʱ��ʹ��
                                                                                                                                                                
																																								//���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9 ; 	//TIM_CH1 2 3 4 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  															//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = arr; 																			//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ    
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 																		//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision =0; 																	//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  									//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 															//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
																																								//TIM4ͨͨ��һ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 														//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState= TIM_OutputState_Enable; 									//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; 																					//���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 											//�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  																		//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
																																								//TIM4ͨ����
  TIM_OCInitStructure.TIM_OutputState =TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC2Init(TIM4, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
																																								//TIM4ͨ����
  TIM_OCInitStructure.TIM_OutputState =TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
																																								//TIM4ͨ����
  TIM_OCInitStructure.TIM_OutputState =TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC4Init(TIM4, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

  TIM_CtrlPWMOutputs(TIM4,ENABLE);       																			  //MOE �����ʹ��        
  TIM_ARRPreloadConfig(TIM4, ENABLE); 																			 	  //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
  TIM_Cmd(TIM4, ENABLE);  																										  //ʹ��TIM4
}

