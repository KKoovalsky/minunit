## MinUnit

Minunit is a minimal unit testing framework for C/C++ self-contained in a
single header file.

It provides a way to define and configure test suites and a few handy assertion
types.  It reports the summary of the number of tests run, number of assertions
and time elapsed.

This framework supports also FreeRTOS. Unfortunately you won't be ready to go
after downloading the framework - you have to set up an environment to properly
run the tests. Steps to achieve this goal are described further.

Note that this project is based on:
http://www.jera.com/techinfo/jtns/jtn002.html
and
https://github.com/siu/minunit

## Set up the environment for FreeRTOS

This port uses FreeRTOS API function called `vTaskGetRunTimeStats` to get 
runtime statistics. It must be enabled inside `FreeRTOSConfig.h` file by adding
those lines:

	#define configGENERATE_RUN_TIME_STATS			1
	#define configUSE_STATS_FORMATTING_FUNCTIONS	1
	extern volatile unsigned long ulHighFrequencyTimerTicks;
	#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() ( ulHighFrequencyTimerTicks = 0UL )
	#define portGET_RUN_TIME_COUNTER_VALUE()	ulHighFrequencyTimerTicks

Then you have to initialize a hardware timer (look up `freertos_examples`
directory to check how I've done this with STM32F407) to generate interrupt
with a frequency which is 10-100 bigger than frequency of the OS tick
interrupt. In the interrupt handler `ulHighFrequencyTimerTicks` must be
incremented:

	void TIM_UPDATE_ISR_HANDLER()
	{
		...
		ulHighFrequencyTimerTicks += TIM_PERIOD;
		...
	}

The variable `ulHighFrequencyTimerTicks` must be incremented by the period of
time between interrupts. You should also change the value of
`TIME_BASE_UNIT_STR` inside `minunit.h` file if your unit of time counted by
the timer is different than microseconds (it is used with logging messages).
The function `vTaskGetRunTimeStats` then reads the variable
`ulHighFrequencyTimerTicks` to obtain the time spent on execution of code.
As you can see `TIM_PERIOD` determines the resolution of time measurement.

Minunit uses `printf` function for logging. You could not have it in your
project, but you can easily get it in the Internet. You could then adjust
`printf` function to call e.g. a function which sends a byte over UART.

You have to define `ulHighFrequencyTimerTicks` somewhere:

	volatile unsigned long ulHighFrequencyTimerTicks;

Finally you can create the task in which you run tests:

	// This must be also defined (minunit uses the array testtask_name to obtain
	// the name of the testing task.
	const char testtask_name[] = "Testing";

	static void testingTask(void *pvParameters);

	int main()
	{
		...

		initialize_timer();
		initialize_uart();

		...

		xTaskCreate(testingTask, testtask_name, 1024, NULL, osPriorityNormal, NULL);

		vTaskStartScheduler();
	}

	static void testingTask(void *pvParameters)
	{
		test_some_features();
		...
		for (;;)
			vTaskDelay(pdMS_TO_TICKS(1000));
	}

Don't forget to compile with `-D__FREERTOS__` flag.

Check the directory `freertos_examples` for more details.
## How to use it

This is a minimal test suite written with minunit:

	#include "minunit.h"

	MU_TEST(test_check) {
		mu_check(5 == 7);
	}
	MU_TEST_SUITE(test_suite) {
		MU_RUN_TEST(test_check);
	}

	int main(int argc, char *argv[]) {
		MU_RUN_SUITE(test_suite);
		MU_REPORT();
		return 0;
	}

Which will produce the following output:

	F
	test_check failed:
		readme_sample.c:4: 5 == 7


	1 tests, 1 assertions, 1 failures

	Finished in 0.00032524 seconds (real) 0.00017998 seconds (proc)

Check out minunit_example.c to see a complete example. Compile with something
like:

	gcc minunit_example.c -lrt -lm -o minunit_example

Don't forget to add -lrt for the timer and -lm for linking the function fabs
used in mu_assert_double_eq.

## Setup and teardown functions

One can define setup and teardown functions and configure the test suite to run
them by using the macro MU_SUITE_CONFIGURE with within a MU_TEST_SUITE
declaration.

## Assertion types

mu_check(condition): will pass if the condition is evaluated to true, otherwise
it will show the condition as the error message

mu_fail(message): will fail and show the message

mu_assert(condition, message): will pass if the condition is true, otherwise it
will show the failed condition and the message

mu_assert_int_eq(expected, result): it will pass if the two numbers are
equal or show their values as the error message

mu_assert_double_eq(expected, result): it will pass if the two values
are almost equal or show their values as the error message. The value of
MINUNIT_EPSILON sets the threshold to determine if the values are close enough.

mu_assert_string_eq(expected, result): will pass if `expected` string is same
like `result` string. This macro uses the function `strcmp` which leaves on
encountering a null character. On failure a message is shown.

mu_assert_bytearray_eq(expected, result, complen): will pass if `expected`
array is same like `result` array. The comparison is made on `complen` number
of bytes. This macro uses `memcmp` function which allows to compare arrays
containing null characters. On failure a message is shown.

## Authors

David Siñuela Pastor <siu.4coders@gmail.com>
