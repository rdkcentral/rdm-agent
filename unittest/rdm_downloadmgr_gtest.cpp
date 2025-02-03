#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rdmMgr.h"
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_download.h"
#include "rdm_openssl.h"
#include "rdm_downloadutils.h"
#include <system_utils.h>

using ::testing::Return;
using ::testing::_;

// Mocked Function Prototypes
class RdmDownloadMock {
public:
    MOCK_METHOD(int, tarExtract, (const char*, const char*), ());
    MOCK_METHOD(int, findPFile, (const char*, const char*, char*), ());
    MOCK_METHOD(bool, fileCheck, (const char*), ());
    MOCK_METHOD(char*, getExtension, (const char*), ());
    MOCK_METHOD(int, arExtract, (const char*, const char*), ());
    MOCK_METHOD(void, removeFile, (const char*), ());
    MOCK_METHOD(int, rdmDwnlValidation, (RDMAPPDetails*, void*), ());
    MOCK_METHOD(void, rdmIARMEvntSendPayload, (const char*, const char*, const char*, int), ());
    MOCK_METHOD(void, rdmDwnlRunPostScripts, (const char*), ());
};

// Global Mock Instance
RdmDownloadMock* gMock = nullptr;

// Mock Function Implementations
extern "C" {
    int tarExtract(const char* file, const char* path) { return gMock->tarExtract(file, path); }
    int findPFile(const char* dir, const char* pattern, char* out_file) { return gMock->findPFile(dir, pattern, out_file); }
    bool fileCheck(const char* file) { return gMock->fileCheck(file); }
    char* getExtension(const char* file) { return gMock->getExtension(file); }
    int arExtract(const char* file, const char* path) { return gMock->arExtract(file, path); }
    void removeFile(const char* file) { gMock->removeFile(file); }
    int rdmDwnlValidation(RDMAPPDetails* details, void* param) { return gMock->rdmDwnlValidation(details, param); }
    void rdmIARMEvntSendPayload(const char* pkg, const char* ver, const char* home, int error) { gMock->rdmIARMEvntSendPayload(pkg, ver, home, error); }
    void rdmDwnlRunPostScripts(const char* path) { gMock->rdmDwnlRunPostScripts(path); }
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

// Helper Function to Setup RDMAPPDetails
void SetupRdmAppDetails(RDMAPPDetails& appDetails) {
    strcpy(appDetails.app_name, "TestApp");
    strcpy(appDetails.pkg_name, "TestAppPkg");
    strcpy(appDetails.app_dwnl_filepath, "/tmp/TestApp.ipk");
    strcpy(appDetails.app_dwnl_path, "/tmp/downloads");
    strcpy(appDetails.app_home, "/tmp/TestApp");
    strcpy(appDetails.app_mnt, "/mnt/storage");
    appDetails.is_usb = 0;
    appDetails.is_versioned = 0;
}

// **TEST CASES**

// **Test Successful Extraction**
TEST_F(RdmDownloadTest, Test_Extract_Success) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);

    EXPECT_CALL(*gMock, tarExtract(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*gMock, findPFile(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*gMock, fileCheck(_)).WillOnce(Return(true));

    EXPECT_EQ(rdmDwnlExtract(&appDetails), RDM_SUCCESS);
}

// **Test Extraction Failure**
TEST_F(RdmDownloadTest, Test_Extract_Failure) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);

    EXPECT_CALL(*gMock, tarExtract(_, _)).WillOnce(Return(-1));

    EXPECT_NE(rdmDwnlExtract(&appDetails), RDM_SUCCESS);
}

// **Test Download Success with Validation**
TEST_F(RdmDownloadTest, Test_DownloadMgr_Success) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);

    EXPECT_CALL(*gMock, tarExtract(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*gMock, findPFile(_, _, _)).WillOnce(Return(1));
    EXPECT_CALL(*gMock, rdmDwnlValidation(_, _)).WillOnce(Return(RDM_SUCCESS));

    EXPECT_EQ(rdmDownloadMgr(&appDetails), RDM_DL_NOERROR);
}

// **Test Download Failure due to Extraction**
TEST_F(RdmDownloadTest, Test_DownloadMgr_ExtractionFailure) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);

    EXPECT_CALL(*gMock, tarExtract(_, _)).WillRepeatedly(Return(-1));

    EXPECT_EQ(rdmDownloadMgr(&appDetails), RDM_DL_DWNLERR);
}

// **Test Package Already Exists**
TEST_F(RdmDownloadTest, Test_PackageAlreadyExists) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);
    appDetails.dwld_status = 1;

    EXPECT_CALL(*gMock, findPFile(_, _, _)).WillOnce(Return(1));
    EXPECT_CALL(*gMock, tarExtract(_, _)).WillRepeatedly(Return(0));

    EXPECT_EQ(rdmDownloadMgr(&appDetails), RDM_DL_NOERROR);
}

// **Test Signature Validation Failure**
TEST_F(RdmDownloadTest, Test_SignatureValidationFailure) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);

    EXPECT_CALL(*gMock, tarExtract(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*gMock, findPFile(_, _, _)).WillOnce(Return(1));
    EXPECT_CALL(*gMock, rdmDwnlValidation(_, _)).WillOnce(Return(RDM_FAILURE));

    EXPECT_EQ(rdmDownloadMgr(&appDetails), RDM_DL_DWNLERR);
}

// **Main Function**
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
