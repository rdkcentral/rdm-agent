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

GTEST_REPORT_DIR="$TOP_DIR/gtest_xml_reports"
mkdir -p "$GTEST_REPORT_DIR"

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
    report_file="${GTEST_REPORT_DIR}/${test_binary##*/}.xml"
    echo "Running $test_binary with XML output to $report_file..."
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
    echo "--- Current directory for lcov operations: $(pwd) ---"

    echo "--- Searching for .gcno files after build ---"
    find .. -name "*.gcno" -print || echo "No .gcno files found."

    echo "--- Searching for .gcda files after test execution ---"
    find .. -name "*.gcda" -print || echo "No .gcda files found."

    echo "--- Running lcov --zerocounters ---"
    lcov --directory .. --zerocounters || echo "lcov zerocounters failed or found nothing."

    echo "--- Running lcov --capture ---"
    # Capture everything, including 'src/' (if relative to ..) and 'unittest/'
    lcov --capture --directory .. --output-file "$TOP_DIR"/coverage.info 2>&1 | tee lcov_capture_output.log
    cat lcov_capture_output.log | grep -E "WARNING|ERROR" || echo "No lcov capture warnings/errors seen (excluding subroutine redefinitions)."

    # Check if coverage.info was actually created and is not empty
    if [ -s "$TOP_DIR"/coverage.info ]; then
        echo "Successfully created non-empty coverage.info at $TOP_DIR/coverage.info"
        echo "--- Filtering coverage data ---"
        lcov --remove "$TOP_DIR"/coverage.info '/usr/*' --output-file "$TOP_DIR"/coverage.info
        lcov --remove "$TOP_DIR"/coverage.info '*/unittest/*.cpp' --output-file "$TOP_DIR"/coverage.info
        lcov --remove "$TOP_DIR"/coverage.info '*/unittest/*.h' --output-file "$TOP_DIR"/coverage.info
        lcov --remove "$TOP_DIR"/coverage.info '*/mocks/*.h' --output-file "$TOP_DIR"/coverage.info
        lcov --remove "$TOP_DIR"/coverage.info '*/mocks/*.cpp' --output-file "$TOP_DIR"/coverage.info
        lcov --remove "$TOP_DIR"/coverage.info '*/build/*' --output-file "$TOP_DIR"/coverage.info
        lcov --remove "$TOP_DIR"/coverage.info '*/tmp/*' --output-file "$TOP_DIR"/coverage.info
        lcov --remove "$TOP_DIR"/coverage.info '*test.cpp' --output-file "$TOP_DIR"/coverage.info

        echo "--- Final lcov list (filtered) ---"
        lcov --list "$TOP_DIR"/coverage.info
    else
        echo "ERROR: coverage.info is empty or not created! Cannot generate report."
    fi
fi

cd "$TOP_DIR"
