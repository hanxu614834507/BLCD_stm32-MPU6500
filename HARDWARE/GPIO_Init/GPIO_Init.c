#include "GPIO_Init.h"

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

     RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOB|
														RCC_AHB1Periph_GPIOD|
                            RCC_AHB1Periph_GPIOE, ENABLE);	//使能时钟

		
    //LED														 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;									
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//时钟速度100M
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//普通输出模式
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//端口输出模式为推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
		GPIO_ResetBits(GPIOB, GPIO_Pin_7 );
		GPIO_ResetBits(GPIOB, GPIO_Pin_8 );
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	//ctrl brake	制动												 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;									
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//?????50M
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//???????????	
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
    GPIO_Init(GPIOD, &GPIO_InitStructure);	
		GPIO_ResetBits(GPIOD, GPIO_Pin_7 );

	
	//外部输入信号
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;  //外部按键 (PD8检测启动按键)	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  

    GPIO_Init(GPIOD, &GPIO_InitStructure);

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;   				//PWM IN
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
//	
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//	 
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void HALL_GPIO_Init(void)
{
   /* 定义硬件IO初始化结构体变量*/
  GPIO_InitTypeDef GPIO_InitStructure;
  /*定义外部中断线初始化结构体变量*/
  EXTI_InitTypeDef EXTI_InitStructure;
  /* 定义嵌套向量中断控制器初始化结构体变量 */
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 为启用IO引脚中断功能需要使能时钟 */
  RCC_AHB1PeriphClockCmd(HALL_RCC_CLOCKGPIO,ENABLE);
  
  /* 设定引脚IO编号 */
  GPIO_InitStructure.GPIO_Pin = HALL_U_GPIO_PIN;  
  /* 设定引脚IO为输入模式 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
  /* 初始化KEY2对应引脚IO */
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = HALL_V_GPIO_PIN; 
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = HALL_W_GPIO_PIN; 
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* 选择作为中断输入源 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,HALL_U_GPIO_PINSOURCE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,HALL_V_GPIO_PINSOURCE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,HALL_W_GPIO_PINSOURCE);
  
  /* 对应的中断线 */
  EXTI_InitStructure.EXTI_Line=HALL_U_EXITLINE|HALL_V_EXITLINE|HALL_W_EXITLINE;
  /* 外部中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  /* 下降沿触发方式 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  /* 使能中断 */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  /* 根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器 */
  EXTI_Init(&EXTI_InitStructure);
  
  /* 使能外部中断通道 */
  NVIC_InitStructure.NVIC_IRQChannel = HALL_UEXTI_IRQn;
  /* 设置抢占优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  /* 设置子优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  /* 使能外部中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
  /* 初始化配置嵌套向量中断控制器 */
  NVIC_Init(&NVIC_InitStructure); 
    /* 使能外部中断通道 */
  NVIC_InitStructure.NVIC_IRQChannel = HALL_VEXTI_IRQn;
  /* 设置抢占优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  /* 设置子优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  /* 使能外部中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
  /* 初始化配置嵌套向量中断控制器 */
  NVIC_Init(&NVIC_InitStructure); 
  
}







