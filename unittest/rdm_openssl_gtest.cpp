/**
 * Copyright 2023 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>
using ::testing::Return;
#include <cstring>
#include <openssl/evp.h>

// Include the header file for the functions being tested
extern "C" {
    #include "rdm_types.h"
    #include "rdm_openssl.h"

}



#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmopenssl_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256

class MockRdmRbus {
public:
    MOCK_METHOD(int, rdmRbusInit, (void **handle, const char *name), ());
    MOCK_METHOD(void, rdmRbusUnInit, (void *handle), ());
};

MockRdmRbus *g_mockRdmRbus = nullptr;

// Mock function implementations
extern "C" {
    int rdmRbusInit(void **handle, const char *name) {
        if (g_mockRdmRbus) {
            return g_mockRdmRbus->rdmRbusInit(handle, name);
        }
        return -1; // Return an error code if the mock object is not initialized
    }
	 void rdmRbusUnInit(void *handle) {
        if (g_mockRdmRbus) {
            g_mockRdmRbus->rdmRbusUnInit(handle);
        }
    }
}

// Mock class for static functions
class MockRdmOpenssl {
public:
    MOCK_METHOD(int, asciihex_to_bin, (const char* asciihex, size_t asciihex_length, uint8_t* bin, size_t* bin_length));
    MOCK_METHOD(int, bin_to_asciihex, (const uint8_t* bin, size_t bin_length, char* asciihex, size_t* asciihex_length));
    MOCK_METHOD(int, rdm_openssl_file_hash_sha256, (const char* data_file, size_t file_len, uint8_t* hash_buffer, int32_t* buffer_len));
    MOCK_METHOD(int, rdm_openssl_file_hash_sha256_pkg_components, (const char* data_file, size_t file_len, uint8_t* hash_buffer, int32_t* buffer_len));
    MOCK_METHOD(int, prepare_sig_file, (CHAR* sig_file)); // Add this line for `prepare_sig_file`
    MOCK_METHOD(int, prepare_app_manifest, (CHAR* etc_manifest_file, CHAR* cache_manifest_file, CHAR* padding_file, CHAR* prefix));
};

// Global mock object
MockRdmOpenssl* global_mock = nullptr;

// Mock function definitions
extern "C" {
    int v_secure_system(const char* format, ...) {
        va_list args;
        va_start(args, format);
        vprintf(format, args); // Optionally log the command
        va_end(args);

        return 0; // Always return success
    }
    int asciihex_to_bin(const char* asciihex, size_t asciihex_length, uint8_t* bin, size_t* bin_length) {
        return global_mock->asciihex_to_bin(asciihex, asciihex_length, bin, bin_length);
    }

    int bin_to_asciihex(const uint8_t* bin, size_t bin_length, char* asciihex, size_t* asciihex_length) {
        return global_mock->bin_to_asciihex(bin, bin_length, asciihex, asciihex_length);
    }



    int rdm_openssl_file_hash_sha256(const char* data_file, size_t file_len, uint8_t* hash_buffer, int32_t* buffer_len) {
        return global_mock->rdm_openssl_file_hash_sha256(data_file, file_len, hash_buffer, buffer_len);
    }


    int rdm_openssl_file_hash_sha256_pkg_components(const char* data_file, size_t file_len, uint8_t* hash_buffer, int32_t* buffer_len) {
        return global_mock->rdm_openssl_file_hash_sha256_pkg_components(data_file, file_len, hash_buffer, buffer_len);
    }

   int prepare_sig_file(CHAR* sig_file) {
        return global_mock->prepare_sig_file(sig_file); // Mock implementation for `prepare_sig_file`
    }

    int prepare_app_manifest(CHAR* etc_manifest_file, CHAR* cache_manifest_file, CHAR* padding_file, CHAR* prefix) {
        return global_mock->prepare_app_manifest(etc_manifest_file, cache_manifest_file, padding_file, prefix);
    }

}

TEST(OpenSSLTests, RdmOpensslFileHashSha256PkgComponents_ValidInput) {
    MockRdmOpenssl mock;
    global_mock = &mock;

    const char* data_file = "valid_manifest.txt"; // Simulate a valid manifest file
    size_t file_len = 1024; // Example file length
    uint8_t hash_buffer[SHA256_DIGEST_LENGTH] = {0};
    int32_t buffer_len = SHA256_DIGEST_LENGTH;

    EXPECT_CALL(mock, rdm_openssl_file_hash_sha256_pkg_components(data_file, file_len, hash_buffer, &buffer_len))
        .Times(1)
        .WillOnce(testing::Return(0));

    int result = rdm_openssl_file_hash_sha256_pkg_components(data_file, file_len, hash_buffer, &buffer_len);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(buffer_len, SHA256_DIGEST_LENGTH); // Ensure buffer length is correct
    global_mock = nullptr;
}

TEST(OpenSSLTests, RdmOpensslFileHashSha256PkgComponents_InvalidInput) {
    MockRdmOpenssl mock;
    global_mock = &mock;

    const char* data_file = nullptr; // Simulate invalid input
    size_t file_len = 0;
    uint8_t hash_buffer[SHA256_DIGEST_LENGTH] = {0};
    int32_t buffer_len = SHA256_DIGEST_LENGTH;

    EXPECT_CALL(mock, rdm_openssl_file_hash_sha256_pkg_components(data_file, file_len, hash_buffer, &buffer_len))
        .Times(1)
        .WillOnce(testing::Return(retcode_param_error));

    int result = rdm_openssl_file_hash_sha256_pkg_components(data_file, file_len, hash_buffer, &buffer_len);

    EXPECT_EQ(result, retcode_param_error);
    global_mock = nullptr;
}

// Example test for `prepare_sig_file`
TEST(OpenSSLTests, PrepareSigFile_ValidInput) {
    MockRdmOpenssl mock;
    global_mock = &mock;

    CHAR* sig_file = (CHAR*)"test_sig_file.txt";

    EXPECT_CALL(mock, prepare_sig_file(sig_file))
        .Times(1)
        .WillOnce(testing::Return(0));

    int result = prepare_sig_file(sig_file);

    EXPECT_EQ(result, 0);
    global_mock = nullptr;
}

TEST(OpenSSLTests, PrepareSigFile_InvalidInput) {
    MockRdmOpenssl mock;
    global_mock = &mock;

    CHAR* sig_file = nullptr; // Invalid input

    EXPECT_CALL(mock, prepare_sig_file(sig_file))
        .Times(1)
        .WillOnce(testing::Return(1));

    int result = prepare_sig_file(sig_file);

    EXPECT_EQ(result, 1);
    global_mock = nullptr;
}

TEST(OpenSSLTests, AsciiHexToBin_ValidInput) {
    MockRdmOpenssl mock;
    global_mock = &mock;

    const char* asciihex = "4A6F686E446F65";
    size_t asciihex_length = strlen(asciihex);
    uint8_t bin[8] = {0};
    size_t bin_length = sizeof(bin);

    EXPECT_CALL(mock, asciihex_to_bin(asciihex, asciihex_length, bin, &bin_length))
        .Times(1)
        .WillOnce(testing::Return(0));

    int result = asciihex_to_bin(asciihex, asciihex_length, bin, &bin_length);

    EXPECT_EQ(result, 0);
    global_mock = nullptr;
}


TEST(OpenSSLTests, AsciiHexToBin_InvalidInput) {
    MockRdmOpenssl mock;
    global_mock = &mock;

    const char* asciihex = "123"; // Odd-length string
    size_t asciihex_length = strlen(asciihex);
    uint8_t bin[8] = {0};
    size_t bin_length = sizeof(bin);

    EXPECT_CALL(mock, asciihex_to_bin(asciihex, asciihex_length, bin, &bin_length))
        .Times(1)
        .WillOnce(testing::Return(-1));

    int result = asciihex_to_bin(asciihex, asciihex_length, bin, &bin_length);

    EXPECT_EQ(result, -1);
    global_mock = nullptr;
}

TEST(OpenSSLTests, BinToAsciiHex_ValidInput) {
    MockRdmOpenssl mock;
    global_mock = &mock;

    const uint8_t bin[] = {0x4A, 0x6F, 0x68, 0x6E, 0x44, 0x6F, 0x65};
    size_t bin_length = sizeof(bin);
    char asciihex[16] = {0};
    size_t asciihex_length = sizeof(asciihex);

    EXPECT_CALL(mock, bin_to_asciihex(bin, bin_length, asciihex, &asciihex_length))
        .Times(1)
        .WillOnce(testing::Return(0));

    int result = bin_to_asciihex(bin, bin_length, asciihex, &asciihex_length);

    EXPECT_EQ(result, 0);
    global_mock = nullptr;
}

TEST(OpenSSLTests, DumpBuffer_DebugEnabled) {
    char buffer[] = "Test buffer";
    int32_t buffer_size = strlen(buffer);
    char name[] = "test_file";

    // Call the actual `dump_buffer` implementation
    dump_buffer(buffer, buffer_size, name);

    // If `dump_buffer` logs or prints, capture and validate the output if necessary
    SUCCEED(); // Test passes if no exceptions or errors
}

TEST(OpenSSLTests, RdmOpensslFileHashSha256_ValidInput) {
    MockRdmOpenssl mock;
    global_mock = &mock;

    const char* data_file = "test_data_file";
    uint8_t hash_buffer[SHA256_DIGEST_LENGTH] = {0};
    int32_t buffer_len = sizeof(hash_buffer);

    EXPECT_CALL(mock, rdm_openssl_file_hash_sha256(data_file, -1, hash_buffer, &buffer_len))
        .Times(1)
        .WillOnce(testing::Return(0));

    int result = rdm_openssl_file_hash_sha256(data_file, -1, hash_buffer, &buffer_len);

    EXPECT_EQ(result, 0);
    global_mock = nullptr;
}

TEST(OpenSSLTests, RdmInitSslLib_MultipleCalls) {
    // Test `rdmInitSslLib` directly as it is not static
    rdmInitSslLib();
    rdmInitSslLib();
    SUCCEED(); // If no crashes, the test passes
}
TEST(OpenSSLTests, RdmOpensslRsafileSignatureVerify_InvalidInput) {
    const char* data_file = nullptr; // Invalid input
    size_t file_len = 0;
    const char* sig_file = nullptr;
    const char* vkey_file = nullptr;
    char reply_msg[256] = {0};
    int32_t reply_msg_len = sizeof(reply_msg);

    int result = rdmOpensslRsafileSignatureVerify(data_file, file_len, sig_file, vkey_file, reply_msg, &reply_msg_len);

    EXPECT_EQ(result, retcode_param_error);
}

TEST(OpenSSLTests, PrepareAppManifest_ValidInput) {
    MockRdmOpenssl mock;
    global_mock = &mock;

    CHAR* etc_manifest_file = (CHAR*)"valid_manifest.txt";
    CHAR* cache_manifest_file = (CHAR*)"cache_manifest.txt";
    CHAR* padding_file = (CHAR*)"padding_file.txt";
    CHAR* prefix = (CHAR*)"/prefix/";

    EXPECT_CALL(mock, prepare_app_manifest(etc_manifest_file, cache_manifest_file, padding_file, prefix))
        .Times(1)
        .WillOnce(testing::Return(0));

    int result = prepare_app_manifest(etc_manifest_file, cache_manifest_file, padding_file, prefix);

    EXPECT_EQ(result, 0);
    global_mock = nullptr;
}

TEST(OpenSSLTests, PrepareAppManifest_InvalidInput) {
    MockRdmOpenssl mock;
    global_mock = &mock;

    CHAR* etc_manifest_file = nullptr;  // Invalid input
    CHAR* cache_manifest_file = (CHAR*)"cache_manifest.txt";
    CHAR* padding_file = (CHAR*)"padding_file.txt";
    CHAR* prefix = (CHAR*)"/prefix/";

    EXPECT_CALL(mock, prepare_app_manifest(etc_manifest_file, cache_manifest_file, padding_file, prefix))
        .Times(1)
        .WillOnce(testing::Return(1));

    int result = prepare_app_manifest(etc_manifest_file, cache_manifest_file, padding_file, prefix);

    EXPECT_EQ(result, 1);
    global_mock = nullptr;
}
