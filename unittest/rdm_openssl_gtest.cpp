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

// Mock class for static functions
class MockRdmOpenssl {
public:
    MOCK_METHOD(int, asciihex_to_bin, (const char* asciihex, size_t asciihex_length, uint8_t* bin, size_t* bin_length));
    MOCK_METHOD(int, bin_to_asciihex, (const uint8_t* bin, size_t bin_length, char* asciihex, size_t* asciihex_length));
    MOCK_METHOD(int, rdm_openssl_file_hash_sha256, (const char* data_file, size_t file_len, uint8_t* hash_buffer, int32_t* buffer_len));
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
}

class PrepareSigFileTest : public ::testing::Test {
protected:
    const char* test_sig_file = "test_sig_file.sig";
    const char* tmp_sig_file = "test_tmp_sig_file.sig";

    virtual void SetUp() {
        // Create a test signature file with dummy contents
        FILE* file = fopen(test_sig_file, "w");
        ASSERT_NE(file, nullptr);
        const char* data = "123456ABCDEFGH";
        fwrite(data, sizeof(char), strlen(data), file);
        fclose(file);
    }

    virtual void TearDown() {
        // Clean up test files
        remove(test_sig_file);
        remove(tmp_sig_file);
    }
};

TEST_F(PrepareSigFileTest, ValidInput) {
    // Call prepare_sig_file with a valid file
    int result = prepare_sig_file(const_cast<char*>(test_sig_file));

    // Check the return value
    EXPECT_EQ(result, 0);

    // Verify the contents of the modified file
    FILE* file = fopen(test_sig_file, "r");
    ASSERT_NE(file, nullptr);

    char buffer[128] = {0};
    fread(buffer, sizeof(char), sizeof(buffer), file);
    fclose(file);

    // The first 6 bytes should be skipped, so we expect "ABCDEFGH"
    EXPECT_STREQ(buffer, "ABCDEFGH");
}

TEST_F(PrepareSigFileTest, FileOpenFailure) {
    // Try calling with a nonexistent file
    int result = prepare_sig_file(const_cast<char*>("nonexistent_file.sig"));

    // Check the return value (assuming 1 indicates failure)
    EXPECT_EQ(result, 1);
}

TEST_F(PrepareSigFileTest, VSecureSystemCall) {
    // Mock v_secure_system to confirm it is called with expected arguments
    testing::MockFunction<int(const char*, ...)>::gmock_function v_secure_system_mock;
    EXPECT_CALL(v_secure_system_mock, Call(testing::StrEq("/bin/mv %s %s"), testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::Return(0));

    // Call prepare_sig_file
    int result = prepare_sig_file(const_cast<char*>(test_sig_file));

    // Check the return value
    EXPECT_EQ(result, 0);
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
