/*-----------------------------------------------------
********IO�ڷ�ת˵�������Motor Control Board��********
***************PWM1���Ŷ�ӦPE14************************
***************PWM2���Ŷ�ӦPE13************************
-----------------------------------------------------*/
#include "config.h"

void TIM2_Init(void)
{
		TIM2_NVIC_Configuration();
		TIM2_Configuration();
}
void TIM2_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
 													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*�ж�����Ϊ1ms*/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=9;		 								/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler= 16799;				    /* ʱ��Ԥ��Ƶ�� 168M/16800 */ //100us
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* ������Ƶ */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* ���ϼ���ģʽ */
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    		/* �������жϱ�־ */
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);																		/* ����ʱ�� */
    
}



void TIM2_IRQHandler(void)
{
  /* ����жϱ�־λ */

	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{	
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
		Tim2_VCP_Callback();
	}		
	
}



/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
