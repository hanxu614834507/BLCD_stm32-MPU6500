#include "config.h"
void TIM14_PWM_Init(u32 arr,u32 psc);//
MOTOR_DEVICE bldc_dev  ={1000,RUN,CW,0,0}; //���״̬����ṹ��
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
extern u8  TIM5CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ 
u8 time=0;
u8 i=0,t=0;
u8 cnt=0;
u8 res;
u8 c=0;
u8 m=0;
u8 canbuf[8];//can���ݻ�����
u8 canres[8];//can����ֵ
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
	speed_duty=700;//30%ռ�ձ�
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
	NVIC_EnableIRQ(HALL_U_V_W_EXTI_IRQn);//�ⲿ�ж�15_10
	while(1)
	{	
			//KeyControl();
//			delay_ms(10);
//			temperature=DS18B20_Get_Temp();	
//			printf("DS18B20���¶�Ϊ��%d",temperature);
	}
}




//��������
void KeyControl(void)
{
	if(KEY_RUNBRAKE == ON)  //run  //PE1����ߵ�ƽ��������ť����
	state = ON;
	if(state == ON)
	{
	  if(bldc_dev.motor_state==STOP) // �������
	  {
		LED_RUN_ON();								//run
		bldc_dev.motor_state=RUN;
		bldc_dev.step_counter=0;//�������������������ת��0
		bldc_dev.stalling_count=0;//ֹͣ��־
		//IncPIDInit();
		speed_duty=800; //%20 start  �ٶ�ռ�ձ�
		//NVIC_EnableIRQ(HALL_U_V_EXTI_IRQn);//�ⲿ�ж�9_5
		//NVIC_EnableIRQ(HALL_VEXTI_IRQn);//�ⲿ�ж�4
		NVIC_EnableIRQ(HALL_U_V_W_EXTI_IRQn);//�ⲿ�ж�15_10
		HALL_EXTI_Callback();
	  } 
	
	}else // stop
	{
		BLDC_PHASE_STOP();
		//while(KEY_RUNBRAKE);
	}
	
//	if(KEY_SPEED_HIGHT == ON)   //�ٶȵ�λ
//	{
//		speed_duty=speed_duty-100;
//		if(speed_duty<200)speed_duty=200;
////		if(ControlDriveCmd.Value_H > MOTOR_MAX_SPEED)		//������ٶȳ���max speed ������ֵ�ٶ�
////				ControlDriveCmd.Value_H = MOTOR_MAX_SPEED;
////				bldc_dev.motor_speed=ControlDriveCmd.Value_H;
////	}else
////	{
////		if(ControlDriveCmd.Value_L < MOTOR_MIN_SPEED)		//����С�ٶȵ���main speed ������ֵ�ٶ�
////			ControlDriveCmd.Value_L = MOTOR_MIN_SPEED;
////			bldc_dev.motor_speed=ControlDriveCmd.Value_L;
////	}
////	bldc_pid.SetPoint=bldc_dev.motor_speed;							//PID�趨�ٶ�
//	if(KEY_STATE == ON)		//����
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
//		if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)		//û�н��յ�����,ֱ���˳� 
//			BLDC_PHASE_STOP();
//			LED_ECU_ON();        //��ECUʧ��ָʾ��
//		  bldc_dev.motor_state=STOP;
//			bldc_dev.motor_speed=0;
//			bldc_dev.stalling_count=0;
//			time=0;
//	}
//	
//	//CH3 �ܵ���������
//	if(Current_filter(ADCPrimevalValue[1]) > 3000)//���������
//	{
//		Ctrl_Brake_ON();//PD14��ƽ��
//		BLDC_PHASE_STOP();//�ر���������
//		LED_Current_ON();//GPIOD11�ߵ�ƽ,��
//		while(1);
//	}	
//	}
}

