#include "config.h"

MOTOR_DEVICE bldc_dev  ={1000,STOP,CCW,0,0}; //电机状态命令结构体
static PID bldc_pid;
static unsigned char PID_Status = 0;

const unsigned char  Ascill_16[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

unsigned int Program_Cycle_Nums = 0;
short temperature; 
void PrintCommandsList(void);
void KeyControl(void);
void HALL_EXTI_Callback(void);
void HAL_SYSTICK_Callback(void);
void IncPIDInit(void);
int IncPIDCalc(int NextPoint);
unsigned int Current_filter(unsigned int advalue);
unsigned int SpeedVolt_filter(unsigned int advalue);
extern u8  TIM5CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值 
u8 time=0;
u8 i=0,t=0;
u8 cnt=0;
u8 res;
u8 canbuf[8];//can数据缓存区
u8 canres[8];//can返回值
long long temp=0;  
int main(void)
{
  BSP_Init();
	/************************************/  
	printf("*** BLDC Control board ***\r\n");
	printf("Date:20200203 V1.0 \r\n");
	while(1)
	{		
			KeyControl();
			delay_ms(10);
			temperature=DS18B20_Get_Temp();	
			printf("DS18B20的温度为：%d",temperature);
}
}



//按键处理
void KeyControl(void)
{
	
	if(KEY_RUNBRAKE == ON)  //run  //PD8输检测低电平，开启按钮按键
	{
	  if(bldc_dev.motor_state==STOP) // 电机启动
	  {
		bldc_dev.motor_state=RUN;
		bldc_dev.step_counter=0;//霍尔传感器步数，电机转速0
		bldc_dev.stalling_count=0;//停止标志
		IncPIDInit();
		speed_duty=200; //%20 start  速度占空比
		NVIC_EnableIRQ(HALL_UEXTI_IRQn);//外部中断3
		NVIC_EnableIRQ(HALL_VEXTI_IRQn);//外部中断4
		NVIC_EnableIRQ(HALL_WEXTI_IRQn);//外部中断9_5
		HALL_EXTI_Callback();
	  } 
	
	}else // stop
	{
		BLDC_PHASE_STOP();
		while(KEY_RUNBRAKE);
	}
	
	if(ControlDriveCmd.SpeedFlag == ON)   //速度挡位
	{
		if(ControlDriveCmd.Value_H > MOTOR_MAX_SPEED)		//若最大速度超过max speed 设置阈值速度
			ControlDriveCmd.Value_H = MOTOR_MAX_SPEED;
		bldc_dev.motor_speed=ControlDriveCmd.Value_H;
	}else
	{
		if(ControlDriveCmd.Value_L > MOTOR_MIN_SPEED)		//若最小速度低于main speed 设置阈值速度
			ControlDriveCmd.Value_L = MOTOR_MIN_SPEED;
		bldc_dev.motor_speed=ControlDriveCmd.Value_L;
		bldc_pid.SetPoint=bldc_dev.motor_speed;							//PID设定速度
	}
	
	if(ControlDriveCmd.GearPos == ON)		//方向
	{
		bldc_dev.motor_direction=CCW;
	}
	else
	{
		bldc_dev.motor_direction=CW;
	}
	time++;
	if(time==100)
	{   
		if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)		//没有接收到数据,直接退出 
			BLDC_PHASE_STOP();
			LED_YELLOW_ON();
		  bldc_dev.motor_state=STOP;
			bldc_dev.motor_speed=0;
			bldc_dev.stalling_count=0;
			time=0;
	}
	
	//CH3 总电机电流检测
	if(Current_filter(ADCPrimevalValue[1]) > 3000)//电机过电流
	{
		Ctrl_Brake_ON();//PB12电平低
		BLDC_PHASE_STOP();//关闭驱动换相
		LED_RED_ON();//GPIOB7高电平,亮
		while(1);
	}
		
}
/**
  * 函数功能: 外部中断服务函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void HALL_EXTI_Callback(void)
{
  __IO uint8_t uwStep = 0;
  static __IO int8_t uwStep0=0;
  
  uint16_t hall_read=(HALL_GPIO->IDR)&0x000E; // 获取霍尔传感器状态 pin1 2 3
  
  if(bldc_dev.motor_state==STOP)return;
  
  uwStep=hall_read>>1; //此处需要根据实际更改顺序
  if(bldc_dev.motor_direction==CW) // 方向判断  
	uwStep=7-uwStep;
  
  BLDC_PHASE_CHANGE(uwStep);   // 驱动换相
  if(uwStep0!=uwStep) // 确保电机已经换相
  {    
    bldc_dev.step_counter++;
    bldc_dev.stalling_count=0;
  }
  uwStep0=uwStep;
}


void EXTI3_IRQHandler(void)		//霍尔中断
{
	/* 确认是否产生了EXTI LINE中断*/
	if(EXTI_GetITStatus(HALL_U_EXITLINE) != RESET)
	{
	 HALL_EXTI_Callback();
	}
	/* 清除中断标志位	*/
	EXTI_ClearITPendingBit(HALL_U_EXITLINE); 
}
void EXTI4_IRQHandler(void)		//霍尔中断
{
	/* 确认是否产生了EXTI LINE中断*/
	if(EXTI_GetITStatus(HALL_V_EXITLINE) != RESET)
	{
	HALL_EXTI_Callback();
	}
	/* 清除中断标志位	*/
	EXTI_ClearITPendingBit(HALL_V_EXITLINE); 
}
void EXTI9_5_IRQHandler(void)		//霍尔中断
{
	/* 确认是否产生了EXTI LINE中断*/
	if(EXTI_GetITStatus(HALL_W_EXITLINE) != RESET)
	{
	HALL_EXTI_Callback();
	}
	/* 清除中断标志位	*/
	EXTI_ClearITPendingBit(HALL_V_EXITLINE|HALL_W_EXITLINE); 
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
