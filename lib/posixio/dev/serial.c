/** IO Platform driver for serialio
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file lib/posixio//dev/serial.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#define POSIXIO_PRIVATE

#include <config.h>
#include <posixio/posixio.h>
#include <posixio/dev/serial.h>
#include <stm32/serial.h>

#include <errno.h>
#include <string.h>

static int ser_close(void *fh)
{
    if (fh != NULL)
        return 0;
    errno = ENOENT;
    return -1;
}

static void *ser_open(const char *name, int flags, ...)
{
#if USE_SERIAL_USART1
    if (!strcmp(name, "1"))
        return &Serial1;
#endif
#if USE_SERIAL_USART2
    if (!strcmp(name, "2"))
        return &Serial2;
#endif
#if USE_SERIAL_USART3
    if (!strcmp(name, "3"))
        return &Serial3;
#endif
#if USE_SERIAL_USART4
    if (!strcmp(name, "4"))
        return &Serial4;
#endif
#if USE_SERIAL_USART5
    if (!strcmp(name, "5"))
        return &Serial5;
#endif

    errno = ENOENT;
    return NULL;
}

static ssize_t ser_read(void *fh, void *ptr, size_t len)
{
    char *p = ptr;
    int16_t r;
    ssize_t written = 0;

    while (len && (r = serial_get((serial_t *)fh, 1)) != ETIMEDOUT) {
        *p = (char)r;
        p++;
        len--;
        written++;
    }
    if (!written) {
        errno = EAGAIN;
        return 0;
    }
    return written;
}

static ssize_t ser_write(void *fh, const void *ptr, size_t len)
{
    serial_write((serial_t *)fh, ptr, len);
    return len;
}

static struct iodev iodev_serial = {
    .name   = "serial",

    .close  = ser_close,
    .open   = ser_open,
    .read   = ser_read,
    .write  = ser_write,

    .flags  = POSIXDEV_ISATTY
};

int posixio_register_serial(void)
{
    return posixio_register_dev(&iodev_serial);
}
