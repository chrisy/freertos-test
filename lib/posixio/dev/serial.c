/** IO Platform driver for serialio
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
#include <sys/stat.h>

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
#if USE_SERIAL_UART4
    if (!strcmp(name, "4"))
        return &Serial4;
#endif
#if USE_SERIAL_UART5
    if (!strcmp(name, "5"))
        return &Serial5;
#endif

    errno = ENOENT;
    return NULL;
}

static ssize_t ser_read(void *fh, void *ptr, size_t len)
{
    unsigned char *p = ptr;
    int16_t r;
    ssize_t written = 0;

    while (len) {
        r = serial_get((serial_t *)fh, 1);
        if (r == -ETIMEDOUT) {
            if (written)
                break;
            // TODO: if nonblock, break here

            continue;
        }

        *p = (unsigned char)r;
        p++;
        len--;
        written++;
    }

    if (!written) {
        errno = EAGAIN;
        return -1;
    }

    return written;
}

static ssize_t ser_write(void *fh, const void *ptr, size_t len)
{
    serial_write((serial_t *)fh, ptr, len);
    return len;
}

int ser_fstat(void *fh, struct stat *st)
{
    if (st == NULL) {
        errno = EFAULT;
        return -1;
    }

    memset(st, '\0', sizeof(*st));

#if USE_SERIAL_USART1
    if (fh == &Serial1)
        st->st_dev = DEV_USART1;

#endif
#if USE_SERIAL_USART2
    if (fh == &Serial2)
        st->st_dev = DEV_USART2;

#endif
#if USE_SERIAL_USART3
    if (fh == &Serial3)
        st->st_dev = DEV_USART3;

#endif
#if USE_SERIAL_UART4
    if (fh == &Serial4)
        st->st_dev = DEV_UART4;

#endif
#if USE_SERIAL_UART5
    if (fh == &Serial5)
        st->st_dev = DEV_UART5;

#endif
    if (st->st_dev == 0) {
        errno = EBADF;
        return -1;
    }

    st->st_mode = S_IFCHR;

    return 0;
}

int ser_stat(const char *file, struct stat *st)
{
    if (file == NULL || st == NULL) {
        errno = EFAULT;
        return -1;
    }

    memset(st, '\0', sizeof(*st));

#if USE_SERIAL_USART1
    if (!strcmp(file, "1"))
        st->st_dev = DEV_USART1;

#endif
#if USE_SERIAL_USART2
    if (!strcmp(file, "2"))
        st->st_dev = DEV_USART2;

#endif
#if USE_SERIAL_USART3
    if (!strcmp(file, "3"))
        st->st_dev = DEV_USART3;

#endif
#if USE_SERIAL_UART4
    if (!strcmp(file, "4"))
        st->st_dev = DEV_UART4;

#endif
#if USE_SERIAL_UART5
    if (!strcmp(file, "5"))
        st->st_dev = DEV_UART5;

#endif
    if (st->st_dev == 0) {
        errno = ENOENT;
        return -1;
    }

    st->st_mode = S_IFCHR;

    return 0;
}

static struct iodev iodev_serial = {
    .name   = "serial",

    .close  = ser_close,
    .open   = ser_open,
    .read   = ser_read,
    .write  = ser_write,
    .fstat  = ser_fstat,
    .stat   = ser_stat,

    .flags  = POSIXDEV_ISATTY
};

int posixio_register_serial(void)
{
    return posixio_register_dev(&iodev_serial);
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
