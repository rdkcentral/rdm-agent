#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks/mock_rdm_utils.h"
#include "mocks/mock_rdm_dwnlutils.h"
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

    void* mockReturnValue = static_cast<void*>(new int(42)); 
    EXPECT_CALL(*mockRdmUtils, doCurlInit())
        .WillOnce(Return(mockReturnValue));

    EXPECT_CALL(*mockRdmUtils, doHttpFileDownload((::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)))
        .WillOnce(Return(0));
    
    EXPECT_CALL(*mockRdmUtils, doStopDownload((::testing::_)));

    EXPECT_EQ(rdmDwnlDirect(pUrl, pDwnlPath, pPkgName, pOut, isMtls), RDM_SUCCESS);

    delete static_cast<int*>(mockReturnValue);
    delete mockRdmUtils2;

}

// Test rdmDwnlGetCert
TEST(rdmDwnlGetCert, rdmDwnlGetCert_Success) {
    MtlsAuth_t sec = { .cert_name = "MyCert.pem", .cert_type = "Type1", .key_pas = "MyKeyVal" };
    EXPECT_EQ(rdmDwnlGetCert(&sec), RDM_SUCCESS);
}

// Test rdmDwnlRunPostScripts
TEST(rdmDwnlRunPostScripts, rdmDwnlRunPostScripts_Success) {
    char pAppHomw[32] = "/media/apps";
    system("mkdir -p /media/apps/etc/rdm/post-services/");
    system("touch /media/apps/etc/rdm/post-services/post-install.sh");
    EXPECT_EQ(rdmDwnlRunPostScripts(pAppHome), RDM_SUCCESS);
}

TEST(rdmDwnlRunPostScripts, rdmDwnlRunPostScripts_Failure) {
    char pAppHomw[32] = "/media/apps";
    EXPECT_EQ(rdmDwnlRunPostScripts(pAppHome), RDM_FAILURE);
}

// Test rdmUnInstallApps
TEST(rdmUnInstallApps, rdmUnInstallApps_Success) {
    int isBroadband = 0;
    EXPECT_EQ(rdmUnInstallApps(isBroadband), RDM_SUCCESS);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
                                  
                                                 					
