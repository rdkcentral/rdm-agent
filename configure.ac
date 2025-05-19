# Copyright 2023 Comcast Cable Communications Management, LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0
#

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT(RDM, 1.0, bhanuprakash.chintaginjala@sky.uk)
AC_CONFIG_SRCDIR([rdm_main.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE(RDM, RDM)

IS_LIBRDKCONFIG_ENABLED=" "
# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.
AM_PROG_LIBTOOL

# Set shared libraries
AC_DISABLE_STATIC
AC_ENABLE_SHARED

AC_ARG_ENABLE([cpc],
    [AS_HELP_STRING([--enable-cpc], [Enable CPC support])],
    [USE_CPC="$enableval"],
    [USE_CPC=no])

AM_CONDITIONAL([USE_CPC], [test "$USE_CPC" = "yes"])
AC_SUBST(USE_CPC)

if test "$USE_CPC" = "yes"; then
    AC_DEFINE([USE_CPC], [1], [Define to enable CPC support])
fi

# Checks for header files.
AC_ARG_ENABLE([mountutils],
        AS_HELP_STRING([--enable-mountutils],[enables mountutils replacement (default is no)]),
        [
          case "${enableval}" in
           yes) IS_LIBRDKCONFIG_ENABLED=true
                LIBRDKCONFIG_FLAG=" -DLIBRDKCONFIG_BUILD ";;
           no)  IS_LIBRDKCONFIG_ENABLED=false ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-mountutils]) ;;
           esac
           ],
        [echo "mountutils is disabled"])
AM_CONDITIONAL([IS_LIBRDKCONFIG_ENABLED], [test x$IS_LIBRDKCONFIG_ENABLED = xtrue])
AC_SUBST(LIBRDKCONFIG_FLAG)


# IARMBus Support
AC_ARG_ENABLE([iarmbusSupport],
[  --enable-iarmbusSupport    Turn on iarmbus support],
[case "${enableval}" in
  yes) iarmbusSupport=true ;;
  no)  iarmbusSupport=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-iarmbusSupport]) ;;
esac],[iarmbusSupport=false])
AM_CONDITIONAL([IARMBUS_ENABLE], [test x$iarmbusSupport = xtrue])

AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
                                #include <stdio.h>
                                #undef PACKAGE
                                #undef VERSION
                                #undef HAVE_STDLIB_H
                                #include "rdk_debug.h"
                ]])
        ],[
                AC_MSG_RESULT([RDK logging enabled: yes])
                rdm_LDFLAGS="$rdm_LDFLAGS -lrdkloggers"
                rdm_CFLAGS="$rdm_CFLAGS -DRDK_LOGGER_ENABLED"
        ],[
                AC_MSG_RESULT([RDK logging enabled: no])
        ])

AC_SUBST([rdm_LDFLAGS])
AC_SUBST([rdm_CFLAGS])

# Checks for typedefs, structures, and compiler characteristics.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
