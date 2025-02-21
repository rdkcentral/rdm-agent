#include <gtest/gtest.h>                                                                                                
#include <gmock/gmock.h>                                                                                                
#include <stdio.h>                                                                                                      
#include <stdlib.h>                                                                                                     
#include <string.h>                                                                                                     
#include <stdarg.h>                                                                                                                                                                                                                            
 extern "C" {                                                                                                            
 #include"unittest/mocks/rdmMgr.h"                                                                                  
 #include "rdm_types.h"                                                                                                  
 #include "rdm.h"                                                                                                        
 #include "rdm_utils.h"                                                                                                  
 #include "rdm_jsonquery.h"                                                                                              
 #include "rdm_downloadutils.h"                                                                                          
 #include "rdm_download.h"                                                                                               
 #include "rdm_openssl.h"                                                                                                
 #include "rdm_downloadutils.h"     
} 

// Mocked Function Prototypes                                                                                           class RdmDownloadMock {                                                                                                 public:
    MOCK_METHOD(int, tarExtract, (const char*, const char*), ());
    MOCK_METHOD(int, findPFile, (const char*, const char*, char*), ());
    MOCK_METHOD(bool, fileCheck, (const char*), ());
    MOCK_METHOD(char*, getExtension, (const char*), ());
    MOCK_METHOD(int, arExtract, (const char*, const char*), ());
    MOCK_METHOD(void, removeFile, (const char*), ());
    MOCK_METHOD(INT32, rdmDwnlValidation, (RDMAPPDetails*, void*), ());
    MOCK_METHOD(INT32, rdmIARMEvntSendPayload, (CHAR*, CHAR*, CHAR*, INT32), ());
    MOCK_METHOD(INT32, rdmDwnlRunPostScripts, (const char*), ());
    MOCK_METHOD(INT32, rdmDwnlCreateFolder, (CHAR*, CHAR*), ());
    MOCK_METHOD(INT32, rdmDwnlApplication, ( CHAR*,  CHAR*,  CHAR*, CHAR*, INT32), ());
    MOCK_METHOD(INT32, copyFiles, (CHAR*, CHAR*), ());
    MOCK_METHOD(INT32, rdmPackageMgrStateChange, (RDMAPPDetails*), ());
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
    INT32 rdmDwnlValidation(RDMAPPDetails* details, char* param) { return gMock->rdmDwnlValidation(details, param); }
    INT32 rdmIARMEvntSendPayload(CHAR* pkg, CHAR* ver, CHAR* home, INT32 event_id) { return gMock->rdmIARMEvntSendPayload(pkg, ver, home, event_id); }
    INT32 rdmDwnlRunPostScripts(CHAR* path) { return gMock->rdmDwnlRunPostScripts(path); }
    INT32 rdmDwnlCreateFolder(CHAR* mnt_path, CHAR* app_name) { return gMock->rdmDwnlCreateFolder(mnt_path, app_name); }    INT32 rdmDwnlApplication(CHAR* app_dwnl_url, CHAR* app_dwnl_path, CHAR* pkg_name, CHAR* app_dwnl_filepath, INT32 is_mtls) { return gMock->rdmDwnlApplication(app_dwnl_url, app_dwnl_path, pkg_name, app_dwnl_filepath, is_mtls); }
    INT32 copyFiles(CHAR* src,CHAR* dest){  return gMock->copyFiles(src, dest); // Call the mock
    }
    INT32 rdmPackageMgrStateChange(RDMAPPDetails* pRdmAppDet) { return gMock->rdmPackageMgrStateChange(pRdmAppDet); }


}

class RdmDownloadTest : public ::testing::Test {
protected:
    void SetUp() override {
        gMock = new RdmDownloadMock();
    }

    void TearDown() override {
        delete gMock;
    }
};


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



TEST_F(RdmDownloadTest, Test_DownloadMgr_Success) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);

    EXPECT_CALL(*gMock, tarExtract(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*gMock, findPFile(_, _, _)).WillOnce(Return(1));
    EXPECT_CALL(*gMock, rdmDwnlValidation(_, _)).WillOnce(Return(RDM_SUCCESS));

    EXPECT_EQ(rdmDownloadMgr(&appDetails), RDM_DL_NOERROR);
}


TEST_F(RdmDownloadTest, Test_PackageAlreadyExists) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);
    appDetails.dwld_status = 1;


    EXPECT_CALL(*gMock, rdmDwnlCreateFolder(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*gMock, rdmDwnlApplication(_, _, _, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*gMock, tarExtract(_,_)).WillOnce(Return(0));
    EXPECT_CALL(*gMock, findPFile(_, _, _)).WillOnce(Return(0)); // Or handle the case where the file is found
    EXPECT_CALL(*gMock, rdmDwnlValidation(_, _)).WillOnce(Return(RDM_FAILURE)); // Replace with the actual error code
    EXPECT_CALL(*gMock, rdmIARMEvntSendPayload(_, _, _, RDM_PKG_VALIDATE_ERROR)).WillOnce(Return(0)); // Expect payload to be sent

    EXPECT_EQ(rdmDownloadMgr(&appDetails), RDM_DL_DWNLERR);
    //EXPECT_CALL(*gMock, findPFile(_, _, _)).WillOnce(Return(1));
    //EXPECT_CALL(*gMock, tarExtract(_, _)).WillRepeatedly(Return(0));

    //EXPECT_EQ(rdmDownloadMgr(&appDetails), RDM_DL_NOERROR);
}

TEST_F(RdmDownloadTest, Test_SignatureValidationFailure) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);
     appDetails.is_oss = false;

    EXPECT_CALL(*gMock, tarExtract(_, _)).WillRepeatedly(Return(0));
      // *** Add EXPECT_CALLs for the mocked functions ***
    EXPECT_CALL(*gMock, rdmDwnlCreateFolder(_, _))  // Expect rdmDwnlCreateFolder to be called
        .WillOnce(Return(0)); // And make it return 0 (success)

    EXPECT_CALL(*gMock, fileCheck(_)) // Expect fileCheck to be called
        .WillOnce(Return(true)); //  Crucially, make it return true!  This simulates the file existing.

    EXPECT_CALL(*gMock, findPFile(_, _, _)).WillOnce(Return(1));
    EXPECT_CALL(*gMock, rdmDwnlValidation(_, _)).WillOnce(Return(RDM_FAILURE));

    EXPECT_EQ(rdmDownloadMgr(&appDetails), RDM_DL_DWNLERR);
}


TEST_F(RdmDownloadTest, PackageAlreadyAvailable_USB_Success) {
    RDMAPPDetails appDetails;
    appDetails.dwld_status = 1;
    appDetails.is_usb = 1; // Set to true for USB download
    // ... Initialize other necessary fields in appDetails ...

    EXPECT_CALL(*gMock, rdmDwnlCreateFolder(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*gMock, copyFiles(_, _)).WillOnce(Return(0)); // Expect copyFiles to succeed
    //EXPECT_CALL(*gMock, rdmDwnlExtract(_)).WillOnce(Return(0));
    EXPECT_CALL(*gMock, findPFile(_, _, _)).WillOnce(Return(0)); // Adjust as needed
    EXPECT_CALL(*gMock, rdmIARMEvntSendPayload(_, _, _, RDM_PKG_INSTALL_COMPLETE)).WillOnce(Return(0));
    EXPECT_CALL(*gMock, rdmDwnlRunPostScripts(_)).WillOnce(Return(0));

    EXPECT_EQ(rdmDownloadMgr(&appDetails), RDM_DL_NOERROR);
}



// **Main Function**
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
