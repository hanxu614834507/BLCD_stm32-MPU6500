/*******************************************************************************
*******************************************************************************/
#include "config.h"
/*-----------------------------------------------------------------------------
** 函数原型：void CAN_Config(void)
** 函数功能：Configures the CAN.
** 入口参数：无
** 出口参数：无
** 备    注：无
-----------------------------------------------------------------------------*/
ControlCmdType ControlDriveCmd;//接收命令结构体
unsigned char can1_rec_flag = 1;
u8 CAN_Config(void)
{
    GPIO_InitTypeDef       GPIO_InitStructure;
    CAN_InitTypeDef        CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    // Configure CAN1 pin: RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF	;//复用
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    // Configure CAN1 pin: TX
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1); //GPIOD0复用为CAN1
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1); //GPIOD1复用为CAN1
	
	
//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=DISABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //模式设置 
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS1_10tq; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=4;  //分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 
    
		//配置过滤器
 	  CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=(unsigned int)(DRIVE_ID>>13) & 0xFFFF;;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=(unsigned int)(((DRIVE_ID & 0xFFFF)<<3) | CAN_ID_EXT | CAN_RTR_DATA)&0xFFFF;;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFFFF;//不屏蔽任何ID
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xFFFF;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
		
	return 0;
    
}

//can发送一组数据(固定格式:ID为0X00,标准帧,数据帧)	
//数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CAN1_Send_Msg(u8 ID,u8* msg)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=0x00;	 // 标准标识符为0
  TxMessage.ExtId=ID;	 // 设置扩展标示符（29位）
  TxMessage.IDE=0;		  // 使用扩展标识符
  TxMessage.RTR=0;		  // 消息类型为数据帧，一帧8位
  TxMessage.DLC=8;							 // 发送两帧信息
  for(i=0;i<8;i++)
  TxMessage.Data[i]=msg[i];				 // 第一帧信息          
  mbox= CAN_Transmit(CAN1, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
  if(i>=0XFFF)return 1;
  return 0;		

}

//CAN1接收中断函数
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage; 
//	uint8_t* RxData = (uint8_t*)&ControlDriveCmd;
	
	RxMessage.StdId=0x00;
	RxMessage.ExtId=0x00;
	RxMessage.IDE=0;
	RxMessage.DLC=0;
	RxMessage.FMI=0;
	RxMessage.Data[0]=0x00;     
	
	CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);  /* 此函数包含释放提出报文了的,在非必要时,不需要自己释放 */
	if(RxMessage.ExtId==DRIVE_ID) 
	{ 
		ControlDriveCmd.GearPos = RxMessage.Data[0];
		ControlDriveCmd.Value_L = RxMessage.Data[1];
		ControlDriveCmd.Value_H = RxMessage.Data[2];
		ControlDriveCmd.SpeedFlag = RxMessage.Data[3];
//		LED1_ON;
	}
  
	can1_rec_flag = 1;
	CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);  /* 清除挂起中断 */
}



