#include "led.h" 
//////////////////////////////////////////////////////////////////////////////////	 

//LED IO��ʼ��
void LED_Init(void)
{  
	GPIO_ResetBits(GPIOD,GPIO_Pin_3);
	GPIO_SetBits(GPIOD,GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13  | GPIO_Pin_14 );//GPIOD11/12/13/14���øߣ�����
	//GPIO_SetBits(GPIOE,GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_13);
	//GPIO_ResetBits(GPIOE,GPIO_Pin_8);
	//delay_ms(500);
	//GPIO_ResetBits(GPIOD,GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13  | GPIO_Pin_14 );//GPIOD11/12/13/14���õͣ�����
}





