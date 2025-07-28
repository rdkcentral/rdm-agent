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
fi

export TOP_DIR=`pwd`
export top_srcdir=`pwd`

mkdir -p /tmp/gtest_reports/

cd unittest/

echo "--- Current directory for building and testing: $(pwd) ---"
echo "--- Exported CXXFLAGS: $CXXFLAGS"
echo "--- Exported CFLAGS: $CFLAGS"
echo "--- Exported LDFLAGS: $LDFLAGS"

automake --add-missing
autoreconf --install

echo "--- Running configure with explicit flags ---"
./configure \
    CXXFLAGS="${CXXFLAGS}" \
    CFLAGS="${CFLAGS}" \
    LDFLAGS="${LDFLAGS}"

echo "--- Running make clean ---"
make clean

echo "--- Running make with explicit flags (watching for -ftest-coverage) ---"
# Capture make output to a file and display it
make CXXFLAGS="${CXXFLAGS}" CFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS}" 2>&1 | tee make_output.log
cat make_output.log | grep -E "g\+\+|gcc|gcov|coverage" || echo "No compiler/coverage flags seen in make output."

fail=0

for test_binary in \
  ./rdm_main_gtest \
  ./rdm_utils_gtest \
  ./rdm_curl_gtest \
  ./rdm_json_gtest \
  ./rdm_download_gtest \
  ./rdm_downloadutils_gtest \
  ./rdm_rbus_gtest \
  ./rdm_openssl_gtest \
  ./rdm_usbinstall_gtest
do
    report_file="/tmp/gtest_reports/${test_binary}.xml" # Define output path for XML report
    echo "Running $test_binary with XML output to $report_file..."
    # Execute the test binary with gtest_output flag
    ./$test_binary --gtest_output=xml:"${report_file}"
    if [ $status -ne 0 ]; then
        echo "Test $test_binary failed with exit code $status"
        fail=1
    else
        echo "Test $test_binary passed"
    fi
    echo "------------------------------------"
done

if [ $fail -ne 0 ]; then
    echo "Some unit tests failed."
    exit 1
fi

echo "********************"
echo "**** CAPTURE RDM-AGENT COVERAGE DATA ****"
echo "********************"

cd "$TOP_DIR"
if [ "$ENABLE_COV" = true ]; then
    echo "Generating coverage report"

    lcov --capture --directory . --base-directory . --output-file raw_coverage.info
    lcov --remove raw_coverage.info '/unittest/*' --output-file processed_coverage.info
    lcov --extract processed_coverage.info '*/src/* --output-file coverage.info
    lcov --list coverage.info
fi

