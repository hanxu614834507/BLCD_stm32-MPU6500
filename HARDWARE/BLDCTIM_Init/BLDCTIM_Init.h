#ifndef __BLDCTIM_INIT_H__
#define __BLDCTIM_INIT_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include <stm32f4xx.h>

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
/********************�߼���ʱ��TIM�������壬ֻ��TIM1 & TIM8************/
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

#define BLDC_TIM_PWM_FREQ               20000 // PWMƵ��

// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��168MHz/��BLDC_TIMx_PRESCALER+1��
#define BLDC_TIM_PRESCALER              168-1  // ʵ��ʱ��Ƶ��Ϊ��168MHz

// ���嶨ʱ�����ڣ�����ʱ����ʼ������BLDC_TIMx_PERIODֵ�����ظ������Ĵ���Ϊ0ʱ���¶�ʱ�������ɶ�Ӧ�¼����ж�  168000000/(167+1)/20000
#define BLDC_TIM_PERIOD                 (uint16_t)(SystemCoreClock/(BLDC_TIM_PRESCALER+1)/BLDC_TIM_PWM_FREQ)

// ����߼���ʱ���ظ������Ĵ���ֵ��
#define BLDC_TIM_REPETITIONCOUNTER       0

/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void BLDC_TIMx_PWM_Init(void);
void BLDC_PHASE_CHANGE(uint8_t step);
void BLDC_PHASE_STOP(void);

extern  uint16_t speed_duty; // �ٶ�ռ�ձȣ�0~1000  Ϊ1000��ռ�ձ�Ϊ100%
                              // ��ʼ��ֵ���벻С��70�������������

#endif	/* __BLDC_TIM_H__ */
