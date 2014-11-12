/** FSMC definitions.
 * \file src/platform/fsmc.h
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _FSMC_H
#define _FSMC_H

#include <stdint.h>

/* Section .fsmc_bank1 */
extern uint8_t _fsmc_bank1_start;
extern uint8_t _fsmc_bank1_end;

/* Section .fsmc_bank2 */
extern uint8_t _fsmc_bank2_start;
extern uint8_t _fsmc_bank2_end;

/* Section .fsmc_bank3 */
extern uint8_t _fsmc_bank3_start;
extern uint8_t _fsmc_bank3_end;

/* Section .fsmc_bank4 */
extern uint8_t _fsmc_bank4_start;
extern uint8_t _fsmc_bank5_end;


/* Section .fsmc_bank1_1 */
extern uint8_t _fsmc_bank1_1_start;
extern uint8_t _fsmc_bank1_1_end;

/* Section .fsmc_bank1_2 */
extern uint8_t _fsmc_bank1_2_start;
extern uint8_t _fsmc_bank1_2_end;

/* Section .fsmc_bank1_3 */
extern uint8_t _fsmc_bank1_3_start;
extern uint8_t _fsmc_bank1_3_end;

/* Section .fsmc_bank1_4 */
extern uint8_t _fsmc_bank1_4_start;
extern uint8_t _fsmc_bank1_4_end;

#endif /* _FSMC_H */

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
