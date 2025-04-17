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
echo "*********** Return value of rdm_download_gtest $rdmdown"

./rdm_downloadutils_gtest
rdmdutils=$?
echo "*********** Return value of rdm_downloadutils_gtest $rdmdutils"

./rdm_rbus_gtest
rdmrbus=$?
echo "*********** Return value of rdm_downloadutils_gtest $rdmrbus"

# List of unit test executables

# Run tests and capture return values
if [ "$rdmmain" = "0" ] && [ "$utils" = "0" ] && [ "$rdmcurl" = "0" ] && [ "$rdmjson" = "0" ] && [ "$rdmdown" = "0" ] && [ "$rdmdutils" = "0" ] && [ "$rdmrbus" = "0" ]; then
    cd ../

    lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage.filtered.info

    genhtml coverage.filtered.info --output-directory out
else
    echo "L1 UNIT TEST FAILED. PLEASE CHECK AND FIX"
    exit 1
fi

