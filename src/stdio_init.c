/** Wire up stdio to the POSIX layer.
 * \file src/stdio_init.c
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "stdio_init.h"

/** Flag indicating whether the stdio FILE handles have been attached
 * to some sort of I/O device yet. */
uint8_t stdio_started = 0;


/**
 * Connects the stdio FILE handles stdin, stdout and stderr to
 * the serial port USART1. This makes use of the POSIX-like I/O
 * layer.
 *
 * @returns 1 on success, 0 otherwise.
 */
int stdio_start(void)
{
    int fd = -1;

#if USE_SERIAL_USART1
    fd = open("/serial/1", O_RDWR);
#endif
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

    stdio_started = 1;

    return 1;
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
