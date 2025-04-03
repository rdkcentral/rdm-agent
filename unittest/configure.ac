# Initialize Autoconf
AC_INIT([rdm_main_gtest], [1.0])
AC_CONFIG_MACRO_DIRS([m4])
LT_INIT

# Initialize Automake
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Check for necessary headers
AC_CHECK_HEADERS([gtest/gtest.h gmock/gmock.h])

# Checks for programs
AC_PROG_CXX

# Checks for libraries
AC_CHECK_LIB([stdc++], [main])

# Checks for header files
AC_INCLUDES_DEFAULT

# Checks for typedefs, structures, and compiler characteristics
AC_C_CONST

# Checks for library functions
AC_FUNC_MALLOC
AC_FUNC_REALLOC

# Generate the Makefile
AC_CONFIG_FILES([Makefile])

# Generate the configure script
AC_OUTPUT
