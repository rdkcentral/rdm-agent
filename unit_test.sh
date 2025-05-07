#!/bin/bash

#
## Copyright 2023 Comcast Cable Communications Management, LLC
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
## http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## SPDX-License-Identifier: Apache-2.0
#

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
echo "*********** Return value of rdm_rbus_gtest $rdmrbus"

./rdm_openssl_gtest
rdmopenssl=$?
echo "*********** Return value of rdm_openssl_gtest $rdmopenssl"

./rdm_usbinstall_gtest
rdmusbinstall=$?
echo "*********** Return value of rdm_usbinstall_gtest $rdmusbinstall"

# List of unit test executables

# Run tests and capture return values
if [ "$rdmmain" = "0" ] && [ "$utils" = "0" ] && [ "$rdmcurl" = "0" ] && [ "$rdmjson" = "0" ] && [ "$rdmdown" = "0" ] && [ "$rdmdutils" = "0" ] && [ "$rdmrbus" = "0" ] && [ "$rdmopenssl" = "0" ] && [ "$rdmusbinstall" = "0" ]; then
    cd ../

    lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage.filtered.info

    genhtml coverage.filtered.info --output-directory out
else
    echo "L1 UNIT TEST FAILED. PLEASE CHECK AND FIX"
    exit 1
fi

