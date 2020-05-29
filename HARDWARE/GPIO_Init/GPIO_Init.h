#ifndef _GPIO_INIT_H_	
#define _GPIO_INIT_H_

#ifdef __cplusplus			//???CPP??C??
extern "C" {
#endif

#include "config.h"

#define ON  1
#define OFF 0


#define KEY_RUNBRAKE       	  GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) //开启按钮
#define KEY_STATE       		  GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) //速度方向开启按钮
#define KEY_SPEED_HIGHT       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) //速度档位开启按钮
#define KEY_DIY								GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) //预留按钮

//

	
	/* HALL宏定义 ----------------------------------------------------------------*/
#define HALL_RCC_CLOCKGPIO            RCC_AHB1Periph_GPIOB
#define HALL_GPIO_PORTSOURCE          GPIO_PortSourceGPIOB
#define HALL_U_V_W_EXTI_IRQn          EXTI9_5_IRQn
#define HALL_GPIO                     GPIOB

#define HALL_U_GPIO_PIN               GPIO_Pin_7
#define HALL_U_GPIO_PINSOURCE         GPIO_PinSource7
#define HALL_U_EXITLINE               EXTI_Line7
#define HALL_V_GPIO_PIN               GPIO_Pin_8
#define HALL_V_GPIO_PINSOURCE         GPIO_PinSource8
#define HALL_V_EXITLINE               EXTI_Line8
#define HALL_W_GPIO_PIN               GPIO_Pin_9
#define HALL_W_GPIO_PINSOURCE         GPIO_PinSource9
#define HALL_W_EXITLINE               EXTI_Line9

void GPIO_Config(void);
void HALL_GPIO_Init(void);
void GPIO_WConfig(void);


#ifdef __cplusplus		  
}
#endif

#endif
