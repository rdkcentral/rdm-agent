#!/bin/bash
cd unittest/

automake --add-missing
autoreconf --install

./configure

make clean
make

./rdm_main_gtest
rdmmain=$?
echo "*********** Return value of rdm_main_gtest $rdmmain"

./rdm_utils_gtest
utils=$?
echo "*********** Return value of rdm_utils_gtest $utils"

./rdm_curl_gtest
rdmcurl=$?
echo "*********** Return value of curl_gtest $rdmcurl"

./rdm_json_gtest
rdmjson=$?
echo "*********** Return value of json_gtest $rdmjson"

./rdm_download_gtest
rdmdown=$?
echo "*********** Return value of rdm_utils_gtest $rdmdown"

./rdm_downloadutils_gtest
rdmdownutils=$?
echo "*********** Return value of rdm_down_gtest $rdmdownutils"

./rdm_downloadmgr_gtest
rdmdownloadmgr=$?
echo "*********** Return value of rdm_downloadmgr_gtest $rdmdownloadmgr"


./rdm_usbinstall_gtest
rdm_usbinstall_gtest=$?
echo "*********** Return value of usbinstall $rdm_usbinstall_gtest"

./rdm_openssl_gtest
rdm_openssl_gtest=$?
echo "*********** Return value of openssl $rdm_openssl_gtest"

./codebig_utils_gtest
codebig_utils_gtest=$?
echo "*********** Return value of openssl $codebig_utils_gtest"

./rdm_rbus_gtest
rdm_rbus_gtest=$?
echo "*********** Return value of openssl $rdm_rbus_gtest"

./rdm_downloadverapp_gtest
rdm_downloadverapp_gtest=$?
echo "*********** Return value of openssl $rdm_downloadverapp_gtest"
# List of unit test executables

# Run tests and capture return values
if [ "$rdmmain" = "0" ] && [ "$utils" = "0" ] && [ "$rdmcurl" = "0" ] && [ "$rdmjson" = "0" ] && [ "$rdmdown" = "0" ] && [ "$rdmdownloadmgr" = "0" ] && [ "$rdmjson" = "0" ] && [ "$rdm_usbinstall_gtest" = "0" ] && [ "$rdm_openssl_gtest" = "0" ] && [ "$codebig_utils_gtest" = "0" ] && [ "$rdm_rbus_gtest" = "0" ] && [ "$rdm_downloadverapp_gtest" = "0" ]; then
    cd ../

    lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage.filtered.info

    genhtml coverage.filtered.info --output-directory out
else
    echo "L1 UNIT TEST FAILED. PLEASE CHECK AND FIX"
    exit 1
fi

