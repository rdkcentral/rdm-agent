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

ENABLE_COV=true

if [ "x$1" = "x--enable-cov" ]; then
      echo "Enabling coverage options"
      export CXXFLAGS="-g -O0 -fprofile-arcs -ftest-coverage"
      export CFLAGS="-g -O0 -fprofile-arcs -ftest-coverage"
      export LDFLAGS="-lgcov --coverage"
      ENABLE_COV=true
fi

export TOP_DIR=`pwd`
export top_srcdir=`pwd`

mkdir -p /tmp/gtest_reports/

cd ./unittest/

automake --add-missing
autoreconf --install


./configure
make

mkdir -p /opt/secure/

# Create a directory to store Gtest XML reports
mkdir -p /tmp/gtest_reports/

fail=0

run_test() {
    test_binary="$1"
    report_file="/tmp/gtest_reports/${test_binary}.xml" # Define output path for XML report
    echo "Running $test_binary with XML output to $report_file..."
    # Execute the test binary with gtest_output flag
    ./$test_binary --gtest_output=xml:"${report_file}"
    status=$?
    if [ $status -ne 0 ]; then
        echo "Test $test_binary failed with exit code $status"
        fail=1
    else
        echo "Test $test_binary passed"
    fi
    echo "------------------------------------"
}


  run_test rdm_main_gtest 
  run_test rdm_utils_gtest 
  run_test rdm_curl_gtest 
  run_test rdm_json_gtest 
  run_test rdm_download_gtest 
  run_test rdm_downloadutils_gtest 
  run_test rdm_rbus_gtest 
  run_test rdm_openssl_gtest 
  run_test rdm_usbinstall_gtest


 
 if [ $fail -ne 0 ]; then
    echo "Some unit tests failed."
    exit 1
fi


echo "********************"
echo "**** CAPTURE RDM-AGENT COVERAGE DATA ****"
echo "********************"

if [ "$ENABLE_COV" = true ]; then
    echo "Generating coverage report"

    lcov --capture --directory . --base-directory . --output-file raw_coverage.info
    lcov --extract raw_coverage.info '/__w/rdm-agent/rdm-agent/src/rdm_*.c'  '/__w/rdm-agent/rdm-agent/unittest/*.cpp' '/__w/rdm-agent/rdm-agent/rdm_main.c' --output-file rdm_coverage.info
    lcov --remove rdm_coverage.info '/__w/rdm-agent/rdm-agent/unittest/*' --output-file processed_coverage.info
    lcov --extract processed_coverage.info '*.c' --output-file coverage.info
    lcov --list coverage.info
fi

cd "$TOP_DIR" # Use double quotes for robust path handling
