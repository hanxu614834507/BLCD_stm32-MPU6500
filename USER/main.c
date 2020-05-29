#include "config.h"
void TIM14_PWM_Init(u32 arr,u32 psc);//
MOTOR_DEVICE bldc_dev  ={1000,RUN,CW,0,0}; //电机状态命令结构体
static PID bldc_pid;
static unsigned char PID_Status = 0;
const unsigned char  Ascill_16[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
void HALL_GPIO_Initdisable(void);
unsigned int Program_Cycle_Nums = 0;
short temperature; 
void PrintCommandsList(void);
void KeyControl(void);
void HAL_SYSTICK_Callback(void);
void IncPIDInit(void);
int IncPIDCalc(int NextPoint);
u8 state=0;
unsigned int Current_filter(unsigned int advalue);
unsigned int SpeedVolt_filter(unsigned int advalue);
extern u8  TIM5CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值 
u8 time=0;
u8 i=0,t=0;
u8 cnt=0;
u8 res;
u8 c=0;
u8 m=0;
u8 canbuf[8];//can数据缓存区
u8 canres[8];//can返回值
//#define TEST
long long temp=0;  
void dectoBIN(int num);
int main(void)
{
  BSP_Init();
	
	/************************************/  
	printf("*** BLDC Control board ***\r\n");
	printf("Date:20200320 V1.0 \r\n");
	//bldc_dev.motor_state=STOP;
	#ifdef TEST
	speed_duty=700;//30%占空比
	TIM_SetCompare1(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
	TIM_SetCompare2(BLDC_TIMx,0);
	TIM_SetCompare3(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/1000);
	TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Enable);
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
	TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Enable);
  //GPIO_WConfig();
	#endif
	HALL_EXTI_Callback();
	NVIC_EnableIRQ(HALL_U_V_W_EXTI_IRQn);//外部中断15_10
	while(1)
	{	
			//KeyControl();
//			delay_ms(10);
//			temperature=DS18B20_Get_Temp();	
//			printf("DS18B20的温度为：%d",temperature);
	}
}




//按键处理
void KeyControl(void)
{
	if(KEY_RUNBRAKE == ON)  //run  //PE1输检测高电平，开启按钮按键
	state = ON;
	if(state == ON)
	{
	  if(bldc_dev.motor_state==STOP) // 电机启动
	  {
		LED_RUN_ON();								//run
		bldc_dev.motor_state=RUN;
		bldc_dev.step_counter=0;//霍尔传感器步数，电机转速0
		bldc_dev.stalling_count=0;//停止标志
		//IncPIDInit();
		speed_duty=800; //%20 start  速度占空比
		//NVIC_EnableIRQ(HALL_U_V_EXTI_IRQn);//外部中断9_5
		//NVIC_EnableIRQ(HALL_VEXTI_IRQn);//外部中断4
		NVIC_EnableIRQ(HALL_U_V_W_EXTI_IRQn);//外部中断15_10
		HALL_EXTI_Callback();
	  } 
	
	}else // stop
	{
		BLDC_PHASE_STOP();
		//while(KEY_RUNBRAKE);
	}
	
//	if(KEY_SPEED_HIGHT == ON)   //速度挡位
//	{
//		speed_duty=speed_duty-100;
//		if(speed_duty<200)speed_duty=200;
////		if(ControlDriveCmd.Value_H > MOTOR_MAX_SPEED)		//若最大速度超过max speed 设置阈值速度
////				ControlDriveCmd.Value_H = MOTOR_MAX_SPEED;
////				bldc_dev.motor_speed=ControlDriveCmd.Value_H;
////	}else
////	{
////		if(ControlDriveCmd.Value_L < MOTOR_MIN_SPEED)		//若最小速度低于main speed 设置阈值速度
////			ControlDriveCmd.Value_L = MOTOR_MIN_SPEED;
////			bldc_dev.motor_speed=ControlDriveCmd.Value_L;
////	}
////	bldc_pid.SetPoint=bldc_dev.motor_speed;							//PID设定速度
//	if(KEY_STATE == ON)		//方向
//	{
//		if(ControlDriveCmd.GearPos == CCW)
//		bldc_dev.motor_direction=CCW;
//	}
//	else
//	{
//		bldc_dev.motor_direction=CW;
//	}
//	time++;
//	if(time==100)
//	{   
//		if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)		//没有接收到数据,直接退出 
//			BLDC_PHASE_STOP();
//			LED_ECU_ON();        //与ECU失联指示灯
//		  bldc_dev.motor_state=STOP;
//			bldc_dev.motor_speed=0;
//			bldc_dev.stalling_count=0;
//			time=0;
//	}
//	
//	//CH3 总电机电流检测
//	if(Current_filter(ADCPrimevalValue[1]) > 3000)//电机过电流
//	{
//		Ctrl_Brake_ON();//PD14电平低
//		BLDC_PHASE_STOP();//关闭驱动换相
//		LED_Current_ON();//GPIOD11高电平,亮
//		while(1);
//	}	
//	}
}

