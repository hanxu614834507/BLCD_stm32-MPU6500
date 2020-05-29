/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "config.h"

uint16_t speed_duty; // �ٶ�ռ�ձȣ�0~1000  Ϊ1000��ռ�ձ�Ϊ100%
                              // ��ʼ��ֵ���벻С��70�������������

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ����TIMx�������PWMʱ�õ���I/O
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void BLDC_TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* ʹ�ܶ�ʱ��ͨ������GPIOʱ�� */
  RCC_AHB1PeriphClockCmd(BLDC_TIM_GPIO_CLK, ENABLE); //GPIOE

  /* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH1_PIN | BLDC_TIM_CH2_PIN |BLDC_TIM_CH3_PIN | BLDC_TIM_CH1N_PIN | BLDC_TIM_CH2N_PIN | BLDC_TIM_CH3N_PIN;			//PE9
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BLDC_TIM_CH1_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(BLDC_TIM_CH1_PORT,BLDC_TIM_CH1_PINSOURCE,GPIO_AF_TIM1); //GPIOE9����Ϊ��ʱ��1
	GPIO_PinAFConfig(BLDC_TIM_CH1_PORT,BLDC_TIM_CH2_PINSOURCE,GPIO_AF_TIM1); //GPIOE11����Ϊ��ʱ��1
  GPIO_PinAFConfig(BLDC_TIM_CH1_PORT,BLDC_TIM_CH3_PINSOURCE,GPIO_AF_TIM1); //GPIOE13����Ϊ��ʱ��1
	GPIO_PinAFConfig(BLDC_TIM_CH1N_PORT,BLDC_TIM_CH1N_PINSOURCE,GPIO_AF_TIM1); //GPIOE8����Ϊ��ʱ��1����ͨ��1
	GPIO_PinAFConfig(BLDC_TIM_CH2N_PORT,BLDC_TIM_CH2N_PINSOURCE,GPIO_AF_TIM1); //GPIOE10����Ϊ��ʱ��1����ͨ��2
	GPIO_PinAFConfig(BLDC_TIM_CH3N_PORT,BLDC_TIM_CH3N_PINSOURCE,GPIO_AF_TIM1); //GPIOE12����Ϊ��ʱ��1����ͨ��3
//  /* Configuration: BKIN pin */
//  GPIO_InitStructure.GPIO_Pin = BLDC_TIM_BKIN_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(BLDC_TIM_BKIN_PORT, &GPIO_InitStructure);  
}

/**
  * ��������: ����TIMx�����PWM�źŵ�ģʽ�������ڡ����ԡ�ռ�ձ�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> TIMxCNT ���¼���
 *                    TIMx_CCR(��ƽ�����仯)
 * �ź�����=(TIMx_ARR +1 ) * ʱ������
 * ռ�ձ�=TIMx_CCR/(TIMx_ARR +1)
 */
static void BLDC_TIMx_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
 
	/* ʹ�ܶ�ʱ��ʱ�� */
  BLDC_TIM_APBxClock_FUN(BLDC_TIM_CLK, ENABLE);//TIM1ʱ��ʹ��
	TIM_DeInit(TIM1);
	
  /* ��ʱ����������ʱ�� */		 
  TIM_TimeBaseStructure.TIM_Period = BLDC_TIM_PERIOD; 
  /* ����Ԥ��Ƶ����Ԥ��Ƶ����Ϊ168MHz */
  TIM_TimeBaseStructure.TIM_Prescaler = BLDC_TIM_PRESCALER;
  /* ����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���) */
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
  /* ���ϼ���ģʽ */
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  /* �ظ������� */
  TIM_TimeBaseStructure.TIM_RepetitionCounter = BLDC_TIM_REPETITIONCOUNTER;//���BLDC_TIM_REPETITIONCOUNTER�ν����ж�
  TIM_TimeBaseInit(BLDC_TIMx, &TIM_TimeBaseStructure);

  /* ��ʱ�����ͨ��1ģʽ���� */
  /* ģʽ���ã�PWMģʽ1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  /* ���״̬���ã�ʹ����� */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  /* ����ͨ�����״̬���ã�ʹ����� */
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  /* ��������ֵ�������������������ֵʱ����ƽ�������� */
  TIM_OCInitStructure.TIM_Pulse = BLDC_TIM_PERIOD;
  /* ����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ */
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//������ԱȽϵ�
  TIM_OCInitStructure.TIM_OCNPolarity= TIM_OCNPolarity_High;//���û������ԱȽϵ�
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//���������״̬
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;//�����󻥲������״̬
  /* ��ʼ����ʱ��ͨ��1���PWM */
  TIM_OC1Init(BLDC_TIMx, &TIM_OCInitStructure);
  
  /* ��ʱ�����ͨ��2ģʽ���� */
  /* ����ͨ��2�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM */
  TIM_OCInitStructure.TIM_Pulse = BLDC_TIM_PERIOD;
  /* ��ʼ����ʱ��ͨ��2���PWM */
  TIM_OC2Init(BLDC_TIMx, &TIM_OCInitStructure);

  /* ��ʱ�����ͨ��3ģʽ���� */
  /* ����ͨ��3�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM */
  TIM_OCInitStructure.TIM_Pulse = BLDC_TIM_PERIOD;
  /* ��ʼ����ʱ��ͨ��3���PWM */
  TIM_OC3Init(BLDC_TIMx, &TIM_OCInitStructure);

  /* �Զ�������ã��жϣ���ʱ�䣬������*/
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;//������ģʽ�¡��ر�״̬��ʹ��
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;//�ر�ģʽ�¡��ر�״̬��ʹ��
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;//�����ر�
  TIM_BDTRInitStructure.TIM_DeadTime =0xDF;//����ʱ��
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;//ɲ������ʹ��
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;//ɲ������ߵ�ƽ��Ч
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//�Զ����ʹ��
	TIM_BDTRInitStructure.TIM_OSSIState=TIM_OSSIState_Disable;//TIM_OSSIState_Disable;//TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_OSSRState=TIM_OSSRState_Disable;//TIM_OSSRState_Disable;//TIM_OSSRState_Enable;
  TIM_BDTRConfig(BLDC_TIMx, &TIM_BDTRInitStructure);
  
  TIM_OC1PreloadConfig(BLDC_TIMx,TIM_OCPreload_Enable);//ͨ�����д����Ʋ��ε�ֵ��һ���¼�������ʱ��Ч
  TIM_OC2PreloadConfig(BLDC_TIMx,TIM_OCPreload_Enable);
  TIM_OC3PreloadConfig(BLDC_TIMx,TIM_OCPreload_Enable);
  
  /* ʹ�ܶ�ʱ�����ؼĴ���ARR */
  TIM_ARRPreloadConfig(BLDC_TIMx, ENABLE);
	//TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	
  /* ʹ�ܶ�ʱ�� */
  TIM_Cmd(BLDC_TIMx, ENABLE);
  
  /* TIM�����ʹ�� */
  TIM_CtrlPWMOutputs(BLDC_TIMx, ENABLE);
  
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
}

