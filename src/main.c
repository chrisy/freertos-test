/** Program start
 * \file src/main.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <cli/cli.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <posixio/posixio.h>

#include <stm32/dma.h>
#include <stm32/serial.h>
#include <stm32/i2c.h>
#include <stm32/spi.h>

#include "main.h"
#include "stdio_init.h"

static void main_task(void *param);
static void platform_init(void);
static void led_init(void);

QueueSetHandle_t qs_serial = NULL;

int main(void)
{
    dbg("Platform starting up.\r\n");

    // add main thread
    xTaskCreate(main_task, "main",
                STACK_SIZE_MAIN, NULL,
                THREAD_PRIO_MAIN, NULL);

    // start it up
    dbg("Launching RTOS scheduler.\r\n");
    vTaskStartScheduler();
}


void __attribute__ ((noreturn)) main_task(void *param)
{
    dbg("Scheduler launched.\r\n");

    // setup the peripherals
    platform_init();

    // announce life!
    printf("This platform is running!\r\n");
    fflush(stdout);

    // Start the CLI
    cli_init();
    cli_start("serial", NULL, NULL);

    for (;; )
        DELAY_MS(500);
}

static void platform_init(void)
{
    // Bootstrap the POSIX IO platform
    posixio_start();

    // Initialize USARTs
#if configUSE_QUEUE_SETS
    qs_serial = xQueueCreateSet(16);
    ASSERT(qs_serial != NULL);
#endif
#if USE_SERIAL_USART1
    serial_start(&Serial1, 9600
#if configUSE_QUEUE_SETS
                 , qs_serial
#endif
                 );
    serial_puts(&Serial1, "STM32 Platform starting up.\r\n");
    stdio_start();
    printf("STDIO started on USART 1.\r\n");
    fflush(stdout);
#endif

#if USE_SERIAL_USART2
    printf("Starting USART 2.\r\n");
    serial_start(&Serial2, 9600
#if configUSE_QUEUE_SETS
                 , qs_serial
#endif
                 );
#endif

#if USE_SERIAL_USART3
    printf("Starting USART 3.\r\n");
    serial_start(&Serial3, 9600
#if configUSE_QUEUE_SETS
                 , qs_serial
#endif
                 );
#endif

#if USE_SERIAL_UART4
    printf("Starting USART 4.\r\n");
    serial_start(&Serial4, 9600
#if configUSE_QUEUE_SETS
                 , qs_serial
#endif
                 );
#endif

    // Initialize i2c
#if USE_I2C1
    printf("Starting I2C 1.\r\n");
    i2c_start(&I2C1_Dev);
#endif
#if USE_I2C2
    printf("Starting I2C 2.\r\n");
    i2c_start(&I2C2_Dev);
#endif

    printf("Starting LED task.\r\n");
    led_init();

    // Initialize SPI
#if USE_SPI1
    printf("Starting SPI 1.\r\n");
    spi_start(&SPI1_Dev, 0); // todo - workout the SPI mode bits
#endif
#if USE_SPI2
    printf("Starting SPI 2.\r\n");
    spi_start(&SPI2_Dev, 0);
#endif
#if USE_SPI3
    printf("Starting SPI 3.\r\n");
    spi_start(&SPI3_Dev, 0);
#endif
}

#if configGENERATE_RUN_TIME_STATS
void platform_timer_init(void)
{
    TIM_TypeDef *tim = TIM14;

    taskENTER_CRITICAL();
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    taskEXIT_CRITICAL();

    tim->PSC = 60000; // prescaler - gives us 600Hz
    tim->CR1 = TIM_CR1_CEN;
    tim->EGR |= TIM_EGR_UG;
}
#endif

static void led_advance(TimerHandle_t timer)
{
    static uint8_t i = 0;

    uint16_t b = (1 << i) << 6;

    GPIOF->BSRR = b;
    GPIOF->BRR = (0xf << 6) & ~b;

    i++;
    if (i > 3) i = 0;
}

static TimerHandle_t led_timer;

void led_init(void)
{
    taskENTER_CRITICAL();

    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;

    GPIOF->CRL &= ~(
        GPIO_CRL_MODE6 | GPIO_CRL_CNF6 |
        GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOF->CRH &= ~(
        GPIO_CRH_MODE8 | GPIO_CRH_CNF8 |
        GPIO_CRH_MODE9 | GPIO_CRH_CNF9);

    GPIOF->CRL |= GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1;
    GPIOF->CRL |= GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1;
    GPIOF->CRH |= GPIO_CRH_MODE8_0 | GPIO_CRH_MODE8_1;
    GPIOF->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1;

    taskEXIT_CRITICAL();

    GPIOF->BSRR = 0xffff;

    led_timer = xTimerCreate("led", 100 / portTICK_PERIOD_MS,
                             pdTRUE, NULL, led_advance);
    xTimerStart(led_timer, 0);
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
