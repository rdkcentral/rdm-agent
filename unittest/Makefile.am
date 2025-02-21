AUTOMAKE_OPTIONS = subdir-objects

# Define the test executables
bin_PROGRAMS =   rdm_downloadmgr_gtest 

# Common include directories
COMMON_CPPFLAGS = -std=c++11 -I./rbus/include -I../ -I../../ -I/usr/include -I../include -I./mocks -I./iarm  -I../src/codebig  -DGTEST_ENABLE

AM_CPPFLAGS =  -I${top_srcdir}/unittest/rbus/include -I$(top_srcdir)/include -I$(top_srcdir)/mocks -I$(top_srcdir) -I/usr/include
# Common libraries
COMMON_LDADD = -lgtest -lgtest_main -lgmock -lgmock_main \
               -lcurl  -lpthread -L/usr/local/lib  -lssl -lcrypto -lcjson

# Common compiler flags
COMMON_CXXFLAGS = -frtti -fprofile-arcs -ftest-coverage -fpermissive

# Define source files for each test
#rdm_main_gtest_SOURCES =  rdm_main_gtest1.cpp rdm_rbus_mock.cpp  ../rdm_main.c ../src/rdm_rbus.c
#rdm_main_gtest1_SOURCES =  rdm_main_gtest1.cpp   ../rdm_main.c
#rdm_utils_gtest_SOURCES = rdm_utils_gtest.cpp  rdm_iarm_mock.cpp ../src/rdm_utils.c
#rdm_curl_gtest_SOURCES = rdm_curl_gtest.cpp rdm_curl_mock.cpp  ../src/rdm_curldownload.c
rdm_downloadmgr_gtest_SOURCES = rdm_downloadmgr_gtest.cpp ../src/rdm_downloadmgr.c
#rdm_downloadutils_gtest_SOURCES = rdm_downloadutils_gtest.cpp ../src/rdm_downloadutils.c
#rdm_usbinstall_gtest_SOURCES = rdm_usbinstall_gtest.cpp ../src/rdm_usbinstall.c
#rdm_downloadverapp_gtest_SOURCES = rdm_downloadverapp_gtest.cpp ../src/rdm_downloadverapp.c
#rdm_json_gtest_SOURCES = rdm_json_gtest.cpp ../src/rdm_jsonquery.c
#rdm_download_gtest_SOURCES = rdm_download_gtest.cpp  ../src/rdm_download.c


rdm_downloadmgr_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_downloadmgr_gtest_LDADD = $(COMMON_LDADD)
rdm_downloadmgr_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_downloadmgr_gtest_CFLAGS = $(COMMON_CXXFLAGS)
