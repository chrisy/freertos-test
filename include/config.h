/**
 * \file include/config.h
 * Config
 */

#ifndef CONFIG_H
#define CONFIG_H

#ifdef _DEBUG
# define DEBUG 1
#else
# define DEBUG 0
#endif

#ifdef HAVE_CONFIG_H
#include "ac_config.h"
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif

#include "cc_config.h"

#endif /* CC_CONFIG_H */
