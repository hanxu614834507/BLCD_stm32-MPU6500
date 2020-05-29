/* 包含头文件 ----------------------------------------------------------------*/
#include "config.h"

uint16_t speed_duty; // 速度占空比：0~1000  为1000是占空比为100%
                              // 初始化值必须不小于70，否则电机会堵塞

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 配置TIMx复用输出PWM时用到的I/O
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void BLDC_TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* 使能定时器通道引脚GPIO时钟 */
  RCC_AHB1PeriphClockCmd(BLDC_TIM_GPIO_CLK, ENABLE); //GPIOE

  /* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH1_PIN | BLDC_TIM_CH2_PIN |BLDC_TIM_CH3_PIN | BLDC_TIM_CH1N_PIN | BLDC_TIM_CH2N_PIN | BLDC_TIM_CH3N_PIN;			//PE9
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BLDC_TIM_CH1_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(BLDC_TIM_CH1_PORT,BLDC_TIM_CH1_PINSOURCE,GPIO_AF_TIM1); //GPIOE9复用为定时器1
	GPIO_PinAFConfig(BLDC_TIM_CH1_PORT,BLDC_TIM_CH2_PINSOURCE,GPIO_AF_TIM1); //GPIOE11复用为定时器1
  GPIO_PinAFConfig(BLDC_TIM_CH1_PORT,BLDC_TIM_CH3_PINSOURCE,GPIO_AF_TIM1); //GPIOE13复用为定时器1
	GPIO_PinAFConfig(BLDC_TIM_CH1N_PORT,BLDC_TIM_CH1N_PINSOURCE,GPIO_AF_TIM1); //GPIOE8复用为定时器1互补通道1
	GPIO_PinAFConfig(BLDC_TIM_CH2N_PORT,BLDC_TIM_CH2N_PINSOURCE,GPIO_AF_TIM1); //GPIOE10复用为定时器1互补通道2
	GPIO_PinAFConfig(BLDC_TIM_CH3N_PORT,BLDC_TIM_CH3N_PINSOURCE,GPIO_AF_TIM1); //GPIOE12复用为定时器1互补通道3
//  /* Configuration: BKIN pin */
//  GPIO_InitStructure.GPIO_Pin = BLDC_TIM_BKIN_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(BLDC_TIM_BKIN_PORT, &GPIO_InitStructure);  
}

/**
  * 函数功能: 配置TIMx输出的PWM信号的模式，如周期、极性、占空比
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> TIMxCNT 重新计数
 *                    TIMx_CCR(电平发生变化)
 * 信号周期=(TIMx_ARR +1 ) * 时钟周期
 * 占空比=TIMx_CCR/(TIMx_ARR +1)
 */
static void BLDC_TIMx_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
 
	/* 使能定时器时钟 */
  BLDC_TIM_APBxClock_FUN(BLDC_TIM_CLK, ENABLE);//TIM1时钟使能
	TIM_DeInit(TIM1);
	
  /* 定时器基本参数时钟 */		 
  TIM_TimeBaseStructure.TIM_Period = BLDC_TIM_PERIOD; 
  /* 设置预分频：不预分频，即为168MHz */
  TIM_TimeBaseStructure.TIM_Prescaler = BLDC_TIM_PRESCALER;
  /* 设置时钟分频系数：不分频(这里用不到) */
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
  /* 向上计数模式 */
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  /* 重复计算器 */
  TIM_TimeBaseStructure.TIM_RepetitionCounter = BLDC_TIM_REPETITIONCOUNTER;//溢出BLDC_TIM_REPETITIONCOUNTER次进入中断
  TIM_TimeBaseInit(BLDC_TIMx, &TIM_TimeBaseStructure);

  /* 定时器输出通道1模式配置 */
  /* 模式配置：PWM模式1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  /* 输出状态设置：使能输出 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  /* 互补通道输出状态设置：使能输出 */
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  /* 设置跳变值，当计数器计数到这个值时，电平发生跳变 */
  TIM_OCInitStructure.TIM_Pulse = BLDC_TIM_PERIOD;
  /* 当定时器计数值小于CCR1_Val时为高电平 */
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性比较低
  TIM_OCInitStructure.TIM_OCNPolarity= TIM_OCNPolarity_High;//设置互补极性比较低
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//死区后输出状态
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;//死区后互补端输出状态
  /* 初始化定时器通道1输出PWM */
  TIM_OC1Init(BLDC_TIMx, &TIM_OCInitStructure);
  
  /* 定时器输出通道2模式配置 */
  /* 设置通道2的电平跳变值，输出另外一个占空比的PWM */
  TIM_OCInitStructure.TIM_Pulse = BLDC_TIM_PERIOD;
  /* 初始化定时器通道2输出PWM */
  TIM_OC2Init(BLDC_TIMx, &TIM_OCInitStructure);

  /* 定时器输出通道3模式配置 */
  /* 设置通道3的电平跳变值，输出另外一个占空比的PWM */
  TIM_OCInitStructure.TIM_Pulse = BLDC_TIM_PERIOD;
  /* 初始化定时器通道3输出PWM */
  TIM_OC3Init(BLDC_TIMx, &TIM_OCInitStructure);

  /* 自动输出启用，中断，死时间，锁配置*/
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;//在运行模式下“关闭状态”使能
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;//关闭模式下“关闭状态”使能
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;//锁定关闭
  TIM_BDTRInitStructure.TIM_DeadTime =0xDF;//死区时间
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;//刹车功能使能
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;//刹车输入高电平有效
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//自动输出使能
	TIM_BDTRInitStructure.TIM_OSSIState=TIM_OSSIState_Disable;//TIM_OSSIState_Disable;//TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_OSSRState=TIM_OSSRState_Disable;//TIM_OSSRState_Disable;//TIM_OSSRState_Enable;
  TIM_BDTRConfig(BLDC_TIMx, &TIM_BDTRInitStructure);
  
  TIM_OC1PreloadConfig(BLDC_TIMx,TIM_OCPreload_Enable);//通过软件写入控制波形的值下一次事件被更新时生效
  TIM_OC2PreloadConfig(BLDC_TIMx,TIM_OCPreload_Enable);
  TIM_OC3PreloadConfig(BLDC_TIMx,TIM_OCPreload_Enable);
  
  /* 使能定时器重载寄存器ARR */
  TIM_ARRPreloadConfig(BLDC_TIMx, ENABLE);
	//TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	
  /* 使能定时器 */
  TIM_Cmd(BLDC_TIMx, ENABLE);
  
  /* TIM主输出使能 */
  TIM_CtrlPWMOutputs(BLDC_TIMx, ENABLE);
  
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
}

/**
  * 函数功能: TIMx 输出PWM信号初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：只要调用这个函数TIMx的三个通道就会有PWM信号输出
  */
void BLDC_TIMx_PWM_Init(void)
{
	speed_duty=700;		//速度占空比30%
	BLDC_TIMx_GPIO_Config();
	BLDC_TIMx_Configuration();
}



/**
  * 函数功能: 无刷驱动换相
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
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

