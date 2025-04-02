#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstring>
#include <stdio.h>
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "mocks/mock_curl.h"
extern "C" {
    #include "rdm_curldownload.h"
}


#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmcurl_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256


using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::DoAll;
using ::testing::StrEq;

MockCurl* mockCurl;

class RDMTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockCurl = new MockCurl();
    }

    void TearDown() override {
        delete mockCurl;
    }
};
TEST_F(RDMTest, rdmCurlInit_Success) {
    CURL* mockCurlHandle = reinterpret_cast<CURL*>(0x1); // Use a dummy non-null pointer
    EXPECT_CALL(*mockCurl, curl_easy_init())
        .WillOnce(Return(&mockCurlHandle));

    void* curlHandle = nullptr;
    EXPECT_EQ(rdmCurlInit(&curlHandle), RDM_SUCCESS);
    EXPECT_EQ(curlHandle, &mockCurlHandle);
}

TEST_F(RDMTest, rdmCurlInit_Failure) {
    EXPECT_CALL(*mockCurl, curl_easy_init())
        .WillOnce(Return(nullptr));

    void* curlHandle = nullptr;
    EXPECT_EQ(rdmCurlInit(&curlHandle), RDM_FAILURE);
}

TEST_F(RDMTest, rdmCurlUnInit) {
     CURL* mockCurlHandle = reinterpret_cast<CURL*>(0x1); // Use a dummy non-null pointer
    EXPECT_CALL(*mockCurl, curl_easy_cleanup(&mockCurlHandle));

    rdmCurlUnInit(&mockCurlHandle);
}
TEST_F(RDMTest, rdmCurlSetParams_Success) {
    CURL* mockCurlHandle = reinterpret_cast<CURL*>(0x1); // Use a dummy non-null pointer
    RDMCurlParams curlParams = {};
    RDMCertParam certParams = {};
    curlParams.cert_param = certParams;
    curlParams.url =  const_cast<CHAR*>("http://example.com");
    curlParams.ssl_version = CURL_SSLVERSION_TLSv1_2;
    curlParams.conn_timeout = 10;
    curlParams.full_conn_timeout = 20;
    curlParams.callback = nullptr;
    curlParams.callback_param = nullptr;

    EXPECT_CALL(*mockCurl, curl_easy_setopt_str(&mockCurlHandle, CURLOPT_URL, StrEq(curlParams.url)))
        .WillOnce(Return(CURLE_OK));
    EXPECT_CALL(*mockCurl, curl_easy_setopt_long(&mockCurlHandle, CURLOPT_SSLVERSION, curlParams.ssl_version))
        .WillOnce(Return(CURLE_OK));
    EXPECT_CALL(*mockCurl, curl_easy_setopt_long(&mockCurlHandle, CURLOPT_CONNECTTIMEOUT, curlParams.conn_timeout))
        .WillOnce(Return(CURLE_OK));
    EXPECT_CALL(*mockCurl, curl_easy_setopt_long(&mockCurlHandle, CURLOPT_TIMEOUT, curlParams.full_conn_timeout))
        .WillOnce(Return(CURLE_OK));
    EXPECT_CALL(*mockCurl, curl_easy_setopt_ptr(&mockCurlHandle, CURLOPT_WRITEFUNCTION, curlParams.callback))
        .WillOnce(Return(CURLE_OK));
    EXPECT_CALL(*mockCurl, curl_easy_setopt_ptr(&mockCurlHandle, CURLOPT_WRITEDATA, curlParams.callback_param))
        .WillOnce(Return(CURLE_OK));
    EXPECT_CALL(*mockCurl, curl_easy_setopt_long(&mockCurlHandle, CURLOPT_SSLENGINE_DEFAULT, certParams.def_val))
        .WillOnce(Return(CURLE_OK));
    EXPECT_CALL(*mockCurl, curl_easy_setopt_str(&mockCurlHandle, CURLOPT_SSLCERTTYPE, certParams.sslcert_type))
        .WillOnce(Return(CURLE_OK));
    EXPECT_CALL(*mockCurl, curl_easy_setopt_str(&mockCurlHandle, CURLOPT_SSLCERT, certParams.sslcert_path))
        .WillOnce(Return(CURLE_OK));
    EXPECT_CALL(*mockCurl, curl_easy_setopt_str(&mockCurlHandle, CURLOPT_KEYPASSWD, certParams.sslkey_pass))
        .WillOnce(Return(CURLE_OK));
    EXPECT_CALL(*mockCurl, curl_easy_setopt_long(&mockCurlHandle, CURLOPT_SSL_VERIFYPEER, certParams.ssl_peerverify))
        .WillOnce(Return(CURLE_OK));
		
    EXPECT_EQ(rdmCurlSetParams(&mockCurlHandle, &curlParams), RDM_SUCCESS);
}


TEST_F(RDMTest, rdmCurlDwnStart_Success) {
    CURL* mockCurlHandle = reinterpret_cast<CURL*>(0x1); // Use a dummy non-null pointer
    CURLcode curlCode = CURLE_OK;
    INT64 httpCode = 200;

    EXPECT_CALL(*mockCurl, curl_easy_perform(&mockCurlHandle))
        .WillOnce(Return(curlCode));
    EXPECT_CALL(*mockCurl, curl_easy_getinfo_long(&mockCurlHandle, CURLINFO_RESPONSE_CODE, _))
        .WillOnce(DoAll(SetArgPointee<2>(httpCode), Return(curlCode)));

    EXPECT_EQ(rdmCurlDwnStart(&mockCurlHandle), RDM_SUCCESS);
}

TEST_F(RDMTest, rdmCurlDwnStart_Failure) {
    CURL* mockCurlHandle = reinterpret_cast<CURL*>(0x1); // Use a dummy non-null pointer
    CURLcode curlCode = CURLE_FAILED_INIT;
    INT64 httpCode = 500;

    EXPECT_CALL(*mockCurl, curl_easy_perform(&mockCurlHandle))
        .WillOnce(Return(curlCode));
    EXPECT_CALL(*mockCurl, curl_easy_getinfo_long(&mockCurlHandle, CURLINFO_RESPONSE_CODE, _))
        .WillOnce(DoAll(SetArgPointee<2>(httpCode), Return(CURLE_OK)));

    EXPECT_EQ(rdmCurlDwnStart(&mockCurlHandle), RDM_FAILURE);
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
