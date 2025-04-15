#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks/mock_rdm_utils.h"
#include "mocks/mock_rdm_downloadutils.h"
#include "mocks/system_utils.h"

// Declare the C functions with extern "C"
extern "C" {
    #include "rdm_download.h"
    #include "rdm_types.h"
    #include "rdm_downloadutils.h"
}

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmdownload_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256
using ::testing::_;
using ::testing::Return;
using ::testing::StrEq;


MockRdmUtils* mockRdmUtils;
MockRdmDownloadUtils* mockRdmDownloadUtils;
extern "C" {
    INT32 checkFileSystem(const char* path) {
        return mockRdmUtils->checkFileSystem(path);
    }

    INT32 findPFile(const char* path, const char* pkg, char* tmp_file) {
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

    void* doCurlInit() {
        return mockRdmUtils->doCurlInit();
    }

    INT32 doHttpFileDownload(void *in_curl, FileDwnl_t *pfile_dwnl, MtlsAuth_t *auth, unsigned int max_dwnl_speed, char *dnl_start_pos, int *out_httpCode) {
        return mockRdmUtils->doHttpFileDownload(in_curl, pfile_dwnl, auth, max_dwnl_speed, dnl_start_pos, out_httpCode);
    }

    void doStopDownload(void* curl) {
        return mockRdmUtils->doStopDownload(curl);
    }

    INT32 rdmDownloadMgr(RDMAPPDetails* appDetails) {
        return mockRdmDownloadUtils->rdmDownloadMgr(appDetails);
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

/*TEST_F(RDMDownloadTest, rdmDownloadApp_Success) {
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
    EXPECT_CALL(*mockRdmDownloadUtils, rdmDownloadMgr(&appDetails))
        .WillOnce(Return(RDM_DL_NOERROR));

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

    EXPECT_CALL(*mockRdmDownloadUtils, rdmDownloadMgr(&appDetails))
        .Times(RDM_RETRY_COUNT)
        .WillRepeatedly(Return(RDM_DL_DWNLERR));

    EXPECT_CALL(*mockRdmUtils, createDir(StrEq("/downloads/test")))
        .WillOnce(Return(RDM_FAILURE));

 EXPECT_CALL(*mockRdmDownloadUtils, rdmDwnlUnInstallApp(::testing::_, ::testing::_))
        .Times(2);

    EXPECT_EQ(rdmDownloadApp(&appDetails, &DLStatus), RDM_SUCCESS);
}*/

// Test rdmDwnlDirect
TEST(rdmDwnlDirect, rdmDwnlDirect_Success) {
    char pUrl[128];
    char pDwnlPath[64];
    char pPkgName[64];
    char pOut[64];
    strncpy(pUrl, "http://example.com", sizeof(pUrl) - 1);
    strncpy(pDwnlPath, "/media/apps", sizeof(pDwnlPath) - 1);
    strncpy(pPkgName, "MyPackage", sizeof(pPkgName) - 1);
    strncpy(pOut, "/etc", sizeof(pOut) - 1);
    INT32 isMtls = 0;
    EXPECT_EQ(rdmDwnlDirect(pUrl, pDwnlPath, pPkgName, pOut, isMtls), RDM_SUCCESS);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
                                  
                                                 					
