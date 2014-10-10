#include <config.h>
#include <FreeRTOS.h>
#include <task.h>

static uint64_t milliseconds;

void
vApplicationIdleHook(void)
{
//    __WFI();
}


void
vApplicationTickHook(void)
{
	milliseconds += 1000 / configTICK_RATE_HZ;
}


void
vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
//    HALT();
}


/*
 * uint64_t
 * milliseconds_get(void) {
 *  uint64_t ret;
 *  taskENTER_CRITICAL();
 *  ret = milliseconds;
 *  taskEXIT_CRITICAL();
 *  return ret;
 * }
 */
