dnl Process this file with autoconf to produce a configure script.
# vim:set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT
#

AC_PREREQ(2.67)
AC_COPYRIGHT([(c) 2014 Chris Luke])

AC_INIT([FreeRTOS-test], [0.3])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([aclocal])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([subdir-objects foreign])

flavor=
AS_IF([test ! -z "${ac_cv_host}"],
    [flavor="${ac_cv_host}-"; CC="${flavor}gcc"]
)

# Build programs
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_PROG_AWK
: ${CFLAGS="-nostartfiles"}
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CC_C99
AM_PROG_CC_C_O
#LT_INIT
#AC_DISABLE_FAST_INSTALL
#AC_DISABLE_SHARED
#AC_ENABLE_STATIC
#AC_PROG_LIBTOOL
#AC_SUBST([LIBTOOL_DEPS])

if test x"$ac_cv_prog_cc_c99" = xno; then
        AC_MSG_ERROR([We need C99 and cannot figure out how to enable it])
fi

AC_CHECK_TOOL([OBJCOPY], [objcopy], [])
AC_CHECK_TOOL([OBJDUMP], [objdump], [])
AC_PROG_RANLIB

# Compiler flags
AX_CFLAGS_GCC_OPTION([-pipe])
AX_CFLAGS_GCC_OPTION([-mcpu=cortex-m3 -mthumb])
AX_CFLAGS_GCC_OPTION([-mfix-cortex-m3-ldrd])

# Library functions
AC_CHECK_FUNCS([memcpy strdup strndup])

# Compiler flags part deux
AX_CFLAGS_GCC_OPTION([-Wall])
AX_CFLAGS_GCC_OPTION([-W])
AX_CFLAGS_GCC_OPTION([-Wextra])

# These are warnings included in -Wextra, but we don't want all of -Wextra
AX_CFLAGS_GCC_OPTION([-Wno-unused-parameter])

# And finally, be picky
AX_CFLAGS_GCC_OPTION([-Werror])
AX_CFLAGS_GCC_OPTION([-pedantic])

# Doxygen
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(ON)

DX_INIT_DOXYGEN([$PACKAGE_NAME],[amlocal/doxygen.cfg],[doc])

# Debug mode
AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [configure with debugging features enabled]) ]
)
if test x"$enable_debug" = xyes; then
  CPPFLAGS="$CPPFLAGS -D_DEBUG"
  ggdb=
  if test x`uname` = xLinux; then
    ggdb=gdb
  fi
  CFLAGS="$CFLAGS -O0 -g$ggdb"
else
  CFLAGS="$CFLAGS -O2 -g"
fi

# Determine the chip
AC_ARG_WITH([chip],
  [AS_HELP_STRING([--with-chip=[chipname]], [configure with a specific chip [default=STM32F103ZE]]) ]
)
AC_MSG_NOTICE([with_chip=$with_chip])
default_chip="STM32F103ZE"
AS_IF([test "x$with_chip" = xno], [chip="$default_chip"],
      [test "x$with_chip" = xyes], [chip="$default_chip"],
      [test "x$with_chip" = x], [chip="$default_chip"],
      [chip="$with_chip"]
)
# Check we have a linker script for this chip
AC_MSG_NOTICE([Checking for chip $chip...])
AS_IF([test -f "src/platform/$chip.ld"],
    [AC_MSG_NOTICE([Linking for chip $chip])],
    [AC_MSG_ERROR([Cannot link for chip $chip])]
)
AC_SUBST([chip])

# Add our various discovered flags
CPPFLAGS="$CPPFLAGS $LIBCURL_CPPFLAGS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
LIBS="$LIBS $LIBCURL $PTHREAD_LIBS"

# Headers
AC_CHECK_HEADERS([sys/types.h stdlib.h math.h string.h \
    fcntl.h sys/stat.h stdint.h])

# Definitions (typedef, struct etc)
AC_C_INLINE
case "$ac_cv_c_inline" in
    yes) cc_inline=inline;;
    no) cc_inline=;;
    *) cc_inline="$ac_cv_c_inline";;
esac
AC_SUBST([cc_inline])

# Setup our include search paths
bdir="${srcdir}"
if test x"${bdir}" = x'.' ; then
    bdir=$(pwd)
fi
CPPFLAGS="$CPPFLAGS -I${bdir}/include"
CPPFLAGS="$CPPFLAGS -I${bdir}/lib"  # slightly misnamed, but meh

CPPFLAGS="$CPPFLAGS -I${bdir}/extlib/platform"
CPPFLAGS="$CPPFLAGS -I${bdir}/extlib/stdperiph/inc"
CPPFLAGS="$CPPFLAGS -I${bdir}/extlib/stm3210e_eval"
CPPFLAGS="$CPPFLAGS -I${bdir}/extlib/FreeRTOS/Source/include"
CPPFLAGS="$CPPFLAGS -I${bdir}/extlib/FreeRTOS/Source/portable/GCC/ARM_CM3"
CPPFLAGS="$CPPFLAGS -I${bdir}/extlib/microrl/src"

# If we have a local newlib, use it
if test -d newlib/arm-none-eabi/lib/armv7-m; then
    LDFLAGS="$LDFLAGS -L${bdir}/newlib/arm-none-eabi/lib/armv7-m"
fi
if test -d newlib/arm-none-eabi/include; then
    cc1=`arm-none-eabi-gcc ${CFLAGS} -print-prog-name=cc1`
    stdincs=`$cc1 ${CFLAGS} -v </dev/null 2>&1 |  grep '^ /' | grep -v '\.\.'`
    CFLAGS="$CFLAGS -nostdinc"
    rm -f gccdump.s
    for inc in $stdincs; do
        CPPFLAGS="$CPPFLAGS -isystem ${inc}"
    done
    CPPFLAGS="$CPPFLAGS -isystem ${bdir}/newlib/arm-none-eabi/include"
fi

# Display the summary
CC=`echo $CC | sed -e 's/  / /g'`
CPPFLAGS=`echo $CPPFLAGS | sed -e 's/  / /g'`
CFLAGS=`echo $CFLAGS | sed -e 's/  / /g'`
LD=`echo $LD | sed -e 's/  / /g'`
LDFLAGS=`echo $LDFLAGS | sed -e 's/  / /g'`
LIBS=`echo $LIBS | sed -e 's/  / /g'`

echo "CC:       $CC" >&2
echo "CPPFLAGS: $CPPFLAGS" >&2
echo "CFLAGS:   $CFLAGS" >&2
echo "LD:       $LD" >&2
echo "LDFLAGS:  $LDFLAGS" >&2
echo "LIBS:     $LIBS" >&2

# Output
AC_CONFIG_HEADER([include/ac_config.h])
AC_CONFIG_FILES([Makefile
    include/Makefile
    include/cc_config.h
    extlib/Makefile
    lib/Makefile
    src/Makefile
    tools/Makefile])
AC_OUTPUT

