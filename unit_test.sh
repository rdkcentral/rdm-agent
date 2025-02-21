#!/bin/bash
cd unittest/

automake --add-missing
autoreconf --install

./configure

make clean
make

#./rdm_main_gtest1
#rdmmain=$?
#echo "*********** Return value of system_utils_gtest $rdmmain"

#./rdm_utils_gtest
#utils=$?
#echo "*********** Return value of rdm_utils_gtest $utils"

#./rdm_curl_gtest
#rdmcurl=$?
#echo "*********** Return value of common_device_api_gtest $rdmcurl"


./rdm_download_gtest
rdmdown=$?
echo "*********** Return value of rdm_download_gtest $rdmdown"

./rdm_utils_gtest
rdmutils=$?
echo "*********** Return value of rdm_utils_gtest $rdmutils"

#./rdm_json_gtest
#jsonparse=$?
#echo "*********** Return value of json_parse_gtest $jsonparse"

if [ "$rdmdown" = "0" ] && ["$rdmutils" = "0"] ; then
    cd ../

    lcov --capture --directory . --output-file coverage.info

    lcov --remove coverage.info '/usr/*' --output-file coverage.filtered.info

    genhtml coverage.filtered.info --output-directory out
else
    echo "L1 UNIT TEST FAILED. PLEASE CHECK AND FIX"
    exit 1
fi
             
