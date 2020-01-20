#include "config.h"
/****************************************************************************
* 名称:void  BSP_Init(void)
* 功能:驱动初始化
* 入口参数:无
* 出口参数:无
* 说明:无
****************************************************************************/


void BSP_Init(void)
{
	SystemInit();
	TIM2_Init();	
	delay_init(168);
	GPIO_Config();
	HALL_GPIO_Init();
	current_Init();
	CAN_Config();
	BLDC_TIMx_PWM_Init();
	LED_Init();
	mpu6500_init();
  uart_init(115200);  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

