#ifndef TIMER_H_
#define TIMER_H_

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim2;

void MX_TIM2_Init(void);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle);
void TIM2_IRQHandler();

#endif /* TIMER_H_ */
