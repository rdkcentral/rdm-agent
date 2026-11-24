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
#include <fstream>
#include <string>
#include <unistd.h>
#include "unittest/mocks/mock_iarm_bus.h"
#include "unittest/mocks/mock_rdm_utils.h"
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
using ::testing::DoAll;
using ::testing::SetArgPointee;
using namespace std;

MockIARM* mockIARM = nullptr;
extern MockRdmUtils* mockRdmUtils;

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

class RdmCleanupTest : public ::testing::Test {
protected:
    char root[64] = "/tmp/rdm-cleanup-XXXXXX";

    void SetUp() override {
        ASSERT_NE(mkdtemp(root), nullptr);
    }

    void TearDown() override {
        EXPECT_EQ(system(("rm -rf -- \"" + std::string(root) + "\"").c_str()), 0);
    }

    std::string path(const char *name) {
        return std::string(root) + "/" + name;
    }

    void createFile(const std::string &name) {
        std::ofstream file(name);
        ASSERT_TRUE(file.good());
        file << "keep";
    }
};

TEST_F(RdmCleanupTest, PackageSearchAppendsAndRespectsCountLimit) {
    ASSERT_EQ(mkdir(path("nested").c_str(), 0700), 0);
    createFile(path("nested/app-signed.tar"));
    createFile(path("nested/ignored.txt"));
    CHAR existing[RDM_APP_PATH_LEN] = "existing";
    CHAR result[RDM_APP_PATH_LEN] = {0};
    CHAR *out[] = {existing, result};
    INT32 count = 1;

    EXPECT_EQ(rdmFindPackageFiles(root, "*-signed.tar", out, sizeof(result), &count, 2),
              RDM_SUCCESS);
    EXPECT_EQ(count, 2);
    EXPECT_STREQ(existing, "existing");
    EXPECT_STREQ(result, path("nested/app-signed.tar").c_str());
    EXPECT_EQ(rdmFindPackageFiles(root, "*-signed.tar", out, sizeof(result), &count, 2),
              RDM_SUCCESS);
    EXPECT_EQ(count, 2);
}

TEST_F(RdmCleanupTest, PackageSearchRejectsTruncationWithoutWritingOutput) {
    createFile(path("app-signed.tar"));
    CHAR result[8] = "keep";
    CHAR *out[] = {result};
    INT32 count = 0;

    EXPECT_EQ(rdmFindPackageFiles(root, "*-signed.tar", out, sizeof(result), &count, 1),
              RDM_FAILURE);
    EXPECT_EQ(count, 0);
    EXPECT_STREQ(result, "keep");
    count = 2;
    EXPECT_EQ(rdmFindPackageFiles(root, "*", out, sizeof(result), &count, 1), RDM_FAILURE);
}

TEST_F(RdmCleanupTest, PackageSearchHandlesFullPathBoundary) {
    CHAR result[RDM_APP_PATH_LEN] = {0};
    CHAR *out[] = {result};
    INT32 count = 0;
    std::string name(RDM_APP_PATH_LEN - strlen(root) - 2, 'a');
    createFile(path(name.c_str()));

    EXPECT_EQ(rdmFindPackageFiles(root, "*", out, sizeof(result), &count, 1), RDM_SUCCESS);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(strlen(result), sizeof(result) - 1);
    EXPECT_EQ(unlink(path(name.c_str()).c_str()), 0);
    name += "b";
    createFile(path(name.c_str()));
    count = 0;
    strcpy(result, "keep");
    EXPECT_EQ(rdmFindPackageFiles(root, "*", out, sizeof(result), &count, 1), RDM_FAILURE);
    EXPECT_EQ(count, 0);
    EXPECT_STREQ(result, "keep");
}

class RdmDwnlDirectTest : public ::testing::Test {
protected:
    int dummyCurl = 0;
    char out[512];

    void SetUp() override {
        mockRdmUtils = new MockRdmUtils();
        memset(out, 0, sizeof(out));
    }

    void TearDown() override {
        delete mockRdmUtils;
        mockRdmUtils = nullptr;
    }
};

