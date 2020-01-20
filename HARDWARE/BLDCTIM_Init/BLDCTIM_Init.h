#ifndef __BLDCTIM_INIT_H__
#define __BLDCTIM_INIT_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f4xx.h>

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
/********************高级定时器TIM参数定义，只限TIM1 & TIM8************/
#define BLDC_TIMx                       TIM1
#define BLDC_TIM_APBxClock_FUN          RCC_APB2PeriphClockCmd
#define BLDC_TIM_CLK                    RCC_APB2Periph_TIM1

#define BLDC_TIM_GPIO_CLK               (RCC_AHB1Periph_GPIOE)
#define BLDC_TIM_CH1_PORT               GPIOE
#define BLDC_TIM_CH1_PIN                GPIO_Pin_9
#define BLDC_TIM_CH2_PORT               GPIOE
#define BLDC_TIM_CH2_PIN                GPIO_Pin_11
#define BLDC_TIM_CH3_PORT               GPIOE
#define BLDC_TIM_CH3_PIN                GPIO_Pin_13

#define BLDC_TIM_CH1_PINSOURCE					GPIO_PinSource9
#define BLDC_TIM_CH2_PINSOURCE					GPIO_PinSource11
#define BLDC_TIM_CH3_PINSOURCE					GPIO_PinSource13

#define BLDC_TIM_CH1N_PORT              GPIOE
#define BLDC_TIM_CH1N_PIN               GPIO_Pin_8
#define BLDC_TIM_CH2N_PORT              GPIOE
#define BLDC_TIM_CH2N_PIN               GPIO_Pin_10
#define BLDC_TIM_CH3N_PORT              GPIOE
#define BLDC_TIM_CH3N_PIN               GPIO_Pin_12

#define BLDC_TIM_CH1N_PINSOURCE					GPIO_PinSource8
#define BLDC_TIM_CH2N_PINSOURCE					GPIO_PinSource10
#define BLDC_TIM_CH3N_PINSOURCE					GPIO_PinSource12
//#define BLDC_TIM_BKIN_PORT              GPIOB
//#define BLDC_TIM_BKIN_PIN               GPIO_Pin_12

#define BLDC_TIM_PWM_FREQ               20000 // PWM频率

// 定义定时器预分频，定时器实际时钟频率为：84MHz/（BLDC_TIMx_PRESCALER+1）
#define BLDC_TIM_PRESCALER              0  // 实际时钟频率为：168MHz

// 定义定时器周期，当定时器开始计数到BLDC_TIMx_PERIOD值并且重复计数寄存器为0时更新定时器并生成对应事件和中断  168/(168+1)/20000
#define BLDC_TIM_PERIOD                 (uint16_t)(SystemCoreClock/(BLDC_TIM_PRESCALER+1)/BLDC_TIM_PWM_FREQ)

// 定义高级定时器重复计数寄存器值，
#define BLDC_TIM_REPETITIONCOUNTER       0

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void BLDC_TIMx_PWM_Init(void);
void BLDC_PHASE_CHANGE(uint8_t step);
void BLDC_PHASE_STOP(void);

extern  uint16_t speed_duty; // 速度占空比：0~1000  为1000是占空比为100%
                              // 初始化值必须不小于70，否则电机会堵塞

#endif	/* __BLDC_TIM_H__ */
