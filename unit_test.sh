#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status

cd unit-test/

# Generate required files for automake
automake --add-missing
autoreconf --install

# Run configure script
./configure

# Clean and build the project
make clean
make

# List of unit test executables
TESTS=(
    "rdm_curl_gtest"
    "rdm_download_gtest"
    "rdm_downloadmgr_gtest"
    "rdm_downloadutils_gtest"
    "json_parse_gtest"
    "downloadUtil_gtest"
    "rdm_json_gtest"
    "rdm_service_gtest"
    "rdm_main_gtest"
    "rdm_openssl_gtest"
    "rdm_downloadverapp_gtest"
    "rdm_utils_gtest"
    "rdm_usbinstall_gtest"
    "codebig_utils_gtest"
)

# Run tests and capture return values
FAILED_TESTS=()
for test in "${TESTS[@]}"; do
    ./"$test"
    ret_val=$?
    echo "*********** Return value of $test = $ret_val"
    
    if [ "$ret_val" -ne 0 ]; then
        FAILED_TESTS+=("$test")
    fi
done

# Check if any tests failed
if [ ${#FAILED_TESTS[@]} -eq 0 ]; then
    cd ../

    echo "Generating coverage report..."
    lcov --capture --directory . --output-file coverage.info
    lcov --remove coverage.info '/usr/*' --output-file coverage.filtered.info
    genhtml coverage.filtered.info --output-directory out
else
    echo "L1 UNIT TEST FAILED. PLEASE CHECK AND FIX"
    echo "Failed Tests: ${FAILED_TESTS[*]}"
    exit 1
fi

echo "All tests passed successfully."
exit 0
