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

ENABLE_COV=true # Keep this true if you always want coverage

if [ "x$1" = "x--enable-cov" ]; then
      echo "Enabling coverage options"
      export CXXFLAGS="-g -O0 -fprofile-arcs -ftest-coverage"
      export CFLAGS="-g -O0 -fprofile-arcs -ftest-coverage"
      export LDFLAGS="-lgcov --coverage"
      ENABLE_COV=true # This line is redundant if ENABLE_COV is already true above
fi

export TOP_DIR=`pwd`
export top_srcdir=`pwd`

# Create a directory for Gtest XML reports *before* changing directory
# This directory will be relative to TOP_DIR
GTEST_REPORT_DIR="$TOP_DIR/gtest_xml_reports"
mkdir -p "$GTEST_REPORT_DIR"

cd unittest/

automake --add-missing
autoreconf --install

./configure \
    CXXFLAGS="${CXXFLAGS}" \
    CFLAGS="${CFLAGS}" \
    LDFLAGS="${LDFLAGS}"

make clean
make

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
    report_file="${GTEST_REPORT_DIR}/${test_binary##*/}.xml" # Construct XML path relative to TOP_DIR
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
done

if [ $fail -ne 0 ]; then
    echo "Some unit tests failed."
    exit 1
fi

echo "********************"
echo "**** CAPTURE RDM-AGENT COVERAGE DATA ****"
echo "********************"
if [ "$ENABLE_COV" = true ]; then
    lcov --directory .. --zerocounters
    lcov --capture --directory .. --output-file "$TOP_DIR"/coverage.info # Save to TOP_DIR directly
    lcov --remove "$TOP_DIR"/coverage.info '/usr/*' --output-file "$TOP_DIR"/coverage.info
    lcov --remove "$TOP_DIR"/coverage.info '*/unittest/*.cpp' --output-file "$TOP_DIR"/coverage.info
    lcov --remove "$TOP_DIR"/coverage.info '*/unittest/*.h' --output-file "$TOP_DIR"/coverage.info
    lcov --remove "$TOP_DIR"/coverage.info '*/mocks/*.h' --output-file "$TOP_DIR"/coverage.info
    lcov --remove "$TOP_DIR"/coverage.info '*/mocks/*.cpp' --output-file "$TOP_DIR"/coverage.info
    lcov --remove "$TOP_DIR"/coverage.info '*/build/*' --output-file "$TOP_DIR"/coverage.info
    lcov --remove "$TOP_DIR"/coverage.info '*/tmp/*' --output-file "$TOP_DIR"/coverage.info
    lcov --remove "$TOP_DIR"/coverage.info '*test.cpp' --output-file "$TOP_DIR"/coverage.info # Catch any gtest file
    lcov --list "$TOP_DIR"/coverage.info
fi
cd "$TOP_DIR" 
