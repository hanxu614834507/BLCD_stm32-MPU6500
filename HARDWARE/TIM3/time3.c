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

/*中断周期为1ms*/
void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Period=29999;		 								/* 自动重装载寄存器周期的值(计数值) */
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= 16799;				    /* 时钟预分频数 Tout=((period+1)*(prescaler+1))/10000  50.4ms.*/
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* 采样分频 */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* 向上计数模式 */
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);							    		/* 清除溢出中断标志 */
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM3, ENABLE);																		/* 开启时钟 */
    
}



void TIM3_IRQHandler(void)
{
  /* 清除中断标志位 */

	if ( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) 
	{	
		if((EXTI_GetITStatus(HALL_U_EXITLINE) != RESET)|(EXTI_GetITStatus(HALL_V_EXITLINE) != RESET)|(EXTI_GetITStatus(HALL_W_EXITLINE) != RESET))
		//HALL_EXTI_Callback();
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);
	}		
	
}

/**
  * 函数功能: 外部中断服务函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void HALL_EXTI_Callback(void)
{
  __IO uint8_t uwStep = 0;
  static __IO int8_t uwStep0=0;
  
  uint16_t hall_read=(HALL_GPIO->IDR)&0x0380; // 获取霍尔传感器状态 pin7 8 9
  
  if(bldc_dev.motor_state==STOP)return;
  
  uwStep=hall_read>>7; //此处需要根据实际更改顺序
  if(bldc_dev.motor_direction==CW) // 方向判断  
	uwStep=7-uwStep;
  BLDC_PHASE_CHANGE(uwStep);   // 驱动换相
  if(uwStep0!=uwStep) // 确保电机已经换相
  {    
    bldc_dev.step_counter++;
    bldc_dev.stalling_count=0;
  }
  uwStep0=uwStep;
}

