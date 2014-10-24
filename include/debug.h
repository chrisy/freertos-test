/**
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file include/debug.h
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef  _DEBUG_H
#define  _DEBUG_H

void dbg_init(void);
void dbg(const char *msg);
void dbgf(const char *fmt, ...);

#endif  // _DEBUG_H