#include "GPIO_Init.h"

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_WConfig(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOF时钟

  //GPIOD11,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;//LED0和LED1对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO
	GPIO_ResetBits(GPIOE,GPIO_Pin_12 | GPIO_Pin_13);
}
void GPIO_Config(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOF时钟

  //GPIOD11,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13  | GPIO_Pin_14  ;//LED0和LED1对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIO
	
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13;//LED0和LED1对应IO口
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//LED0和LED1对应IO口
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
// GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO
//	ctrl brake	制动												 
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;									
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//?????50M
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//???????????	
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
//    GPIO_Init(GPIOD, &GPIO_InitStructure);	
//		GPIO_ResetBits(GPIOD, GPIO_Pin_7 );


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
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  /* 初始化HALL_U对应引脚IO */
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
   /* 初始化HALL_V对应引脚IO */
  GPIO_InitStructure.GPIO_Pin = HALL_V_GPIO_PIN; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
   /* 初始化HALL_W对应引脚IO */
  GPIO_InitStructure.GPIO_Pin = HALL_W_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
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
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  /* 根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器 */
  EXTI_Init(&EXTI_InitStructure);
  
  /* 使能外部中断通道 */
  NVIC_InitStructure.NVIC_IRQChannel = HALL_U_V_W_EXTI_IRQn;
  /* 设置抢占优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  /* 设置子优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  /* 使能外部中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
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
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
  /* 初始化配置嵌套向量中断控制器 */
  NVIC_Init(&NVIC_InitStructure); 
  
}



