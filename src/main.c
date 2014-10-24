/** Program start
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file src/main.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <FreeRTOS.h>
#include <task.h>
#include <posixio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <dma.h>
#include <serial.h>
#include <i2c.h>
#include <spi.h>

#include <microrl.h>

#include "main.h"

static void main_task(void *param);
static void platform_init(void);

QueueSetHandle_t qs_serial = NULL;

int main(void)
{
    dbg("Platform starting up.\r\n");

    // add main thread
    xTaskCreate(main_task, "main", 4096, NULL, 4, NULL);

    // start it up
    dbg("Launching RTOS scheduler.\r\n");
    vTaskStartScheduler();
}

struct cli {
    FILE        *in;
    FILE        *out;
    microrl_t   rl;
};

static void cli_print(void *opaque, const char *str)
{
    struct cli *cli = (struct cli *)opaque;

    if (cli->out != NULL) {
        fputs(str, cli->out);
        fflush(cli->out);
    }
}

static int cli_exec(void *opaque, int argc, const char *const *argv)
{
    struct cli *cli = (struct cli *)opaque;

    fprintf(cli->out, "Execute:");
    while (argc) {
        fprintf(cli->out, " %s", *argv);
        argc--; argv++;
    }
    fputs("\r\n", cli->out);

    return 0;
}

static void cli_sigint(void *opaque)
{
    struct cli *cli = (struct cli *)opaque;

    (void)cli;
}

void __attribute__ ((noreturn)) cli_task(void *param)
{
    struct cli *cli = (struct cli *)param;

    fprintf(stdout, "Starting CLI task\r\n");
    microrl_init(&cli->rl, cli, cli_print);
    microrl_set_execute_callback(&cli->rl, cli_exec);
    microrl_set_sigint_callback(&cli->rl, cli_sigint);

    for (;; ) {
        int ch = fgetc(cli->in);
        if (ch != EOF)
            microrl_insert_char(&cli->rl, (char)(ch & 0xff));
    }
}

void __attribute__ ((noreturn)) main_task(void *param)
{
    dbg("Scheduler launched.\r\n");

    // setup the peripherals
    platform_init();

    // announce life!
    dbg("This platform is running!\r\n");

    // add cli thread
    static struct cli serial_cli;
    memset(&serial_cli, '\0', sizeof(struct cli));
    serial_cli.in = stdin;
    serial_cli.out = stdout;
    xTaskCreate(cli_task, "cli_serial", 4096, &serial_cli, 4, NULL);

    //vTaskDelete(NULL);
    for (;; ) ;
}

#if USE_SERIAL_USART1
static int stdio_init(void)
{
    int fd;

    fd = open("/serial/1", O_RDWR);
    if (fd == -1)
        return 0;

    // stdin
    dup2(fd, 0);

    // stdout (buffered)
    dup2(fd, 1);
    setvbuf(stderr, NULL, _IOFBF, 0);

    // stderr (not buffered)
    dup2(fd, 2);
    setvbuf(stderr, NULL, _IONBF, 0);

    // this fd is no longer needed
    close(fd);

    return 0;
}
#endif

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
    stdio_init();
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
