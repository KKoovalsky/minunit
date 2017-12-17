/**
 *  This example uses HAL for STM32F407. In this file one can find the functions which initialize the benchmark timer
 * 	and its interrupt handler.
 */
#include "timer.h"
#include "err_handler.h"

TIM_HandleTypeDef htim2;

// The definition of ulHighFrequencyTimerTicks
volatile unsigned long ulHighFrequencyTimerTicks;

#define CLK_FREQ	168000000
#define PRESCALER 	42
#define PERIOD		40

// Interrupt frequency: 168 000 000 Hz / 42 / 40 = 100 kHz, then period in microseconds:
#define US_PER_PERIOD   ( 1000000 ) * ( PRESCALER ) * ( PERIOD ) / ( CLK_FREQ )

/* TIM2 init function */
void MX_TIM2_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = PRESCALER - 1;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = PERIOD - 1;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_TIM_Base_Start_IT(&htim2);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
	if (tim_baseHandle->Instance == TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
	}
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
	if (tim_baseHandle->Instance == TIM2)
	{
		__HAL_RCC_TIM2_CLK_DISABLE();
		HAL_NVIC_DisableIRQ(TIM2_IRQn);
	}
}

/**
 * \brief This interrupt is handled every 10 us. It is used to compute benchmarks.
 */
void TIM2_IRQHandler()
{
	ulHighFrequencyTimerTicks += US_PER_PERIOD;
	__HAL_TIM_CLEAR_FLAG(&htim2, TIM_IT_UPDATE);
}
