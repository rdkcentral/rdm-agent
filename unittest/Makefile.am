AUTOMAKE_OPTIONS = subdir-objects 
ACLOCAL_AMFLAGS = -I m4


# Define the test executables
bin_PROGRAMS = rdm_main_gtest rdm_curl_gtest rdm_utils_gtest rdm_json_gtest \
               rdm_download_gtest rdm_openssl_gtest rdm_usbinstall_gtest

# Common include directories
COMMON_CPPFLAGS = -std=c++11 -I/usr/include/cjson -I./include  -I../ -I../../ -I/usr/include -I../include -I../mocks -I../src -I../include    \
                  -I/usr/include/gtest -I/usr/local/include/gtest  -I../src/codebig\
                  -DGTEST_ENABLE

AM_CPPFLAGS =  -I$(top_srcdir)/unittest/mocks -I$(top_srcdir)/include -I$(top_srcdir)/mocks -I$(top_srcdir) -I/usr/include 

# Common libraries
COMMON_LDADD = -lgtest -lgtest_main -lgmock -lgmock_main \
               -lpthread -lcurl -lcjson -lssl -lcrypto 

# Common compiler flags
COMMON_CXXFLAGS = -frtti -fprofile-arcs -ftest-coverage -fpermissive

# Define source files for each test
rdm_main_gtest_SOURCES = rdm_main_gtest.cpp ../rdm_main.c 
rdm_curl_gtest_SOURCES = rdm_curl_gtest.cpp  ../src/rdm_curldownload.c
rdm_utils_gtest_SOURCES = rdm_utils_gtest.cpp mocks/mock_iarm_bus.cpp  ../src/rdm_utils.c
rdm_json_gtest_SOURCES = rdm_json_gtest.cpp ../src/rdm_jsonquery.c
rdm_download_gtest_SOURCES = rdm_download_gtest.cpp ../src/rdm_download.c
rdm_openssl_gtest_SOURCES = rdm_openssl_gtest.cpp ../src/rdm_openssl.c
rdm_usbinstall_gtest_SOURCES = rdm_usbinstall_gtest.cpp ../src/rdm_usbinstall.c



rdm_main_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_main_gtest_LDADD = $(COMMON_LDADD)
rdm_main_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_main_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_curl_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_curl_gtest_LDADD = $(COMMON_LDADD)
rdm_curl_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_curl_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_utils_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_utils_gtest_LDADD = $(COMMON_LDADD)
rdm_utils_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_utils_gtest_CFLAGS = $(COMMON_CXXFLAGS)


rdm_json_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_json_gtest_LDADD = $(COMMON_LDADD)
rdm_json_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_json_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_download_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_download_gtest_LDADD = $(COMMON_LDADD)
rdm_download_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_download_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_openssl_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_openssl_gtest_LDADD = $(COMMON_LDADD)
rdm_openssl_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_openssl_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_usbinstall_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_usbinstall_gtest_LDADD = $(COMMON_LDADD)
rdm_usbinstall_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_usbinstall_gtest_CFLAGS = $(COMMON_CXXFLAGS)