/**
  * ��������: TIMx ���PWM�źų�ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����ֻҪ�����������TIMx������ͨ���ͻ���PWM�ź����
  */
void BLDC_TIMx_PWM_Init(void)
{
	speed_duty=700;		//�ٶ�ռ�ձ�30%
	BLDC_TIMx_GPIO_Config();
	BLDC_TIMx_Configuration();
}



/**
  * ��������: ��ˢ��������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void BLDC_PHASE_CHANGE(uint8_t step)
{
  // H_PWM_L_ON
  switch(step)
  {
    case 6: //B+ C-
      /* Next step: Step 2 Configuration -------------------------------------- */ 
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
		  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
			TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      /*  Channel1 configuration */
      /*  Channel2 configuration */    
      TIM_SetCompare2(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Enable);
      /*  Channel3 configuration */
      TIM_SetCompare3(BLDC_TIMx,0);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Enable);
      break;
    case 2: //B+ A-
      /* Next step: Step 3 Configuration -------------------------------------- */      
			TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
		  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
			TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,0);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Enable);
    
      /*  Channel2 configuration */
      TIM_SetCompare2(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Enable);
      /*  Channel3 configuration */
      break;
    case 3: //C+ A-
      /* Next step: Step 4 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
		  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
			TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,0);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Enable);
      
      /*  Channel2 configuration */ 
      /*  Channel3 configuration */
      TIM_SetCompare3(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Enable);
      break;
    case 1: //C+ B-
      /* Next step: Step 5 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
		  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
			TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);  
    
      /*  Channel1 configuration */      
      /*  Channel2 configuration */   
      TIM_SetCompare2(BLDC_TIMx,0);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Enable);
    
      /*  Channel3 configuration */      
      TIM_SetCompare3(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Enable);
      break;
    case 5: //A+ B-
      /* Next step: Step 6 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
		  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
			TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Enable);
      /*  Channel2 configuration */
      TIM_SetCompare2(BLDC_TIMx,0);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Enable);
      /*  Channel3 configuration */
      break;
    case 4: //A+ C-
      /* Next step: Step 1 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
		  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
			TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Enable);
      /*  Channel2 configuration */      
      /*  Channel3 configuration */
      TIM_SetCompare3(BLDC_TIMx,0);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Enable);
      break;
    default:
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      break;
  }
#if 0 // H_ON_L_PWN
  switch(step)
  {
    case 4: //B+ C-
      /* Next step: Step 2 Configuration -------------------------------------- */ 
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
      
      /*  Channel1 configuration */
      /*  Channel2 configuration */    
      TIM_SetCompare2(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Enable);
      /*  Channel3 configuration */
      TIM_SetCompare3(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Enable);
      break;
    case 5: //B+ A-
      /* Next step: Step 3 Configuration -------------------------------------- */      
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Enable);
    
      /*  Channel2 configuration */
      TIM_SetCompare2(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Enable);
      /*  Channel3 configuration */
      break;
    case 1: //C+ A-
      /* Next step: Step 4 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
    
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Enable);
      
      /*  Channel2 configuration */ 
      /*  Channel3 configuration */
      TIM_SetCompare3(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Enable);
      break;
    case 3: //C+ B-
      /* Next step: Step 5 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);    
    
      /*  Channel1 configuration */      
      /*  Channel2 configuration */   
      TIM_SetCompare2(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Enable);
    
      /*  Channel3 configuration */      
      TIM_SetCompare3(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Enable);
      break;
    case 2: //A+ B-
      /* Next step: Step 6 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Enable);
      /*  Channel2 configuration */
      TIM_SetCompare2(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Enable);
      /*  Channel3 configuration */
      break;
    case 6: //A+ C-
      /* Next step: Step 1 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
      
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Enable);
      /*  Channel2 configuration */      
      /*  Channel3 configuration */
      TIM_SetCompare3(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Enable);
      break;
    default:
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      break;
  }
#endif
}


void BLDC_PHASE_STOP(void)
{
		EXTI_ClearITPendingBit(HALL_U_EXITLINE|HALL_V_EXITLINE|HALL_W_EXITLINE);
		NVIC_DisableIRQ(HALL_U_V_W_EXTI_IRQn);
		TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
		TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
		TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
		TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
		TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
		TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
		delay_ms(5);
		TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Enable);
		TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Enable);
		TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Enable);
		while(bldc_dev.stalling_count<1600);
		TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
		TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
		TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
		bldc_dev.motor_state=STOP;	
}

