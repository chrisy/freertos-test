/**
 * \file src/stdio_init.h
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef  _STDIO_INIT_H
#define  _STDIO_INIT_H

#include <stdint.h>

extern uint8_t stdio_started;

int stdio_start(void);

#endif  // _STDIO_INIT_H

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
