#include "usart.h"	 

/***********************************************************************************************************/
//																					USART1,2,3 ��ʼ��
/***********************************************************************************************************/

void USART1_Initialise(u32 bound){
//GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}


void USART2_Initialise( u32 bound )
{
    GPIO_InitTypeDef GPIO_InitStructure;//I/O?��3?��??��?��11��?
    NVIC_InitTypeDef NVIC_InitStructure;//?D??3?��??��?��11��?
    USART_InitTypeDef USART_InitStructure;//��??��3?��??��?��11��?  
    
    /* Enable the USART2 Pins Software Remapping */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_AFIO, ENABLE); 
    
    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
    
    USART_InitStructure.USART_BaudRate = bound;                
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     
    USART_InitStructure.USART_Parity = USART_Parity_No;        
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      
    
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    /* Enable USART2 */
    USART_Cmd(USART2, ENABLE);
		
}

void USART3_Initialise(u32 bt)    
    
{    
   USART_InitTypeDef USART_InitStructure;    
   NVIC_InitTypeDef  NVIC_InitStructure;     
   GPIO_InitTypeDef  GPIO_InitStructure;    //����һ���ṹ�������������ʼ��GPIO   
	
   //ʹ�ܴ��ڵ�RCCʱ��    
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //ʹ��UART3����GPIOB��ʱ��    
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);    
    
   // Configure USART2 Rx (PB.11) as input floating      
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);    
    
   // Configure USART2 Tx (PB.10) as alternate function push-pull    
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);    
    
   //���ô���    
   USART_InitStructure.USART_BaudRate = bt;    
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;    
   USART_InitStructure.USART_StopBits = USART_StopBits_1;    
   USART_InitStructure.USART_Parity = USART_Parity_No;    
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;       
   USART_Init(USART3, &USART_InitStructure);      
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);          
   USART_Cmd(USART3, ENABLE);   
     
	 NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);         
}  

/***********************************************************************************************************/
//																			USART1,2,3 ����ͨ�Ų���
/***********************************************************************************************************/
/*-------------------------------------------------------��������*/
//���ڽ��ܺ���
void (* g_usartRxFunc[3])(uint8_t data);

//���ڽ�������
void USART_RxFuncConfig(USART_TypeDef* USARTx , void (* func)(uint8_t data))
{
	if(USARTx == USART1)
	{
		g_usartRxFunc[0] = func;
	}
	else	if(USARTx == USART2)
	{
		g_usartRxFunc[1] = func;
	}
	else	if(USARTx == USART3)
	{
		g_usartRxFunc[2] = func;
	}
}


/*-------------------------------------------------------���ڽ���*/
void USART1_IRQHandler(void)        
{
	uint8_t bt;
	if(USART1->SR&(1<<5))
	{	 
		bt = (USART1->DR&(uint8_t)0x00FF);
		g_usartRxFunc[0](bt);
	}
} 
void USART2_IRQHandler(void)  
{
	uint8_t bt;
	if(USART2->SR&(1<<5))
	{	 
		bt = (USART2->DR&(uint8_t)0x00FF);
		g_usartRxFunc[1](bt);
	}
}


void USART3_IRQHandler(void)
{
	uint8_t bt;
	if(USART3->SR&(1<<5))
	{	 
		bt = (USART3->DR&(uint8_t)0x00FF);
		g_usartRxFunc[2](bt);
	}
}

/*--------------------------------------------------------���ڷ���*/
void USART_SendString(USART_TypeDef* USARTx,uint8_t * Send)
{
    while (*Send)                  //����ַ���������־
    {
			  while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);  //�ȴ����ͽ���
        USART_SendData(USARTx,*Send++);                        //��ESP��������
    }
}
















