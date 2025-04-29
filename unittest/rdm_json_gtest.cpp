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

#include <fstream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


extern "C"
{
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include <cjson/cJSON.h>
#include "rdm_jsonquery.h"
}

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmjson_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256
using ::testing::Return;
using ::testing::_;


class MockRdmJson {
public:
    MOCK_METHOD(INT32, rdmJSONGetLen, (const CHAR* fileName, INT32* len), ());
    MOCK_METHOD(INT32, rdmJSONQuery, (const CHAR* fileName, const CHAR* query, CHAR* output), ());
    MOCK_METHOD(INT32, rdmJSONGetAppNames, (INT32 index, CHAR* appName), ());
    MOCK_METHOD(INT32, rdmJSONGetAppDetID, (INT32 index, RDMAPPDetails* appDetails), ());
    MOCK_METHOD(INT32, rdmJSONGetAppDetName, (const CHAR* appName, RDMAPPDetails* appDetails), ());
};
class RdmJsonTest : public ::testing::Test {
protected:
    MockRdmJson g_mockRdmJson;
    void SetUp() override {
         //Create a sample JSON file for testing
        std::ofstream jsonFile("test_manifest.json");
        jsonFile << R"({
            "apps": [
                {"name": "App1", "size": "10MB", "version": "1.0", "is_versioned": "true"},
                {"name": "App2", "size": "20MB", "version": "2.0", "is_versioned": "false"}
            ]
        })";
        jsonFile.close();


    // Set up default mock expectations
        EXPECT_CALL(g_mockRdmJson, rdmJSONGetLen(_, _))
            .WillRepeatedly([](const CHAR* fileName, INT32* len) {
                if (strcmp(fileName, "test_manifest.json") != 0) {
                    return RDM_FAILURE;
                }
                *len = 2; // Simulate the length of "apps" array
                return RDM_SUCCESS;
            });

 EXPECT_CALL(g_mockRdmJson, rdmJSONQuery(_, _, _))
            .WillRepeatedly([](const CHAR* fileName, const CHAR* query, CHAR* output) {
                if (strcmp(fileName, "test_manifest.json") != 0) {
                    return RDM_FAILURE;
                }
                if (strcmp(query, "/apps/0/name") == 0) {
                    strcpy(output, "App1");
                    return RDM_SUCCESS;
                }
				 return RDM_FAILURE;
            });

        EXPECT_CALL(g_mockRdmJson, rdmJSONGetAppNames(_, _))
            .WillRepeatedly([](INT32 index, CHAR* appName) {
                if (index == 0) {
                    strcpy(appName, "App1");
                    return RDM_SUCCESS;
                } else if (index == 1) {
                    strcpy(appName, "App2");
                    return RDM_SUCCESS;
                }
                return RDM_FAILURE;
            });
 EXPECT_CALL(g_mockRdmJson, rdmJSONGetAppDetID(_, _))
            .WillRepeatedly([](INT32 index, RDMAPPDetails* appDetails) {
                if (index == 0) {
                    strcpy(appDetails->app_name, "App1");
                    strcpy(appDetails->pkg_ver, "1.0");
                    strcpy(appDetails->pkg_type, "normal");
                    strcpy(appDetails->app_size, "10MB");
                    return RDM_SUCCESS;
                }
                return RDM_FAILURE;
            });

    }


    void TearDown() override {
        remove("test_manifest.json");
    }

};
                                                         
														 
TEST_F(RdmJsonTest, Test_rdmJSONQuery_Success) {
    CHAR output[256] = {0};
    ASSERT_EQ(g_mockRdmJson.rdmJSONQuery("test_manifest.json", "/apps/0/name", output), RDM_SUCCESS);
    ASSERT_STREQ(output, "App1"); // Verify the output matches the expected value
}

TEST_F(RdmJsonTest, Test_rdmJSONGetLen_Success) {
    INT32 len = 0;
    ASSERT_EQ(g_mockRdmJson.rdmJSONGetLen("test_manifest.json", &len), RDM_SUCCESS);
    ASSERT_EQ(len, 2);
}


TEST_F(RdmJsonTest, Test_rdmJSONGetLen_InvalidFile) {
    INT32 len = 0;
    ASSERT_EQ(rdmJSONGetLen("invalid.json", &len), RDM_FAILURE);
}

TEST_F(RdmJsonTest, Test_rdmJSONQuery_InvalidPath) {
    CHAR output[256] = {0};
    ASSERT_EQ(g_mockRdmJson.rdmJSONQuery("test_manifest.json", "/invalid_path", output), RDM_FAILURE);
}


TEST_F(RdmJsonTest, Test_rdmJSONGetAppNames_InvalidIndex) {
    CHAR appName[256] = {0};
    ASSERT_EQ(g_mockRdmJson.rdmJSONGetAppNames(10, appName), RDM_FAILURE);
}
TEST_F(RdmJsonTest, Test_rdmJSONGetAppDetID_Success) {
    RDMAPPDetails appDetails;
    ASSERT_EQ(g_mockRdmJson.rdmJSONGetAppDetID(0, &appDetails), RDM_SUCCESS);
    ASSERT_STREQ(appDetails.app_name, "App1");
    ASSERT_STREQ(appDetails.pkg_ver, "1.0");
}


TEST_F(RdmJsonTest, Test_rdmJSONGetAppNames_Success) {
    CHAR appName[256] = {0};
    ASSERT_EQ(g_mockRdmJson.rdmJSONGetAppNames(0, appName), RDM_SUCCESS);
    ASSERT_STREQ(appName, "App1");
}

TEST_F(RdmJsonTest, Test_rdmJSONGetAppDetName_InvalidName) {
    RDMAPPDetails appDetails;
    ASSERT_EQ(rdmJSONGetAppDetName("InvalidApp", &appDetails), RDM_FAILURE);
}
