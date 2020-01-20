/*******************************************************************************
*******************************************************************************/
#include "config.h"
/*-----------------------------------------------------------------------------
** ����ԭ�ͣ�void CAN_Config(void)
** �������ܣ�Configures the CAN.
** ��ڲ�������
** ���ڲ�������
** ��    ע����
-----------------------------------------------------------------------------*/
ControlCmdType ControlDriveCmd;//��������ṹ��
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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF	;//����
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    // Configure CAN1 pin: TX
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1); //GPIOD0����ΪCAN1
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1); //GPIOD1����ΪCAN1
	
	
//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//�����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ����������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=DISABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //ģʽ���� 
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//����ͬ����Ծ����(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS1_10tq; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=4;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 
    
		//���ù�����
 	  CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=(unsigned int)(DRIVE_ID>>13) & 0xFFFF;;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=(unsigned int)(((DRIVE_ID & 0xFFFF)<<3) | CAN_ID_EXT | CAN_RTR_DATA)&0xFFFF;;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFFFF;//�������κ�ID
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xFFFF;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
		
	return 0;
    
}

//can����һ������(�̶���ʽ:IDΪ0X00,��׼֡,����֡)	
//���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN1_Send_Msg(u8 ID,u8* msg)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=0x00;	 // ��׼��ʶ��Ϊ0
  TxMessage.ExtId=ID;	 // ������չ��ʾ����29λ��
  TxMessage.IDE=0;		  // ʹ����չ��ʶ��
  TxMessage.RTR=0;		  // ��Ϣ����Ϊ����֡��һ֡8λ
  TxMessage.DLC=8;							 // ������֡��Ϣ
  for(i=0;i<8;i++)
  TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          
  mbox= CAN_Transmit(CAN1, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
  if(i>=0XFFF)return 1;
  return 0;		

}

//CAN1�����жϺ���
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
	
	CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);  /* �˺��������ͷ���������˵�,�ڷǱ�Ҫʱ,����Ҫ�Լ��ͷ� */
	if(RxMessage.ExtId==DRIVE_ID) 
	{ 
		ControlDriveCmd.GearPos = RxMessage.Data[0];
		ControlDriveCmd.Value_L = RxMessage.Data[1];
		ControlDriveCmd.Value_H = RxMessage.Data[2];
		ControlDriveCmd.SpeedFlag = RxMessage.Data[3];
//		LED1_ON;
	}
  
	can1_rec_flag = 1;
	CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);  /* ��������ж� */
}


