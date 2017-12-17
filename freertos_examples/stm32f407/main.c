#include "stm32f4xx_hal.h"
#include "clock.h"
#include "gpio.h"
#include "usart.h"
#include "bench_tim.h"
#include "cases/eeprom_test.h"

static void testingTask(void *pvParameters);
const char testtask_name[] = "Testing";

int main(void)
{
	// Initialize all basic modules
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();

	// Initialize timer which counts execution time
	MX_TIM2_Init();

	// Initialize UART used for logging
	MX_USART3_UART_Init();

	xTaskCreate(testingTask, testtask_name, 1024, NULL, osPriorityNormal, NULL);

	vTaskStartScheduler();
}

static void testingTask(void *pvParameters)
{
	// Running all of the tests
	test_eeprom_read_write();

	for (;;)
		vTaskDelay(pdMS_TO_TICKS(1000));
}

