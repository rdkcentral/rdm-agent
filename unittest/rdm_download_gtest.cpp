#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks/mock_rdm_utils.h"
#include "mocks/mock_rdm_downloadutils.h"
#include "mocks/mock_iarm_bus.h"

// Declare the C functions with extern "C"
extern "C" {
    #include "rdm_download.h"
    #include "rdm_types.h"
    #include "mocks/libIBus.h"
    #include "mocks/rdmMgr.h"
}

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmdownload_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256
using ::testing::_;
using ::testing::Return;
using ::testing::StrEq;


MockRdmUtils* mockRdmUtils;
MockRdmDownloadUtils* mockRdmDownloadUtils;
MockIARM* mockIARM = new MockIARM();
extern "C"{
    INT32 checkFileSystem(const char* path) {
        return mockRdmUtils->checkFileSystem(path);
    }

    INT32 findPFile(const char* path, const char* pkg, char* tmp_file) {
	strcpy(tmp_file, "/media/apps/rdm/downloads/MyPkg/MyPkg_1.0-signed.tar");
        return mockRdmUtils->findPFile(path, pkg, tmp_file);
    }

    INT32 emptyFolder(const char* path) {
        return mockRdmUtils->emptyFolder(path);
    }

    UINT32 getFreeSpace(const char* path) {
        return mockRdmUtils->getFreeSpace(path);
    }

    INT32 createDir(const char* path) {
        return mockRdmUtils->createDir(path);
    }

    int tarExtract(char* filePath, char* appDwnlPath) {
        return mockRdmUtils->tarExtract(filePath, appDwnlPath);
    }

    int fileCheck(char* tmpFile) {
        return mockRdmUtils->fileCheck(tmpFile);
    }

    const char* getExtension(char *filename) {
        return mockRdmUtils->getExtension(filename);
    }

    VOID rdmDwnlUnInstallApp(CHAR* dwnl_path, CHAR* home_path) {
        mockRdmDownloadUtils->rdmDwnlUnInstallApp(dwnl_path, home_path);
    }

    INT32 rdmDownloadVerApp(RDMAPPDetails* appDetails) {
        return mockRdmDownloadUtils->rdmDownloadVerApp(appDetails);
    }
}

class RDMDownloadTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockRdmUtils = new MockRdmUtils();
        mockRdmDownloadUtils = new MockRdmDownloadUtils();
    }

    void TearDown() override {
        delete mockRdmUtils;
        delete mockRdmDownloadUtils;
    }
};

TEST_F(RDMDownloadTest, rdmDownloadApp_Success) {
    RDMAPPDetails appDetails = {};
    strncpy(appDetails.app_name, "test_app", sizeof(appDetails.app_name) - 1);
    strncpy(appDetails.pkg_name, "test_pkg", sizeof(appDetails.pkg_name) - 1);
    strncpy(appDetails.app_mnt, "/mnt/test", sizeof(appDetails.app_mnt) - 1);
    strncpy(appDetails.app_home, "/home/test", sizeof(appDetails.app_home) - 1);
    strncpy(appDetails.app_dwnl_path, "/downloads/test", sizeof(appDetails.app_dwnl_path) - 1);
    strncpy(appDetails.app_dwnl_info, "/downloads/test/rdmDownloadInfo.txt", sizeof(appDetails.app_dwnl_info) - 1);
    appDetails.app_size_mb = 100;
    appDetails.bFsCheck = true;
    appDetails.is_versioned = false;
    appDetails.is_usb = 0;

    INT32 DLStatus = RDM_DL_NOERROR;

    EXPECT_CALL(*mockRdmUtils, checkFileSystem(StrEq(appDetails.app_mnt)))
        .WillOnce(Return(1));
    EXPECT_CALL(*mockRdmUtils, findPFile(StrEq(appDetails.app_mnt), StrEq(appDetails.pkg_name), _))
        .WillOnce(Return(0));
    EXPECT_CALL(*mockRdmUtils, emptyFolder(StrEq(appDetails.app_dwnl_path)))
        .WillOnce(Return(0));
    EXPECT_CALL(*mockRdmUtils, emptyFolder(StrEq(appDetails.app_home)))
        .WillOnce(Return(0));
    EXPECT_CALL(*mockRdmUtils, getFreeSpace(StrEq(appDetails.app_mnt)))
        .WillOnce(Return(200));
    EXPECT_CALL(*mockRdmUtils, createDir(_))
        .WillOnce(Return(RDM_SUCCESS));

    EXPECT_EQ(rdmDownloadApp(&appDetails, &DLStatus), RDM_SUCCESS);
    EXPECT_EQ(DLStatus, RDM_DL_NOERROR);
}
                    