void EXTI9_5_IRQHandler(void)		//霍尔中断
{
	if(EXTI_GetITStatus(HALL_U_EXITLINE) != RESET)      //U触发
	{	
		//TIM_Cmd(TIM3, ENABLE);				//开启定时器3
			/* 清除中断标志位	*/
		EXTI_ClearITPendingBit(HALL_U_EXITLINE); 
		HALL_EXTI_Callback();
		//TIM_Cmd(TIM3, DISABLE);				//开启定时器3
	}
	/* 确认是否产生了EXTI LINE中断*/
	if(EXTI_GetITStatus(HALL_V_EXITLINE) != RESET)			 //V触发
	{
		//TIM_Cmd(TIM3, ENABLE);				//开启定时器3
		/* 清除中断标志位	*/
		EXTI_ClearITPendingBit(HALL_V_EXITLINE); 
		HALL_EXTI_Callback();
		//TIM_Cmd(TIM3, DISABLE);				//关闭定时器3
	}
	/* 确认是否产生了EXTI LINE中断*/
	if(EXTI_GetITStatus(HALL_W_EXITLINE) != RESET)			 //W触发
	{
		//TIM_Cmd(TIM3, ENABLE);				//开启定时器3
		/* 清除中断标志位	*/
		EXTI_ClearITPendingBit(HALL_W_EXITLINE); 
		HALL_EXTI_Callback();
		//TIM_Cmd(TIM3, DISABLE);				//关闭定时器3
	}
}
/**************PID参数初始化********************************/
void IncPIDInit(void) 
{
  bldc_pid.LastError=0;                    //Error[-1]
  bldc_pid.PrevError=0;                    //Error[-2]
  bldc_pid.Proportion=P_DATA;              //比例常数 Proportional Const
  bldc_pid.Integral=I_DATA;                //积分常数  Integral Const
  bldc_pid.Derivative=D_DATA;              //微分常数 Derivative Const
  bldc_pid.SetPoint=bldc_dev.motor_speed;  //设定目标Desired Value
}
/********************增量式PID控制设计************************************/
int IncPIDCalc(int NextPoint) 
{
  int iError,iIncpid;                                       //当前误差
  iError=bldc_pid.SetPoint - NextPoint;                     //增量计算
  iIncpid=(bldc_pid.Proportion * iError)                    //E[k]项
              -(bldc_pid.Integral * bldc_pid.LastError)     //E[k-1]项
              +(bldc_pid.Derivative * bldc_pid.PrevError);  //E[k-2]项
              
  bldc_pid.PrevError=bldc_pid.LastError;                    //存储误差，用于下次计算
  bldc_pid.LastError=iError;
  return(iIncpid);                                    //返回增量值
}
/**
  * 函数功能:	使用TIM2定时器中断回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 1ms运行该函数一次
  */
void Tim2_VCP_Callback(void)
{
  static uint16_t time_count=0;
  
  bldc_dev.stalling_count++;  
  if(bldc_dev.motor_state==RUN)
  {
    time_count++;
    if(bldc_dev.stalling_count>2000) // 电机卡住超时
      bldc_dev.motor_state=STOP;    
  }
  else
  {
    time_count=0;
  }
  if(time_count>50) // 50ms
  {
    int temp;
    int pid_result;
    // bldc_dev.step_counter记录霍尔传感器在50ms时间内产生的脉冲个数，而电机旋转一圈会有总共
    // (MOTOR_POLE_PAIR_NUM*6)个脉冲,使用 n=bldc_dev.step_counter/(MOTOR_POLE_PAIR_NUM*6) 为
    // 50ms内电机转动圈数，为换算为【转/分钟(rpm)】
    //  n/50 = x/(60*1000) --> x=bldc_dev.step_counter*50
	if(PID_Status == 1)
	{
		temp=bldc_dev.step_counter*50;
		pid_result=IncPIDCalc(temp);     // 计算增量
		pid_result =pid_result*10/25;    // *10/25为转速和占空比一个转换，转速（0~2500），占空比（0~1000）
	    if((pid_result+speed_duty)<70)
		  speed_duty =70;
		else if((pid_result+speed_duty)>1000)
		  speed_duty =1000;
		else
		  speed_duty +=pid_result;   
	}
	else
	{
		speed_duty=bldc_dev.motor_speed*10/25;// 计算
		if(speed_duty < 70)
		  speed_duty =70;
		else if(speed_duty > 1000)
		  speed_duty =1000;
	}
	
    time_count=0;
    bldc_dev.step_counter=0;
  }
}

