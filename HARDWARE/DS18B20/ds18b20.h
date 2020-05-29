#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//DS18B20��������	   
//////////////////////////////////////////////////////////////////////////////////

//IO��������
#define DS18B20_IO_IN()  {GPIOE->MODER&=~(3<<(15));GPIOG->MODER|=0<<15;}	//PG9����ģʽ
#define DS18B20_IO_OUT() {GPIOE->MODER&=~(3<<(15));GPIOG->MODER|=1<<15;} 	//PG9���ģʽ
 
////IO��������											   
#define	DS18B20_DQ_OUT PEout(15) //���ݶ˿�	PG9
#define	DS18B20_DQ_IN  PEin(15)  //���ݶ˿�	PG9 
   	
u8 DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20    
#endif















