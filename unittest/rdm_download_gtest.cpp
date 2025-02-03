#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_downloadutils.h"
#include "rdm_download.h"
#include "system_utils.h"

using ::testing::Return;
using ::testing::_;

// Mock System Functions
class RdmDownloadMock {
public:
    MOCK_METHOD(bool, checkFileSystem, (const char*), ());
    MOCK_METHOD(int, findPFile, (const char*, const char*, char*), ());
    MOCK_METHOD(int, emptyFolder, (const char*), ());
    MOCK_METHOD(uint32_t, getFreeSpace, (const char*), ());
    MOCK_METHOD(int, createDir, (const char*), ());
    MOCK_METHOD(int, rdmDownloadMgr, (RDMAPPDetails*), ());
    MOCK_METHOD(int, rdmDownloadVerApp, (RDMAPPDetails*), ());
    MOCK_METHOD(void, rdmDwnlUnInstallApp, (const char*, const char*), ());
};

// Global Mock Instance
RdmDownloadMock* gMock = nullptr;

// Fake Implementations for Redirecting Calls
extern "C" {
    bool checkFileSystem(const char* path) { return gMock->checkFileSystem(path); }
    int findPFile(const char* dir, const char* pkg, char* tmp_file) { return gMock->findPFile(dir, pkg, tmp_file); }
    int emptyFolder(const char* path) { return gMock->emptyFolder(path); }
    uint32_t getFreeSpace(const char* path) { return gMock->getFreeSpace(path); }
    int createDir(const char* path) { return gMock->createDir(path); }
    int rdmDownloadMgr(RDMAPPDetails* details) { return gMock->rdmDownloadMgr(details); }
    int rdmDownloadVerApp(RDMAPPDetails* details) { return gMock->rdmDownloadVerApp(details); }
    void rdmDwnlUnInstallApp(const char* path1, const char* path2) { gMock->rdmDwnlUnInstallApp(path1, path2); }
}

// Test Fixture
class RdmDownloadTest : public ::testing::Test {
protected:
    void SetUp() override {
        gMock = new RdmDownloadMock();
    }

    void TearDown() override {
        delete gMock;
    }
};

// Helper Function to Setup a Default `RDMAPPDetails` Structure
void SetupRdmAppDetails(RDMAPPDetails& appDetails) {
    strcpy(appDetails.app_name, "TestApp");
    strcpy(appDetails.pkg_name, "TestAppPkg");
    strcpy(appDetails.app_mnt, "/mnt/storage");
    strcpy(appDetails.app_home, "/mnt/storage/TestApp");
    strcpy(appDetails.app_dwnl_path, "/mnt/storage/rdm/downloads/TestApp");
    strcpy(appDetails.app_dwnl_info, "/opt/persistent/rdmDownloadInfo.txt");
    strcpy(appDetails.pkg_type, "pkg");
    appDetails.app_size_mb = 100;
    appDetails.app_size_kb = 100000;
    appDetails.dwld_status = 0;
    appDetails.bFsCheck = 1;
    appDetails.is_usb = 0;
    appDetails.is_versioned = 0;
}

// **TEST CASES**

// **Test Disk Check Failure**
TEST_F(RdmDownloadTest, Test_DiskCheckFailure) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);
    int dlStatus = RDM_SUCCESS;

    EXPECT_CALL(*gMock, checkFileSystem(_)).WillOnce(Return(false));

    EXPECT_EQ(rdmDownloadApp(&appDetails, &dlStatus), RDM_SUCCESS);
}

// **Test App Already Exists**
TEST_F(RdmDownloadTest, Test_AppAlreadyExists) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);
    int dlStatus = RDM_SUCCESS;

    EXPECT_CALL(*gMock, checkFileSystem(_)).WillOnce(Return(true));
    EXPECT_CALL(*gMock, findPFile(_, _, _)).WillOnce(Return(1));

    EXPECT_EQ(rdmDownloadApp(&appDetails, &dlStatus), RDM_SUCCESS);
    EXPECT_EQ(appDetails.dwld_status, 1);
}

// **Test Not Enough Space**
TEST_F(RdmDownloadTest, Test_NotEnoughSpace) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);
    int dlStatus = RDM_SUCCESS;

    EXPECT_CALL(*gMock, checkFileSystem(_)).WillOnce(Return(true));
    EXPECT_CALL(*gMock, findPFile(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*gMock, emptyFolder(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*gMock, getFreeSpace(_)).WillOnce(Return(50));

    EXPECT_EQ(rdmDownloadApp(&appDetails, &dlStatus), RDM_SUCCESS);
}

// **Test Versioned Download Success**
TEST_F(RdmDownloadTest, Test_VersionedDownloadSuccess) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);
    appDetails.is_versioned = 1;
    int dlStatus = RDM_SUCCESS;

    EXPECT_CALL(*gMock, rdmDownloadVerApp(_)).WillOnce(Return(RDM_SUCCESS));

    EXPECT_EQ(rdmDownloadApp(&appDetails, &dlStatus), RDM_SUCCESS);
}

// **Test Legacy App Download Success**
TEST_F(RdmDownloadTest, Test_LegacyDownloadSuccess) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);
    int dlStatus = RDM_SUCCESS;

    EXPECT_CALL(*gMock, rdmDownloadMgr(_))
        .WillOnce(Return(RDM_DL_NOERROR));

    EXPECT_EQ(rdmDownloadApp(&appDetails, &dlStatus), RDM_SUCCESS);
}

// **Test Legacy App Download with Retry**
TEST_F(RdmDownloadTest, Test_LegacyDownloadRetry) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);
    int dlStatus = RDM_SUCCESS;

    EXPECT_CALL(*gMock, rdmDownloadMgr(_))
        .Times(RDM_RETRY_COUNT - 1)
        .WillRepeatedly(Return(RDM_DL_ERROR));

    EXPECT_CALL(*gMock, rdmDownloadMgr(_))
        .WillOnce(Return(RDM_DL_NOERROR));

    EXPECT_CALL(*gMock, rdmDwnlUnInstallApp(_, _))
        .Times(RDM_RETRY_COUNT - 1);

    EXPECT_EQ(rdmDownloadApp(&appDetails, &dlStatus), RDM_SUCCESS);
}

// **Test Metadata File Update**
TEST_F(RdmDownloadTest, Test_MetadataFileUpdate) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);
    int dlStatus = RDM_SUCCESS;

    EXPECT_CALL(*gMock, createDir(_)).WillOnce(Return(RDM_SUCCESS));
    EXPECT_CALL(*gMock, rdmDownloadMgr(_)).WillOnce(Return(RDM_DL_NOERROR));

    EXPECT_EQ(rdmDownloadApp(&appDetails, &dlStatus), RDM_SUCCESS);
}

// **Main Function**
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
