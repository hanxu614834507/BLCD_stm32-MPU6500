#ifndef _CONFIG_H_	
#define _CONFIG_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include <math.h>
#include <string.h>			//�ַ����Ƚ�
#include <ctype.h>			//��дת��
#include <stdlib.h>
#include <stdio.h>

#include "stm32f4xx.h"		//STM32�̼���
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_usart.h"
#include "bsp.h"			//�弶��ʼ��
#include "sys.h"
#include "time2.h"
#include "time3.h"
#include "delay.h"
#include "usart.h"
#include "current.h"
#include "ds18b20.h"
#include "mpu6500.h"
#include "led.h"
#include "pwm.h"
#include "can.h"
#include "key.h"
#include "adc.h"
#include "GPIO_Init.h"
#include "BLDCTIM_Init.h" 


//#include "usb_lib.h"
//#include "hw_config.h"
//#include "usb_pwr.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
typedef enum
{
  CW=0,  // ˳ʱ�뷽��(���ŵ���ῴ)��һ�㶨��Ϊ��ת
  CCW=1  // ��ʱ�뷽��(���ŵ���ῴ)��һ�㶨��Ϊ��ת
}MOTOR_DIR;

typedef enum 
{
  STOP=0,  // ͣ��
  RUN=1    // ����
}MOTOR_STATE;
//���ܿ�������
typedef struct SendControlCmd
{
	uint8_t GearPos;		//��λ
	uint8_t Value_L;		//���Ʊ仯��ֵ
	uint8_t Value_H;		//���Ʊ仯��ֵ
	uint8_t SpeedFlag;		//�ٶȵ�λ
}ControlCmdType;
extern ControlCmdType ControlDriveCmd;
//����ϴ���ECU��״̬��Ϣ
typedef struct MotorStateInformation
{
	uint8_t EnableState;	//���յ���ECUָ��ִ�е�״̬
	uint8_t	ModeState;		//������״̬����λ�źţ�
	uint8_t Value_L;		//ִ�е�ֵ
	uint8_t Value_H;		//ִ�е�ֵ
	uint8_t	ErrorCodeNum;	//���ֵĹ���
	uint8_t Reserved1;		//����ֵ
	uint8_t Reserved2;		//����ֵ
	uint8_t Reserved3;		//����ֵ
}MotorStateDataType;
typedef struct
{
  __IO int          motor_speed;        // ���ת��(RPM):0..3500
  __IO MOTOR_STATE  motor_state;        // �����ת״̬
  __IO MOTOR_DIR    motor_direction;    // ���ת������
  __IO uint32_t     step_counter;       // �������������������ڲ������ת��
  __IO uint16_t     stalling_count;     // ֹͣ��־�����ֵ����2000����Ϊ���ֹͣ��ת
}MOTOR_DEVICE;
extern MOTOR_DEVICE bldc_dev;
//����PID�ṹ��
typedef struct 
{
   __IO int      SetPoint;      //�趨Ŀ�� Desired Value
   __IO double   Proportion;    //�������� Proportional Const
   __IO double   Integral;      //���ֳ��� Integral Const
   __IO double   Derivative;    //΢�ֳ��� Derivative Const
   __IO int      LastError;     //Error[-1]
   __IO int      PrevError;     //Error[-2]
}PID;

/* ˽�к궨�� ----------------------------------------------------------------*/
/*************************************/
//����PID��غ�
//�����������趨�Ե������Ӱ��ǳ���
/*************************************/
#define  P_DATA                   0.5               //P����
#define  I_DATA                   0.06              //I����
#define  D_DATA                   0                 //D����

//#define  MOTOR_RATED_SPEED        2500              //ȷ��ת��
#define  MOTOR_MAX_SPEED          10              //����ת��
#define  MOTOR_MIN_SPEED          1             
#define  MOTOR_POLE_PAIR_NUM      4                 //������(8 �ż���)
#define  DRIVE_ID			0x44	//�������ID


/************************************/
#define LED_Current_OFF()   	GPIO_SsetBits(GPIOD, GPIO_Pin_11 );  	   //LED1 ��������ָʾ��
#define LED_Current_ON()  		GPIO_ResetBits(GPIOD, GPIO_Pin_11 ); 	  

#define LED_RUN_OFF()   	GPIO_SetBits(GPIOD, GPIO_Pin_12 );  	   //LED2 ����ָʾ��
#define LED_RUN_ON()  		GPIO_ResetBits(GPIOD, GPIO_Pin_12 ); 	  

#define LED_ECU_OFF()   	GPIO_SetBits(GPID, GPIO_Pin_13 );  //LED3  ��ECUʧ��ָʾ�� 
#define LED_ECU_ON()  		 GPIO_ResetBits(GPIOD, GPIO_Pin_13 );  	  

#define Ctrl_Brake_OFF()   	GPIO_SetBits(GPIOD, GPIO_Pin_14 );  	  
#define Ctrl_Brake_ON()  	GPIO_ResetBits(GPIOD, GPIO_Pin_14 ); 	  //�رյ�Դ����


extern const unsigned char Ascill_16[];

#define Enable_PrintfDebug

void DebugPrintf(const char *fmt,...);
void TIM1_PWM_Init(void);
void SendControlCmd(void);
void HALL_EXTI_Callback(void);
#ifdef __cplusplus		   //�����CPP����C���� //��������
}

#endif

#endif
