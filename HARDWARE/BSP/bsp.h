#ifndef _BSP_H_	
#define _BSP_H_

#ifdef __cplusplus		   //定义对CPP进行C处理 //开始部分
extern "C" {
#endif
#include "stm32f4xx.h"
void BSP_Init(void);			//硬件资源初始化

#ifdef __cplusplus		   			//定义对CPP进行C处理 //结束部分
}
#endif

#endif