unsigned int Current_filter(unsigned int advalue)
{
	static unsigned int valuelist[5];
	static unsigned char index=0;
	unsigned char i;
	unsigned long value=0;
	
	valuelist[index++] = advalue;
	if(index==5) index =0;
	
	value =0;
	for(i=0;i<5;i++)
	{
		value +=valuelist[i];
	}
	return value/5;
}
void dectoBIN(int num)
{
	if(num>0){
		dectoBIN(num/2);
		printf("%d",num%2);
	}
}

	
void HALL_GPIO_Initdisable(void)
{
   /* 定义硬件IO初始化结构体变量*/
  GPIO_InitTypeDef GPIO_InitStructure;
  /*定义外部中断线初始化结构体变量*/
  EXTI_InitTypeDef EXTI_InitStructure;
  /* 定义嵌套向量中断控制器初始化结构体变量 */
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 为启用IO引脚中断功能需要使能时钟 */
  RCC_AHB1PeriphClockCmd(HALL_RCC_CLOCKGPIO,DISABLE);
  
  /* 设定引脚IO编号 */
  GPIO_InitStructure.GPIO_Pin = HALL_U_GPIO_PIN;  
  /* 设定引脚IO为输入模式 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
  /* 初始化HALL_U对应引脚IO */
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
   /* 初始化HALL_V对应引脚IO */
  GPIO_InitStructure.GPIO_Pin = HALL_V_GPIO_PIN; 
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
   /* 初始化HALL_W对应引脚IO */
  GPIO_InitStructure.GPIO_Pin = HALL_W_GPIO_PIN; 
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);
  /* 选择作为中断输入源 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,HALL_U_GPIO_PINSOURCE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,HALL_V_GPIO_PINSOURCE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,HALL_W_GPIO_PINSOURCE);
  
  /* 对应的中断线 */
  EXTI_InitStructure.EXTI_Line=HALL_U_EXITLINE|HALL_V_EXITLINE|HALL_W_EXITLINE;
  /* 外部中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  /* 上升下降沿触发方式 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  /* 使能中断 */
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  /* 根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器 */
  EXTI_Init(&EXTI_InitStructure);
  
  /* 使能外部中断通道 */
  NVIC_InitStructure.NVIC_IRQChannel = HALL_U_V_W_EXTI_IRQn;
  /* 设置抢占优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  /* 设置子优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  /* 使能外部中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;	
  /* 初始化配置嵌套向量中断控制器 */
  NVIC_Init(&NVIC_InitStructure); 
    /* 使能外部中断通道 */
//  NVIC_InitStructure.NVIC_IRQChannel = HALL_VEXTI_IRQn;
//  /* 设置抢占优先级为1 */
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
//  /* 设置子优先级为1 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//  /* 使能外部中断通道 */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	
//  NVIC_InitStructure.NVIC_IRQChannel = HALL_W_EXTI_IRQn;
//  /* 设置抢占优先级为1 */
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
//  /* 设置子优先级为1 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//  /* 使能外部中断通道 */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;	
  /* 初始化配置嵌套向量中断控制器 */
  NVIC_Init(&NVIC_InitStructure); 
  
}
	

/********************************************************/
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM14_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  	//TIM1时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTA时钟	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1); //GPIOA9复用为定时器1
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);              //初始化PFA9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);//初始化定时器14
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//在空闲时输出低，如果没这句，第一通道有问题
	TIM_OCInitStructure.TIM_Pulse = 2000;//自动重装载值@
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
 
  TIM_ARRPreloadConfig(TIM1,ENABLE);//ARPE使能 
	
	TIM_Cmd(TIM1, ENABLE);  //使能TIM1
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//使能TIM1pwm输出，TIM1/TIM8有效
 
										  
}  


