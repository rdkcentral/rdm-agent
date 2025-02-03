#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rdm_openssl.h"
#include "rdm.h"
#include "rdm_types.h"
#include "rdm_utils.h"
#include "openssl/evp.h"
#include "openssl/pem.h"
#include "openssl/err.h"

using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::_;

class MockOpenSSL {
public:
    MOCK_METHOD(int, EVP_DigestInit_ex, (EVP_MD_CTX*, const EVP_MD*, ENGINE*), ());
    MOCK_METHOD(int, EVP_DigestUpdate, (EVP_MD_CTX*, const void*, size_t), ());
    MOCK_METHOD(int, EVP_DigestFinal_ex, (EVP_MD_CTX*, unsigned char*, unsigned int*), ());
    MOCK_METHOD(EVP_PKEY*, PEM_read_PUBKEY, (FILE*, EVP_PKEY**, pem_password_cb*, void*), ());
};

MockOpenSSL* mockOpenSSL;

extern "C" int EVP_DigestInit_ex(EVP_MD_CTX* ctx, const EVP_MD* type, ENGINE* impl) {
    return mockOpenSSL->EVP_DigestInit_ex(ctx, type, impl);
}
extern "C" int EVP_DigestUpdate(EVP_MD_CTX* ctx, const void* d, size_t cnt) {
    return mockOpenSSL->EVP_DigestUpdate(ctx, d, cnt);
}
extern "C" int EVP_DigestFinal_ex(EVP_MD_CTX* ctx, unsigned char* md, unsigned int* s) {
    return mockOpenSSL->EVP_DigestFinal_ex(ctx, md, s);
}
extern "C" EVP_PKEY* PEM_read_PUBKEY(FILE* fp, EVP_PKEY** x, pem_password_cb* cb, void* u) {
    return mockOpenSSL->PEM_read_PUBKEY(fp, x, cb, u);
}

class RdmOpenSSLTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockOpenSSL = new MockOpenSSL();
    }
    void TearDown() override {
        delete mockOpenSSL;
    }
};

TEST_F(RdmOpenSSLTest, Test_rdmOpensslRsafileSignatureVerify_Success) {
    const char* data_file = "test_data_file";
    size_t file_len = 1024;
    const char* sig_file = "test_sig_file";
    const char* vkey_file = "test_key.pem";
    char reply_msg[256];
    int reply_msg_len = sizeof(reply_msg);

    EXPECT_CALL(*mockOpenSSL, EVP_DigestInit_ex(_, _, _)).WillOnce(Return(1));
    EXPECT_CALL(*mockOpenSSL, EVP_DigestUpdate(_, _, _)).WillOnce(Return(1));
    EXPECT_CALL(*mockOpenSSL, EVP_DigestFinal_ex(_, _, _)).WillOnce(Return(1));
    EXPECT_CALL(*mockOpenSSL, PEM_read_PUBKEY(_, _, _, _)).WillOnce(Return((EVP_PKEY*)1));

    ASSERT_EQ(rdmOpensslRsafileSignatureVerify(data_file, file_len, sig_file, vkey_file, reply_msg, &reply_msg_len), 0);
}

TEST_F(RdmOpenSSLTest, Test_rdmOpensslRsafileSignatureVerify_Fail_InvalidParams) {
    ASSERT_EQ(rdmOpensslRsafileSignatureVerify(nullptr, 1024, "test_sig_file", "test_key.pem", nullptr, nullptr), -1);
}

TEST_F(RdmOpenSSLTest, Test_rdmOpensslRsafileSignatureVerify_Fail_DigestUpdate) {
    EXPECT_CALL(*mockOpenSSL, EVP_DigestUpdate(_, _, _)).WillOnce(Return(0));
    char reply_msg[256];
    int reply_msg_len = sizeof(reply_msg);
    ASSERT_NE(rdmOpensslRsafileSignatureVerify("test_data_file", 1024, "test_sig_file", "test_key.pem", reply_msg, &reply_msg_len), 0);
}

TEST_F(RdmOpenSSLTest, Test_rdmOpensslRsafileSignatureVerify_Fail_PubKeyRead) {
    EXPECT_CALL(*mockOpenSSL, PEM_read_PUBKEY(_, _, _, _)).WillOnce(Return(nullptr));
    char reply_msg[256];
    int reply_msg_len = sizeof(reply_msg);
    ASSERT_NE(rdmOpensslRsafileSignatureVerify("test_data_file", 1024, "test_sig_file", "test_key.pem", reply_msg, &reply_msg_len), 0);
}

TEST_F(RdmOpenSSLTest, Test_rdmSignatureVerify_Fail_InvalidInput) {
    ASSERT_NE(rdmSignatureVerify(nullptr, nullptr, 1), 0);
}

TEST_F(RdmOpenSSLTest, Test_rdmSignatureVerify_Success) {
    ASSERT_EQ(rdmSignatureVerify("/tmp/cache", "test_app", 1), 0);
}

TEST_F(RdmOpenSSLTest, Test_rdmDecryptKey_Success) {
    char out_key[256];
    ASSERT_EQ(rdmDecryptKey(out_key), 0);
}
