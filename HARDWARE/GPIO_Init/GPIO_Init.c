#include "GPIO_Init.h"

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_WConfig(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOFʱ��

  //GPIOD11,F10��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;//LED0��LED1��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIO
	GPIO_ResetBits(GPIOE,GPIO_Pin_12 | GPIO_Pin_13);
}
void GPIO_Config(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOFʱ��

  //GPIOD11,F10��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13  | GPIO_Pin_14  ;//LED0��LED1��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIO
	
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13;//LED0��LED1��ӦIO��
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIO
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//LED0��LED1��ӦIO��
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
// GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIO
//	ctrl brake	�ƶ�												 
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;									
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//?????50M
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//???????????	
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
//    GPIO_Init(GPIOD, &GPIO_InitStructure);	
//		GPIO_ResetBits(GPIOD, GPIO_Pin_7 );


//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;   				//PWM IN
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
//	
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//	 
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void HALL_GPIO_Init(void)
{
   /* ����Ӳ��IO��ʼ���ṹ�����*/
  GPIO_InitTypeDef GPIO_InitStructure;
  /*�����ⲿ�ж��߳�ʼ���ṹ�����*/
  EXTI_InitTypeDef EXTI_InitStructure;
  /* ����Ƕ�������жϿ�������ʼ���ṹ����� */
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ϊ����IO�����жϹ�����Ҫʹ��ʱ�� */
  RCC_AHB1PeriphClockCmd(HALL_RCC_CLOCKGPIO,ENABLE);
  
  /* �趨����IO��� */
  GPIO_InitStructure.GPIO_Pin = HALL_U_GPIO_PIN;  
  /* �趨����IOΪ����ģʽ */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  /* ��ʼ��HALL_U��Ӧ����IO */
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
   /* ��ʼ��HALL_V��Ӧ����IO */
  GPIO_InitStructure.GPIO_Pin = HALL_V_GPIO_PIN; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
   /* ��ʼ��HALL_W��Ӧ����IO */
  GPIO_InitStructure.GPIO_Pin = HALL_W_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* ѡ����Ϊ�ж�����Դ */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,HALL_U_GPIO_PINSOURCE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,HALL_V_GPIO_PINSOURCE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,HALL_W_GPIO_PINSOURCE);
  
  /* ��Ӧ���ж��� */
  EXTI_InitStructure.EXTI_Line=HALL_U_EXITLINE|HALL_V_EXITLINE|HALL_W_EXITLINE;
  /* �ⲿ�ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  /* �����½��ش�����ʽ */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  /* ʹ���ж� */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  /* ����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ��� */
  EXTI_Init(&EXTI_InitStructure);
  
  /* ʹ���ⲿ�ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannel = HALL_U_V_W_EXTI_IRQn;
  /* ������ռ���ȼ�Ϊ1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  /* ���������ȼ�Ϊ1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  /* ʹ���ⲿ�ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
  /* ��ʼ������Ƕ�������жϿ����� */
  NVIC_Init(&NVIC_InitStructure); 
    /* ʹ���ⲿ�ж�ͨ�� */
//  NVIC_InitStructure.NVIC_IRQChannel = HALL_VEXTI_IRQn;
//  /* ������ռ���ȼ�Ϊ1 */
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
//  /* ���������ȼ�Ϊ1 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//  /* ʹ���ⲿ�ж�ͨ�� */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	
//  NVIC_InitStructure.NVIC_IRQChannel = HALL_W_EXTI_IRQn;
//  /* ������ռ���ȼ�Ϊ1 */
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
//  /* ���������ȼ�Ϊ1 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//  /* ʹ���ⲿ�ж�ͨ�� */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
  /* ��ʼ������Ƕ�������жϿ����� */
  NVIC_Init(&NVIC_InitStructure); 
  
}



