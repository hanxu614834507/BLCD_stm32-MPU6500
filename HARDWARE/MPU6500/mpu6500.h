#ifndef _MPU_6500_H_
#define _MPU_6500_H_

#include "sys.h"

#define SPI_NSS_HIGH 	GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define SPI_NSS_LOW		GPIO_ResetBits(GPIOB, GPIO_Pin_12)

typedef struct
{
	s16  xa; 	//x????
	s16  ya;		
	s16  za;
	s16  xg;	//x????
	s16  yg;	//x????
	s16  zg;	//x????
	
}s_mpu6500;


extern s_mpu6500 mpu6500_info;


void mpu6500_init(void);
s16 SpiReadWrite16Bits(s16 TxData);


#endif
