/** Small wrapper to make sure 'errno' is a real variable, not a macro.
 *
 * If 'errno' is a macro, undefine it and add an extern reference to the
 * real one. This is so routines that need to set it to a value can
 * guarantee that it will succeed. With some libc's errno ends up being
 * a macro.
 *
 * \file include/real_errno.h
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _REAL_ERRNO_H
#define _REAL_ERRNO_H

#include <errno.h>

#ifdef errno
#undef errno
/** The errno variable */
extern int errno;
#endif

#endif /* _REAL_ERRNO_H */
