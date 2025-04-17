#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks/mock_rdm_utils.h"
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

MockRdmUtils* mockRdmUtils = new MockRdmUtils();
//char *ext = new char[3];
//strcpy(ext, "sh");
extern char* retStr;

extern "C"{
    void* doCurlInit() {
        return mockRdmUtils->doCurlInit();
    }

    INT32 doHttpFileDownload(void *in_curl, FileDwnl_t *pfile_dwnl, MtlsAuth_t *auth, unsigned int max_dwnl_speed, char *dnl_start_pos, int *out_httpCode) {
        return mockRdmUtils->doHttpFileDownload(in_curl, pfile_dwnl, auth, max_dwnl_speed, dnl_start_pos, out_httpCode);
    }

    void doStopDownload(void* curl) {
        return mockRdmUtils->doStopDownload(curl);
    }

    void copyCommandOutput(char *filePath, char *fileOut, int len) {
        return mockRdmUtils->copyCommandOutput(filePath, fileOut, len);
    }

    INT32 findPFile(const char* path, const char* pkg, char* tmp_file) {
        return mockRdmUtils->findPFile(path, pkg, tmp_file);
    }

    char* getExtension(char* filename) {
	    char *extension;

    if(filename == NULL) {
        return NULL;
    }

    extension = strrchr(filename, '.');

    if (extension) {
        return (extension + 1);
    }

    return NULL;
    }
}

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

    void* mockReturnValue = static_cast<void*>(new int(0)); 
    EXPECT_CALL(*mockRdmUtils, doCurlInit())
        .WillOnce(Return(mockReturnValue));
 
    EXPECT_CALL(*mockRdmUtils, doStopDownload(::testing::_));

    EXPECT_EQ(rdmDwnlDirect(pUrl, pDwnlPath, pPkgName, pOut, isMtls), RDM_SUCCESS);

    delete static_cast<int*>(mockReturnValue);
    //delete mockRdmUtils;

}

// Test rdmDwnlGetCert
TEST(rdmDwnlGetCert, rdmDwnlGetCert_Success) {
    MtlsAuth_t sec = { .cert_name = "MyCert.pem", .cert_type = "Type1", .key_pas = "MyKeyVal" };
    EXPECT_EQ(rdmDwnlGetCert(&sec), RDM_SUCCESS);
}

// Test rdmDwnlRunPostScripts
TEST(rdmDwnlRunPostScripts, rdmDwnlRunPostScripts_Success) {
    char pAppHome[32] = "/media/apps";
    //static char ext[3] = "sh";
    system("mkdir -p /media/apps/etc/rdm/post-services/");
    system("touch /media/apps/etc/rdm/post-services/post-install.sh");
    //void* mockReturnValue = static_cast<void*>(new char[3]);
    //strcpy(static_cast<char*>(mockReturnValue), "sh");
    retStr = (char*)malloc(3 * sizeof(char)); // Allocate memory for the string
    if (retStr != nullptr)
        strcpy(retStr, "sh"); // Copy the value "sh" into retStr

    EXPECT_EQ(rdmDwnlRunPostScripts(pAppHome), RDM_SUCCESS);

    //delete[] ext;
}

TEST(rdmDwnlRunPostScripts, rdmDwnlRunPostScripts_Failure) {
    char pAppHome[32] = "/tmp/some_path";
    EXPECT_EQ(rdmDwnlRunPostScripts(pAppHome), RDM_FAILURE);
}

// Test rdmUnInstallApps
TEST(rdmUnInstallApps, rdmUnInstallApps_Success) {
    int isBroadband = 0;
    EXPECT_EQ(rdmUnInstallApps(isBroadband), RDM_SUCCESS);
}

// Test rdmDwnlValidation
/*
TEST(rdmDwnlValidation, rdmDwnlValidation_SUccess) {
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

    EXPECT_EQ(rdmDwnlValidation(&appDetails, NULL), RDM_SUCCESS);
}*/
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
                                  
                                                 					
