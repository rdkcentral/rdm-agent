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
IS_LIBRDKCERTSEL_ENABLED=""

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

AC_ARG_ENABLE([openssl],
    [AS_HELP_STRING([--enable-openssl], [Enable OPENSSL support])],
    [IS_OPENSSL="$enableval"],
    [IS_OPENSSL=no])

AM_CONDITIONAL([IS_OPENSSL], [test "$IS_OPENSSL" = "yes"])
AC_SUBST(IS_OPENSSL)

if test "$IS_OPENSSL" = "yes"; then
    AC_DEFINE([IS_OPENSSL], [1], [Define to enable OPENSSL support])
fi

AC_ARG_ENABLE([rdkb],
    [AS_HELP_STRING([--enable-rdkb], [Enable RDKB support])],
    [IS_RDKB="$enableval"],
    [IS_RDKB=no])

AM_CONDITIONAL([IS_RDKB], [test "$IS_RDKB" = "yes"])
AC_SUBST(IS_RDKB)

if test "$IS_RDKB" = "yes"; then
    AC_DEFINE([IS_RDKB], [1], [Define to enable RDKB support])
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

AC_ARG_ENABLE([t2api],
	      AS_HELP_STRING([--enable-t2api],[enables telemetry]),
	      [
	        case "${enableval}" in
                 yes) IS_TELEMETRY2_ENABLED=true
                      T2_EVENT_FLAG=" -DT2_EVENT_ENABLED ";;
                 no)  IS_TELEMETRY2_ENABLED=false ;;
               *) AC_MSG_ERROR([bad value ${enableval} for --enable-t2enable]) ;;
	       esac
	       ],
	   [echo "telemetry is disabled"])
AM_CONDITIONAL([IS_TELEMETRY2_ENABLED], [test x$IS_TELEMETRY2_ENABLED = xtrue])
AC_SUBST(T2_EVENT_FLAG)

AC_ARG_ENABLE([rdkcertselector],
        AS_HELP_STRING([--enable-rdkcertselector],[enables rdkcertselector replacement (default is no)]),
        [
          case "${enableval}" in
           yes) IS_LIBRDKCERTSEL_ENABLED=true
                LIBRDKCERTSEL_FLAG=" -DLIBRDKCERTSELECTOR ";;
           no)  IS_LIBRDKCERTSEL_ENABLED=false ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-rdkcertselector]) ;;
           esac
           ],
        [echo "rdkcertselector is disabled"])
AM_CONDITIONAL([IS_LIBRDKCERTSEL_ENABLED], [test x$IS_LIBRDKCERTSEL_ENABLED = xtrue])
AC_SUBST(LIBRDKCERTSEL_FLAG)

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

# L2Test Support
AC_ARG_ENABLE([unittest],
        AS_HELP_STRING([--enable-unittest],[enable unittest build (default is no)]),
        [
          case "${enableval}" in
           yes) L2_TEST_ENABLED=true
                L2_TEST_FLAG="-DL2_TEST_ENABLED" ;;
           no) L2_TEST_ENABLED=false AC_MSG_ERROR([unittest support is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-unittest ]) ;;
           esac
           ],
         [echo "unittest is disabled"])

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
