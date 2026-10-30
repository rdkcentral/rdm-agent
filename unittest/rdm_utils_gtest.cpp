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
#include "unittest/mocks/mock_iarm_bus.h"
extern "C" {
#include "unittest/mocks/libIBus.h"
#include "unittest/mocks/rdmMgr.h"
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_download.h"
#include <dirent.h>
#include <string.h>
#include <cstdlib>
}

// Mock IARM functions for isolated testing


#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdm_utils_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256

using namespace testing;
using ::testing::_;
using ::testing::Return;
using ::testing::StrEq;
using namespace std;

MockIARM* mockIARM = nullptr;

class RdmTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockIARM = new MockIARM();
    }

    void TearDown() override {
        delete mockIARM;
        mockIARM = nullptr;
    }
};

// Test rdmIARMEvntSendStatus
TEST_F(RdmTest, rdmIARMEvntSendStatus_Success) {
    EXPECT_CALL(*mockIARM, IARM_Bus_Init(StrEq("AppDownloadEvent")))
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_Connect())
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_BroadcastEvent(StrEq(IARM_BUS_RDMMGR_NAME), IARM_BUS_RDMMGR_EVENT_APPDOWNLOADS_CHANGED, ::testing::_, sizeof(UINT8)))
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_Disconnect());
    EXPECT_CALL(*mockIARM, IARM_Bus_Term());

    EXPECT_EQ(rdmIARMEvntSendStatus(1), RDM_SUCCESS);
}

TEST_F(RdmTest, rdmIARMEvntSendStatus_InitFailure) {
    EXPECT_CALL(*mockIARM, IARM_Bus_Init(StrEq("AppDownloadEvent")))
        .WillOnce(::testing::Return(IARM_RESULT_IPCCORE_FAIL));

    EXPECT_EQ(rdmIARMEvntSendStatus(1), RDM_FAILURE);
}

TEST_F(RdmTest, rdmIARMEvntSendStatus_ConnectFailure) {
    EXPECT_CALL(*mockIARM, IARM_Bus_Init(StrEq("AppDownloadEvent")))
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_Connect())
        .WillOnce(::testing::Return(IARM_RESULT_IPCCORE_FAIL));
    EXPECT_CALL(*mockIARM, IARM_Bus_Term());

    EXPECT_EQ(rdmIARMEvntSendStatus(1), RDM_FAILURE);
}
          // Test rdmIARMEvntSendPayload
TEST_F(RdmTest, rdmIARMEvntSendPayload_Success) {
    EXPECT_CALL(*mockIARM, IARM_Bus_Init(StrEq("AppDownloadEvent")))
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_Connect())
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_BroadcastEvent(StrEq(IARM_BUS_RDMMGR_NAME), IARM_BUS_RDMMGR_EVENT_APP_INSTALLATION_STATUS, ::testing::_, sizeof(IARM_Bus_RDMMgr_EventData_t)))
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_Disconnect());
    EXPECT_CALL(*mockIARM, IARM_Bus_Term());

    EXPECT_EQ(rdmIARMEvntSendPayload((CHAR*)"test_pkg", (CHAR*)"1.0", (CHAR*)"/path/to/pkg", 1), RDM_SUCCESS);
}

TEST_F(RdmTest, rdmIARMEvntSendPayload_InitFailure) {
    EXPECT_CALL(*mockIARM, IARM_Bus_Init(StrEq("AppDownloadEvent")))
        .WillOnce(::testing::Return(IARM_RESULT_IPCCORE_FAIL));

    EXPECT_EQ(rdmIARMEvntSendPayload((CHAR*)"test_pkg", (CHAR*)"1.0", (CHAR*)"/path/to/pkg", 1), RDM_FAILURE);
}
// Test rdmListDirectory
TEST(RdmDirectoryTest, rdmListDirectory_Success) {
    CHAR* dirList[10];
    INT32 numDirs = 0;

    // Create a temporary directory for testing
    system("mkdir -p /tmp/test_rdm_dir && touch /tmp/test_rdm_dir/file1 /tmp/test_rdm_dir/file2");

    EXPECT_EQ(rdmListDirectory((CHAR*)"/tmp/test_rdm_dir", dirList, &numDirs, 10), RDM_SUCCESS);
    EXPECT_EQ(numDirs, 4);

    for (int i = 0; i < numDirs; i++) {
        free(dirList[i]);
    }

    system("rm -rf /tmp/test_rdm_dir");
}

TEST(RdmDirectoryTest, rdmListDirectory_Failure) {
    CHAR* dirList[10];
    INT32 numDirs = 0;

    EXPECT_EQ(rdmListDirectory((CHAR*)"/nonexistent", dirList, &numDirs, 10), RDM_FAILURE);
}

TEST(RdmDirectoryTest, rdmListDirectory_NullArgument) {
    CHAR* dirList[10];
    INT32 numDirs = 0;

    EXPECT_EQ(rdmListDirectory(nullptr, dirList, &numDirs, 10), RDM_FAILURE);
}

TEST(RdmDirectoryTest, rdmListDirectory_StopsAtMaxEntries) {
    CHAR* sentinel = (CHAR*)"sentinel";
    CHAR* dirList[4] = { nullptr, nullptr, nullptr, sentinel };
    INT32 numDirs = 0;

    // 5 files plus "." and ".." is more than the 3 slots allowed
    system("mkdir -p /tmp/test_rdm_dir_max && touch /tmp/test_rdm_dir_max/f1 /tmp/test_rdm_dir_max/f2 /tmp/test_rdm_dir_max/f3 /tmp/test_rdm_dir_max/f4 /tmp/test_rdm_dir_max/f5");

    EXPECT_EQ(rdmListDirectory((CHAR*)"/tmp/test_rdm_dir_max", dirList, &numDirs, 3), RDM_FAILURE);
    EXPECT_EQ(numDirs, 3);
    EXPECT_EQ(dirList[3], sentinel);

    for (int i = 0; i < numDirs; i++) {
        free(dirList[i]);
    }

    system("rm -rf /tmp/test_rdm_dir_max");
}

TEST(RdmDirectoryTest, rdmListDirectory_SkipsNamesTooLong) {
    CHAR* dirList[10];
    INT32 numDirs = 0;
    std::string longName(200, 'a');

    system(("mkdir -p /tmp/test_rdm_dir_long && touch /tmp/test_rdm_dir_long/app1 /tmp/test_rdm_dir_long/" + longName).c_str());

    EXPECT_EQ(rdmListDirectory((CHAR*)"/tmp/test_rdm_dir_long", dirList, &numDirs, 10), RDM_SUCCESS);
    EXPECT_EQ(numDirs, 3);

    for (int i = 0; i < numDirs; i++) {
        EXPECT_LT(strlen(dirList[i]), (size_t)RDM_APPNAME_LEN);
        free(dirList[i]);
    }

    system("rm -rf /tmp/test_rdm_dir_long");
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
    cout << "Starting RDM_UTILS GTEST ===================>" << endl;
    return RUN_ALL_TESTS();
}

                 
