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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"


#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdm_main_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256

using namespace testing;
using namespace std;
using ::testing::Return;
using ::testing::StrEq;

extern "C" bool rdmTestIsValidInstallPackageToken(const char *token);

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

    void rdmHelp() {
         printf("Usage:\n");
        printf("To Install apps from manifest : rdm\n");
        printf("To Install single app         : rdm -a <app_name>\n");
        printf("To Install from USB           : rdm -u <usb_path>\n");
        printf("To Install Versioned app      : rdm -v <app_name>\n");
        printf("-b - for broadband devices\n");
        printf("-o - for OSS\n");
        printf("To Print help                 : rdm -h\n");
    }
}

class RDMTest : public ::testing::Test {
protected:
    RDMHandle* prdmHandle;

    virtual void SetUp() {
        prdmHandle = (RDMHandle*)malloc(sizeof(RDMHandle));
        memset(prdmHandle, 0, sizeof(RDMHandle));
        g_mockRdmRbus = new MockRdmRbus();
    }

    virtual void TearDown() {
        if (prdmHandle) {
            free(prdmHandle);
        }
        delete g_mockRdmRbus;
		g_mockRdmRbus = nullptr;
    }
};

TEST_F(RDMTest, rdmInit_NullHandle) {
    EXPECT_EQ(rdmInit(NULL), RDM_FAILURE);
}

TEST_F(RDMTest, rdmInit_Success) {
    EXPECT_CALL(*g_mockRdmRbus, rdmRbusInit(testing::_, testing::_))
        .WillOnce(testing::Return(RDM_SUCCESS));

    EXPECT_EQ(rdmInit(prdmHandle), RDM_SUCCESS);
    EXPECT_NE(prdmHandle->pApp_det, nullptr);
    free(prdmHandle->pApp_det);
}

TEST_F(RDMTest, rdmInit_Failure) {
    EXPECT_CALL(*g_mockRdmRbus, rdmRbusInit(testing::_, testing::_))
        .WillOnce(testing::Return(RDM_FAILURE));

    EXPECT_EQ(rdmInit(prdmHandle), RDM_FAILURE);
    free(prdmHandle->pApp_det);
}

TEST_F(RDMTest, rdmUnInit) {
    EXPECT_CALL(*g_mockRdmRbus, rdmRbusUnInit(prdmHandle->pRbusHandle));

    rdmUnInit(prdmHandle);

    EXPECT_EQ(prdmHandle->pApp_det, nullptr);
}
  TEST_F(RDMTest, rdmUnInit_NullAppDet) {
    if (prdmHandle->pApp_det) {
        free(prdmHandle->pApp_det);
        prdmHandle->pApp_det = nullptr;
    }

    EXPECT_CALL(*g_mockRdmRbus, rdmRbusUnInit(prdmHandle->pRbusHandle));

    rdmUnInit(prdmHandle);

    EXPECT_EQ(prdmHandle->pApp_det, nullptr);
}

TEST_F(RDMTest, rdmHelp_Output) {
    ::testing::internal::CaptureStdout();
    rdmHelp();
    std::string output = ::testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Usage:"), std::string::npos);
    EXPECT_NE(output.find("To Install apps from manifest : rdm"), std::string::npos);
    EXPECT_NE(output.find("To Install single app         : rdm -a <app_name>"), std::string::npos);
    EXPECT_NE(output.find("To Install from USB           : rdm -u <usb_path>"), std::string::npos);
    EXPECT_NE(output.find("To Install Versioned app      : rdm -v <app_name>"), std::string::npos);
    EXPECT_NE(output.find("-b - for broadband devices"), std::string::npos);
    EXPECT_NE(output.find("-o - for OSS"), std::string::npos);
    EXPECT_NE(output.find("To Print help                 : rdm -h"), std::string::npos);
}

TEST(RDMInstallPackageValidationTest, AcceptsValidPackageVersions) {
    EXPECT_TRUE(rdmTestIsValidInstallPackageToken("meminsight:1.0"));
    EXPECT_TRUE(rdmTestIsValidInstallPackageToken("stage-agent:12.34"));
    EXPECT_TRUE(rdmTestIsValidInstallPackageToken("package_name-1:0.8"));
}

TEST(RDMInstallPackageValidationTest, AcceptsBundlePrefixes) {
    EXPECT_TRUE(rdmTestIsValidInstallPackageToken("app:stage-agent:1.0"));
    EXPECT_TRUE(rdmTestIsValidInstallPackageToken("cert:ca-store-update-bundle:0.8"));
}

TEST(RDMInstallPackageValidationTest, RejectsInvalidPackageVersions) {
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken("meminsight:1"));
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken("meminsight:1.1.1"));
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken("meminsight:1."));
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken("meminsight:.1"));
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken("meminsight:1.a"));
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken("meminsight:1..0"));
}

TEST(RDMInstallPackageValidationTest, RejectsInvalidPackageNamesAndPrefixes) {
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken(""));
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken(nullptr));
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken(":1.0"));
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken("mem insight:1.0"));
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken("meminsight:1.0:extra"));
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken("app:meminsight:1.0"));
    EXPECT_FALSE(rdmTestIsValidInstallPackageToken("cert:bundle:1.0"));
}

GTEST_API_ int main(int argc, char *argv[]){
    char testresults_fullfilepath[GTEST_REPORT_FILEPATH_SIZE];
    char buffer[GTEST_REPORT_FILEPATH_SIZE];

    memset( testresults_fullfilepath, 0, GTEST_REPORT_FILEPATH_SIZE );
    memset( buffer, 0, GTEST_REPORT_FILEPATH_SIZE );

    snprintf( testresults_fullfilepath, GTEST_REPORT_FILEPATH_SIZE, "json:%s%s" , GTEST_DEFAULT_RESULT_FILEPATH , GTEST_DEFAULT_RESULT_FILENAME);
    ::testing::GTEST_FLAG(output) = testresults_fullfilepath;
    ::testing::InitGoogleTest(&argc, argv);
    //testing::Mock::AllowLeak(mock);
    cout << "Starting RDM_MAIN GTEST ===================>" << endl;
    return RUN_ALL_TESTS();
}             
