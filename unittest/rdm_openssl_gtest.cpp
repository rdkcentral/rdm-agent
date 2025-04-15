#include <gtest/gtest.h>
#include <cstring>
#include <openssl/evp.h>

// Include the header file for the functions being tested
extern "C" {
    #include "rdm_types.h"  
    #include "rdm_openssl.h"
   
}

extern "C" {
    INT32 asciihex_to_bin(const char* asciihex, size_t asciihex_length, uint8_t* bin, size_t* bin_length);
    INT32 bin_to_asciihex(const uint8_t* bin, size_t bin_length, char* asciihex, size_t* asciihex_length);
    void dump_buffer(const char* buffer, int32_t buffer_size, const char* name);
    INT32 rdm_openssl_file_hash_sha256(const char* file_path, int32_t file_descriptor, uint8_t* hash_buffer, int32_t* buffer_len);
}

TEST(OpenSSLTests, AsciiHexToBin_ValidInput) {
    const char* asciihex = "4A6F686E446F65";
    size_t asciihex_length = strlen(asciihex);
    uint8_t bin[8] = {0};
    size_t bin_length = sizeof(bin);

    int result = asciihex_to_bin(asciihex, asciihex_length, bin, &bin_length);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(bin_length, 7);
    EXPECT_EQ(bin[0], 0x4A);
    EXPECT_EQ(bin[1], 0x6F);
    EXPECT_EQ(bin[2], 0x68);
    EXPECT_EQ(bin[3], 0x6E);
    EXPECT_EQ(bin[4], 0x44);
    EXPECT_EQ(bin[5], 0x6F);
    EXPECT_EQ(bin[6], 0x65);
}

TEST(OpenSSLTests, AsciiHexToBin_InvalidInput) {
    const char* asciihex = "123"; // Odd-length string
    size_t asciihex_length = strlen(asciihex);
    uint8_t bin[8] = {0};
    size_t bin_length = sizeof(bin);

    int result = asciihex_to_bin(asciihex, asciihex_length, bin, &bin_length);

    EXPECT_EQ(result, -1);
}

TEST(OpenSSLTests, BinToAsciiHex_ValidInput) {
    const uint8_t bin[] = {0x4A, 0x6F, 0x68, 0x6E, 0x44, 0x6F, 0x65};
    size_t bin_length = sizeof(bin);
    char asciihex[16] = {0};
    size_t asciihex_length = sizeof(asciihex);

    int result = bin_to_asciihex(bin, bin_length, asciihex, &asciihex_length);

    EXPECT_EQ(result, 0);
    EXPECT_STREQ(asciihex, "4a6f686e446f65");
}

TEST(OpenSSLTests, DumpBuffer_DebugEnabled) {
    // Simulate buffer and file output in debug mode
    char buffer[] = "Test buffer";
    int32_t buffer_size = strlen(buffer);
    char name[] = "test_file";

    // Call dump_buffer (should only work in debug mode)
    dump_buffer(buffer, buffer_size, name);

    // Output verification might need file or log checking
    SUCCEED(); // Assuming no crashes, the test passes
}

TEST(OpenSSLTests, RdmInitSslLib_MultipleCalls) {
    // Calling rdmInitSslLib multiple times should not cause issues
    rdmInitSslLib();
    rdmInitSslLib();

    SUCCEED(); // If no crashes, the test passes
}

TEST(OpenSSLTests, RdmOpensslFileHashSha256_ValidInput) {
    const char* data_file = "test_data_file";
    uint8_t hash_buffer[SHA256_DIGEST_LENGTH] = {0};
    int32_t buffer_len = sizeof(hash_buffer);

    // Mock or create a test file for hashing
    FILE* file = fopen(data_file, "w");
    fprintf(file, "Test data for hashing");
    fclose(file);

    int result = rdm_openssl_file_hash_sha256(data_file, -1, hash_buffer, &buffer_len);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(buffer_len, SHA256_DIGEST_LENGTH);

    // Cleanup
    remove(data_file);
}

TEST(OpenSSLTests, RdmOpensslRsafileSignatureVerify_InvalidInput) {
    const char* data_file = nullptr; // Invalid input
    size_t file_len = 0;
    const char* sig_file = nullptr;
    const char* vkey_file = nullptr;
    char reply_msg[256] = {0};
    int32_t reply_msg_len = sizeof(reply_msg);

    int result = rdmOpensslRsafileSignatureVerify(data_file, file_len, sig_file, vkey_file, reply_msg, &reply_msg_len);

    EXPECT_EQ(result, -1);
}