TEST_F(RDMDownloadTest, rdmDownloadApp_Failure) {
    RDMAPPDetails appDetails = {};
    strncpy(appDetails.app_name, "test_app", sizeof(appDetails.app_name) - 1);
    strncpy(appDetails.pkg_name, "test_pkg", sizeof(appDetails.pkg_name) - 1);
    strncpy(appDetails.app_mnt, "/mnt/test", sizeof(appDetails.app_mnt) - 1);
    strncpy(appDetails.app_home, "/home/test", sizeof(appDetails.app_home) - 1);
    strncpy(appDetails.app_dwnl_path, "/downloads/test", sizeof(appDetails.app_dwnl_path) - 1);
    strncpy(appDetails.app_dwnl_info, "/downloads/test/rdmDownloadInfo.txt", sizeof(appDetails.app_dwnl_info) - 1);
    appDetails.app_size_mb = 100;
    appDetails.bFsCheck = true;
    appDetails.is_versioned = false;
    appDetails.is_usb = 0;
     appDetails.app_size_kb = 100;

    INT32 DLStatus = RDM_DL_NOERROR;

      EXPECT_CALL(*mockRdmUtils, checkFileSystem(StrEq(appDetails.app_mnt)))
        .WillOnce(Return(1));
     EXPECT_CALL(*mockRdmUtils, findPFile(::testing::_,::testing::_,::testing::_))
        .WillOnce(Return(0));
    EXPECT_CALL(*mockRdmUtils, emptyFolder(::testing::_))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*mockRdmUtils, getFreeSpace(::testing::_))
        .WillOnce(Return(appDetails.app_size_mb + 1));

    EXPECT_CALL(*mockRdmUtils, createDir(StrEq("/downloads/test")))
        .WillOnce(Return(RDM_FAILURE));

    EXPECT_EQ(rdmDownloadApp(&appDetails, &DLStatus), RDM_SUCCESS);
}

// Test rdmDwnlExtract
TEST_F(RDMDownloadTest, rdmDwnlExtract_Success) {
    RDMAPPDetails appDetails = {};
    strncpy(appDetails.app_name, "test_app", sizeof(appDetails.app_name) - 1);
    strncpy(appDetails.pkg_name, "test_pkg", sizeof(appDetails.pkg_name) - 1);
    strncpy(appDetails.app_mnt, "/mnt/test", sizeof(appDetails.app_mnt) - 1);
    strncpy(appDetails.app_home, "/home/test", sizeof(appDetails.app_home) - 1);
    strncpy(appDetails.app_dwnl_path, "/downloads/test", sizeof(appDetails.app_dwnl_path) - 1);
    strncpy(appDetails.app_dwnl_info, "/downloads/test/rdmDownloadInfo.txt", sizeof(appDetails.app_dwnl_info) - 1);
    appDetails.app_size_mb = 100;
    appDetails.bFsCheck = true;
    appDetails.is_versioned = true;
    appDetails.is_usb = 0;
    appDetails.app_size_kb = 100;

     EXPECT_CALL(*mockRdmUtils, tarExtract(::testing::_,::testing::_))
        .WillRepeatedly(Return(0));

     EXPECT_CALL(*mockRdmUtils, findPFile(::testing::_,::testing::_,::testing::_))
        .WillRepeatedly(Return(1));

     EXPECT_CALL(*mockRdmUtils, fileCheck(::testing::_))
        .WillRepeatedly(Return(1));

    EXPECT_CALL(*mockIARM, IARM_Bus_Init("AppDownloadEvent"))
        .WillRepeatedly(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_Connect())
        .WillRepeatedly(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_BroadcastEvent(IARM_BUS_RDMMGR_NAME, IARM_BUS_RDMMGR_EVENT_APP_INSTALLATION_STATUS, ::testing::_, sizeof(IARM_Bus_RDMMgr_EventData_t)))
        .WillRepeatedly(::testing::Return(IARM_RESULT_SUCCESS));

    EXPECT_EQ(rdmDwnlExtract(&appDetails), RDM_SUCCESS);
    delete mockIARM;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
                                  
                                                 					