void EXTI9_5_IRQHandler(void)		//�����ж�
{
	if(EXTI_GetITStatus(HALL_U_EXITLINE) != RESET)      //U����
	{	
		//TIM_Cmd(TIM3, ENABLE);				//������ʱ��3
			/* ����жϱ�־λ	*/
		EXTI_ClearITPendingBit(HALL_U_EXITLINE); 
		HALL_EXTI_Callback();
		//TIM_Cmd(TIM3, DISABLE);				//������ʱ��3
	}
	/* ȷ���Ƿ������EXTI LINE�ж�*/
	if(EXTI_GetITStatus(HALL_V_EXITLINE) != RESET)			 //V����
	{
		//TIM_Cmd(TIM3, ENABLE);				//������ʱ��3
		/* ����жϱ�־λ	*/
		EXTI_ClearITPendingBit(HALL_V_EXITLINE); 
		HALL_EXTI_Callback();
		//TIM_Cmd(TIM3, DISABLE);				//�رն�ʱ��3
	}
	/* ȷ���Ƿ������EXTI LINE�ж�*/
	if(EXTI_GetITStatus(HALL_W_EXITLINE) != RESET)			 //W����
	{
		//TIM_Cmd(TIM3, ENABLE);				//������ʱ��3
		/* ����жϱ�־λ	*/
		EXTI_ClearITPendingBit(HALL_W_EXITLINE); 
		HALL_EXTI_Callback();
		//TIM_Cmd(TIM3, DISABLE);				//�رն�ʱ��3
	}
}
/**************PID������ʼ��********************************/
void IncPIDInit(void) 
{
  bldc_pid.LastError=0;                    //Error[-1]
  bldc_pid.PrevError=0;                    //Error[-2]
  bldc_pid.Proportion=P_DATA;              //�������� Proportional Const
  bldc_pid.Integral=I_DATA;                //���ֳ���  Integral Const
  bldc_pid.Derivative=D_DATA;              //΢�ֳ��� Derivative Const
  bldc_pid.SetPoint=bldc_dev.motor_speed;  //�趨Ŀ��Desired Value
}
/********************����ʽPID�������************************************/
int IncPIDCalc(int NextPoint) 
{
  int iError,iIncpid;                                       //��ǰ���
  iError=bldc_pid.SetPoint - NextPoint;                     //��������
  iIncpid=(bldc_pid.Proportion * iError)                    //E[k]��
              -(bldc_pid.Integral * bldc_pid.LastError)     //E[k-1]��
              +(bldc_pid.Derivative * bldc_pid.PrevError);  //E[k-2]��
              
  bldc_pid.PrevError=bldc_pid.LastError;                    //�洢�������´μ���
  bldc_pid.LastError=iError;
  return(iIncpid);                                    //��������ֵ
}
/**
  * ��������:	ʹ��TIM2��ʱ���жϻص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: 1ms���иú���һ��
  */