TEST_F(RdmDwnlDirectTest, Success) {
    EXPECT_CALL(*mockRdmUtils, rdmDwnlIsOCSPEnable()).WillOnce(Return(0));
    EXPECT_CALL(*mockRdmUtils, doCurlInit()).WillOnce(Return(&dummyCurl));
    EXPECT_CALL(*mockRdmUtils, doHttpFileDownload(&dummyCurl, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<5>(200), Return(0)));
    EXPECT_CALL(*mockRdmUtils, doStopDownload(&dummyCurl));

    EXPECT_EQ(rdmDwnlDirect((CHAR*)"http://host/pkg.tar", (CHAR*)"/tmp/dl", (CHAR*)"pkg.tar", out), RDM_SUCCESS);
    EXPECT_STREQ(out, "/tmp/dl/pkg.tar");
}

TEST_F(RdmDwnlDirectTest, OcspEnabled) {
    EXPECT_CALL(*mockRdmUtils, rdmDwnlIsOCSPEnable()).WillOnce(Return(1));
    EXPECT_CALL(*mockRdmUtils, doCurlInit()).WillOnce(Return(&dummyCurl));
    EXPECT_CALL(*mockRdmUtils, doHttpFileDownload(&dummyCurl, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<5>(200), Return(0)));
    EXPECT_CALL(*mockRdmUtils, doStopDownload(&dummyCurl));

    EXPECT_EQ(rdmDwnlDirect((CHAR*)"https://host/pkg.tar", (CHAR*)"/tmp/dl", (CHAR*)"pkg.tar", out), RDM_SUCCESS);
}

TEST_F(RdmDwnlDirectTest, CurlInitFailure) {
    EXPECT_CALL(*mockRdmUtils, rdmDwnlIsOCSPEnable()).WillOnce(Return(0));
    EXPECT_CALL(*mockRdmUtils, doCurlInit()).WillOnce(Return(nullptr));
    EXPECT_CALL(*mockRdmUtils, doHttpFileDownload(_, _, _, _, _, _)).Times(0);
    EXPECT_CALL(*mockRdmUtils, doStopDownload(_)).Times(0);

    // Current implementation returns the initial status (RDM_SUCCESS) on curl init failure.
    EXPECT_EQ(rdmDwnlDirect((CHAR*)"http://host/pkg.tar", (CHAR*)"/tmp/dl", (CHAR*)"pkg.tar", out), RDM_SUCCESS);
}

TEST_F(RdmDwnlDirectTest, DownloadFailure) {
    EXPECT_CALL(*mockRdmUtils, rdmDwnlIsOCSPEnable()).WillOnce(Return(0));
    EXPECT_CALL(*mockRdmUtils, doCurlInit()).WillOnce(Return(&dummyCurl));
    EXPECT_CALL(*mockRdmUtils, doHttpFileDownload(&dummyCurl, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<5>(404), Return(-1)));
    EXPECT_CALL(*mockRdmUtils, doStopDownload(&dummyCurl));

    EXPECT_EQ(rdmDwnlDirect((CHAR*)"http://host/pkg.tar", (CHAR*)"/tmp/dl", (CHAR*)"pkg.tar", out), RDM_FAILURE);
}

TEST_F(RdmDwnlDirectTest, CurlErrorWithHttp200) {
    EXPECT_CALL(*mockRdmUtils, rdmDwnlIsOCSPEnable()).WillOnce(Return(0));
    EXPECT_CALL(*mockRdmUtils, doCurlInit()).WillOnce(Return(&dummyCurl));
    EXPECT_CALL(*mockRdmUtils, doHttpFileDownload(&dummyCurl, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<5>(200), Return(-1)));
    EXPECT_CALL(*mockRdmUtils, doStopDownload(&dummyCurl));

    EXPECT_EQ(rdmDwnlDirect((CHAR*)"http://host/pkg.tar", (CHAR*)"/tmp/dl", (CHAR*)"pkg.tar", out), RDM_SUCCESS);
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


