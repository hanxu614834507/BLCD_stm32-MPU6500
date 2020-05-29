#include "config.h"
/****************************************************************************
* ����:void  BSP_Init(void)
* ����:������ʼ��
* ��ڲ���:��
* ���ڲ���:��
* ˵��:��
****************************************************************************/


void BSP_Init(void)
{
	SystemInit();	
	delay_init(168);
	GPIO_Config();
	LED_Init();
	delay_ms(10);
	KEY_Init();
	HALL_GPIO_Init();
	TIM2_Init();
	//current_Init();
	//CAN_Config();
	BLDC_TIMx_PWM_Init();
	//mpu6500_init();
  uart_init(115200);  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
}

