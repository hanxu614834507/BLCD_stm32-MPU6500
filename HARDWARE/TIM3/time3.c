#include "config.h"

void TIM3_Init(void)
{
		TIM3_NVIC_Configuration();
		TIM3_Configuration();
}
void TIM3_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
 													
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*�ж�����Ϊ1ms*/
void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Period=29999;		 								/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler= 16799;				    /* ʱ��Ԥ��Ƶ�� Tout=((period+1)*(prescaler+1))/10000  50.4ms.*/
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* ������Ƶ */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* ���ϼ���ģʽ */
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);							    		/* �������жϱ�־ */
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM3, ENABLE);																		/* ����ʱ�� */
    
}



void TIM3_IRQHandler(void)
{
  /* ����жϱ�־λ */

	if ( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) 
	{	
		if((EXTI_GetITStatus(HALL_U_EXITLINE) != RESET)|(EXTI_GetITStatus(HALL_V_EXITLINE) != RESET)|(EXTI_GetITStatus(HALL_W_EXITLINE) != RESET))
		//HALL_EXTI_Callback();
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);
	}		
	
}

/**
  * ��������: �ⲿ�жϷ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void HALL_EXTI_Callback(void)
{
  __IO uint8_t uwStep = 0;
  static __IO int8_t uwStep0=0;
  
  uint16_t hall_read=(HALL_GPIO->IDR)&0x0380; // ��ȡ����������״̬ pin7 8 9
  
  if(bldc_dev.motor_state==STOP)return;
  
  uwStep=hall_read>>7; //�˴���Ҫ����ʵ�ʸ���˳��
  if(bldc_dev.motor_direction==CW) // �����ж�  
	uwStep=7-uwStep;
  BLDC_PHASE_CHANGE(uwStep);   // ��������
  if(uwStep0!=uwStep) // ȷ������Ѿ�����
  {    
    bldc_dev.step_counter++;
    bldc_dev.stalling_count=0;
  }
  uwStep0=uwStep;
}

