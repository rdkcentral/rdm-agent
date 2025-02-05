AUTOMAKE_OPTIONS = subdir-objects

# Define the test executables
bin_PROGRAMS = rdm_main_gtest rdm_curl_gtest rdm_utils_gtest rdm_json_gtest \
               rdm_download_gtest rdm_downloadmgr_gtest rdm_downloadutils_gtest \
               rdm_usbinstall_gtest rdm_rbus_gtest rdm_downloadverapp_gtest codebig_utils_gtest rdm_openssl_gtest

# Common include directories
COMMON_CPPFLAGS = -std=c++11 -I../include -I../mocks -I../src \
                  -I/usr/include/gtest -I/usr/local/include/gtest \
                  -DGTEST_ENABLE

# Common libraries
COMMON_LDADD = -lgtest -lgtest_main -lgmock -lgmock_main \
               -lcurl -ljson-c -lopenssl -lrbus -lpthread

# Common compiler flags
COMMON_CXXFLAGS = -frtti -fprofile-arcs -ftest-coverage -fpermissive

# Define source files for each test
rdm_main_gtest_SOURCES = unittest/rdm_main_gtest.cpp ../rdm_main.c 
rdm_curl_gtest_SOURCES = unittest/rdm_curl_gtest.cpp  ../src/rdm_curl.c
rdm_utils_gtest_SOURCES = unittest/rdm_utils_gtest.cpp ../src/rdm_utils.c
rdm_downloadmgr_gtest_SOURCES = unittest/rdm_downloadmgr_gtest.cpp ../src/rdm_downloadmgr.c
rdm_downloadutils_gtest_SOURCES = unittest/rdm_downloadutils_gtest.cpp ../src/rdm_downloadutils.c
rdm_usbinstall_gtest_SOURCES = unittest/rdm_usbinstall_gtest.cpp ../src/rdm_usbinstall.c
rdm_downloadverapp_gtest_SOURCES = unittest/rdm_downloadverapp_gtest.cpp ../src/rdm_downloadverapp.c
rdm_json_gtest_SOURCES = unittest/rdm_json_gtest.cpp ../src/rdm_jsonquery.c
rdm_download_gtest_SOURCES = unittest/rdm_download_gtest.cpp ../src/rdm_download.c mocks/mock_curl.cpp
rdm_rbus_gtest_SOURCES = unittest/rdm_rbus_gtest.cpp ../src/rdm_rbus.c mocks/mock_rbus.cpp
rdm_openssl_gtest_SOURCES = unittest/rdm_openssl_gtest.cpp ../src/rdm_openssl.c mocks/mock_openssl.cpp
codebig_utils_gtest_SOURCES = unittest/codebig_utils_gtest.cpp ../src/codebig/codeBigUtils.c 

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

rdm_downloadmgr_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_downloadmgr_gtest_LDADD = $(COMMON_LDADD)
rdm_downloadmgr_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_downloadmgr_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_downloadutils_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_downloadutils_gtest_LDADD = $(COMMON_LDADD)
rdm_downloadutils_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_downloadutils_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_usbinstall_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_usbinstall_gtest_LDADD = $(COMMON_LDADD)
rdm_usbinstall_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_usbinstall_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_downloadverapp_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_downloadverapp_gtest_LDADD = $(COMMON_LDADD)
rdm_downloadverapp_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_downloadverapp_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_json_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_json_gtest_LDADD = $(COMMON_LDADD)
rdm_json_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_json_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_download_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_download_gtest_LDADD = $(COMMON_LDADD)
rdm_download_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_download_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_rbus_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_rbus_gtest_LDADD = $(COMMON_LDADD)
rdm_rbus_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_rbus_gtest_CFLAGS = $(COMMON_CXXFLAGS)

rdm_openssl_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
rdm_openssl_gtest_LDADD = $(COMMON_LDADD)
rdm_openssl_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
rdm_openssl_gtest_CFLAGS = $(COMMON_CXXFLAGS)

codebig_utils_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
codebig_utils_gtest_LDADD = $(COMMON_LDADD)
codebig_utils_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
codebig_utils_gtest_CFLAGS = $(COMMON_CXXFLAGS)

