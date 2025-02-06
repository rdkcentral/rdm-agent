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

AC_INIT([rdm_main_gtest], [1.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Check for necessary headers
AC_CHECK_HEADERS([json-c/json.h gtest/gtest.h gmock/gmock.h curl/curl.h openssl/ssl.h rbus/rbus.h])

# Check for necessary libraries
AC_CHECK_LIB([json-c], [main])
AC_CHECK_LIB([gtest], [main])
AC_CHECK_LIB([gmock], [main])
AC_CHECK_LIB([curl], [main])
AC_CHECK_LIB([openssl], [main])
AC_CHECK_LIB([rbus], [main])
AC_CHECK_LIB([pthread], [main])

# Checks for programs
AC_PROG_CXX
AC_PROG_CC

# Check for standard C++ library
AC_CHECK_LIB([stdc++], [main])

# Checks for compiler characteristics
AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_REALLOC

# Generate Makefile
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