void Tim2_VCP_Callback(void)
{
  static uint16_t time_count=0;
  
  bldc_dev.stalling_count++;  
  if(bldc_dev.motor_state==RUN)
  {
    time_count++;
    if(bldc_dev.stalling_count>2000) // �����ס��ʱ
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
    // bldc_dev.step_counter��¼������������50msʱ���ڲ���������������������תһȦ�����ܹ�
    // (MOTOR_POLE_PAIR_NUM*6)������,ʹ�� n=bldc_dev.step_counter/(MOTOR_POLE_PAIR_NUM*6) Ϊ
    // 50ms�ڵ��ת��Ȧ����Ϊ����Ϊ��ת/����(rpm)��
    //  n/50 = x/(60*1000) --> x=bldc_dev.step_counter*50
	if(PID_Status == 1)
	{
		temp=bldc_dev.step_counter*50;
		pid_result=IncPIDCalc(temp);     // ��������
		pid_result =pid_result*10/25;    // *10/25Ϊת�ٺ�ռ�ձ�һ��ת����ת�٣�0~2500����ռ�ձȣ�0~1000��
	    if((pid_result+speed_duty)<70)
		  speed_duty =70;
		else if((pid_result+speed_duty)>1000)
		  speed_duty =1000;
		else
		  speed_duty +=pid_result;   
	}
	else
	{
		speed_duty=bldc_dev.motor_speed*10/25;// ����
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
   /* ����Ӳ��IO��ʼ���ṹ�����*/
  GPIO_InitTypeDef GPIO_InitStructure;
  /*�����ⲿ�ж��߳�ʼ���ṹ�����*/
  EXTI_InitTypeDef EXTI_InitStructure;
  /* ����Ƕ�������жϿ�������ʼ���ṹ����� */
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ϊ����IO�����жϹ�����Ҫʹ��ʱ�� */
  RCC_AHB1PeriphClockCmd(HALL_RCC_CLOCKGPIO,DISABLE);
  
  /* �趨����IO��� */
  GPIO_InitStructure.GPIO_Pin = HALL_U_GPIO_PIN;  
  /* �趨����IOΪ����ģʽ */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
  /* ��ʼ��HALL_U��Ӧ����IO */
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
   /* ��ʼ��HALL_V��Ӧ����IO */
  GPIO_InitStructure.GPIO_Pin = HALL_V_GPIO_PIN; 
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
   /* ��ʼ��HALL_W��Ӧ����IO */
  GPIO_InitStructure.GPIO_Pin = HALL_W_GPIO_PIN; 
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);
  /* ѡ����Ϊ�ж�����Դ */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,HALL_U_GPIO_PINSOURCE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,HALL_V_GPIO_PINSOURCE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,HALL_W_GPIO_PINSOURCE);
  
  /* ��Ӧ���ж��� */
  EXTI_InitStructure.EXTI_Line=HALL_U_EXITLINE|HALL_V_EXITLINE|HALL_W_EXITLINE;
  /* �ⲿ�ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  /* �����½��ش�����ʽ */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  /* ʹ���ж� */
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  /* ����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ��� */
  EXTI_Init(&EXTI_InitStructure);
  
  /* ʹ���ⲿ�ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannel = HALL_U_V_W_EXTI_IRQn;
  /* ������ռ���ȼ�Ϊ1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  /* ���������ȼ�Ϊ1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  /* ʹ���ⲿ�ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;	
  /* ��ʼ������Ƕ�������жϿ����� */
  NVIC_Init(&NVIC_InitStructure); 
    /* ʹ���ⲿ�ж�ͨ�� */
//  NVIC_InitStructure.NVIC_IRQChannel = HALL_VEXTI_IRQn;
//  /* ������ռ���ȼ�Ϊ1 */
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
//  /* ���������ȼ�Ϊ1 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//  /* ʹ���ⲿ�ж�ͨ�� */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	
//  NVIC_InitStructure.NVIC_IRQChannel = HALL_W_EXTI_IRQn;
//  /* ������ռ���ȼ�Ϊ1 */
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
//  /* ���������ȼ�Ϊ1 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//  /* ʹ���ⲿ�ж�ͨ�� */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;	
  /* ��ʼ������Ƕ�������жϿ����� */
  NVIC_Init(&NVIC_InitStructure); 
  
}
	

/********************************************************/
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM14_PWM_Init(u32 arr,u32 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  	//TIM1ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��PORTAʱ��	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1); //GPIOA9����Ϊ��ʱ��1
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);              //��ʼ��PFA9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);//��ʼ����ʱ��14
	
	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ե�
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//�ڿ���ʱ����ͣ����û��䣬��һͨ��������
	TIM_OCInitStructure.TIM_Pulse = 2000;//�Զ���װ��ֵ@
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���
 
  TIM_ARRPreloadConfig(TIM1,ENABLE);//ARPEʹ�� 
	
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//ʹ��TIM1pwm�����TIM1/TIM8��Ч
 
										  
}  


