#ifndef _GPIO_INIT_H_	
#define _GPIO_INIT_H_

#ifdef __cplusplus			//???CPP??C??
extern "C" {
#endif

#include "config.h"

#define ON  0
#define OFF 1

#define KEY_SPEED_HIGHT       GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13)
#define KEY_RUNBRAKE       	  GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8) //¿ªÆô°´Å¥
#define KEY_DIR       		    GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)	



	
	/* ???? ------------------------------------------------------------------*/
/* ??? --------------------------------------------------------------------*/
#define HALL_RCC_CLOCKGPIO            RCC_AHB1Periph_GPIOA
#define HALL_GPIO_PORTSOURCE          GPIO_PortSourceGPIOA
#define HALL_UEXTI_IRQn               EXTI3_IRQn
#define HALL_VEXTI_IRQn              EXTI4_IRQn
#define HALL_WEXTI_IRQn              EXTI9_5_IRQn
#define HALL_GPIO                     GPIOE

#define HALL_U_GPIO_PIN               GPIO_Pin_1
#define HALL_U_GPIO_PINSOURCE         GPIO_PinSource1
#define HALL_U_EXITLINE               EXTI_Line1
#define HALL_V_GPIO_PIN               GPIO_Pin_2
#define HALL_V_GPIO_PINSOURCE         GPIO_PinSource2
#define HALL_V_EXITLINE               EXTI_Line2
#define HALL_W_GPIO_PIN               GPIO_Pin_3
#define HALL_W_GPIO_PINSOURCE         GPIO_PinSource3
#define HALL_W_EXITLINE               EXTI_Line3

void GPIO_Config(void);
void HALL_GPIO_Init(void);



#ifdef __cplusplus		  
}
#endif

#endif
