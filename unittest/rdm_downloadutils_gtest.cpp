#include <gtest/gtest.h>
#include <gmock/gmock.h>
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
    MOCK_METHOD(int, fileCheck, (const char*), ());
    MOCK_METHOD(int, copyFiles, (const char*, const char*), ());
    MOCK_METHOD(int, folderCheck, (const char*), ());
    MOCK_METHOD(int, createDir, (const char*), ());
    MOCK_METHOD(int, removeFile, (const char*), ());
    MOCK_METHOD(int, emptyFolder, (const char*), ());
    MOCK_METHOD(int, getFileLastModifyTime, (const char*), ());
    MOCK_METHOD(int, getCurrentSysTimeSec, (), ());
    MOCK_METHOD(void*, doCurlInit, (), ());
    MOCK_METHOD(int, doHttpFileDownload, (void*, FileDwnl_t*, MtlsAuth_t*, int, void*, int*), ());
    MOCK_METHOD(void, doStopDownload, (void*), ());
    MOCK_METHOD(void, rdmIARMEvntSendPayload, (const char*, const char*, const char*, int), ());
    MOCK_METHOD(JSON*, ParseJsonStr, (char*), ());
    MOCK_METHOD(void, GetJsonVal, (JSON*, const char*, char*, unsigned int), ());
    MOCK_METHOD(void, FreeJson, (JSON*), ());
};

// Global Mock Instance
RdmDownloadMock* gMock = nullptr;

// Mock Function Implementations
extern "C" {
    int fileCheck(const char* file) { return gMock->fileCheck(file); }
    int copyFiles(const char* src, const char* dest) { return gMock->copyFiles(src, dest); }
    int folderCheck(const char* path) { return gMock->folderCheck(path); }
    int createDir(const char* path) { return gMock->createDir(path); }
    int removeFile(const char* file) { return gMock->removeFile(file); }
    int emptyFolder(const char* folder) { return gMock->emptyFolder(folder); }
    int getFileLastModifyTime(const char* file) { return gMock->getFileLastModifyTime(file); }
    int getCurrentSysTimeSec() { return gMock->getCurrentSysTimeSec(); }
    void* doCurlInit() { return gMock->doCurlInit(); }
    int doHttpFileDownload(void* curl, FileDwnl_t* file, MtlsAuth_t* sec, int speed, void* data, int* code) {
        return gMock->doHttpFileDownload(curl, file, sec, speed, data, code);
    }
    void doStopDownload(void* curl) { gMock->doStopDownload(curl); }
    void rdmIARMEvntSendPayload(const char* pkg, const char* ver, const char* home, int error) {
        gMock->rdmIARMEvntSendPayload(pkg, ver, home, error);
    }
    JSON* ParseJsonStr(char* json) { return gMock->ParseJsonStr(json); }
    void GetJsonVal(JSON* json, const char* key, char* val, unsigned int size) {
        gMock->GetJsonVal(json, key, val, size);
    }
    void FreeJson(JSON* json) { gMock->FreeJson(json); }
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

// **Test Successful URL Update**
TEST_F(RdmDownloadTest, Test_rdmDwnlUpdateURL_Success) {
    char url[MAX_BUFF_SIZE] = "http://oldurl.com/package";
    
    EXPECT_CALL(*gMock, fileCheck(_)).WillOnce(Return(1));
    EXPECT_CALL(*gMock, copyFiles(_, _)).Times(2);
    EXPECT_CALL(*gMock, folderCheck(_)).WillOnce(Return(1));
    EXPECT_CALL(*gMock, copyFiles(_, _)).Times(1);
    
    EXPECT_EQ(rdmDwnlUpdateURL(url), RDM_SUCCESS);
}

// **Test Folder Creation Success**
TEST_F(RdmDownloadTest, Test_rdmDwnlCreateFolder_Success) {
    EXPECT_CALL(*gMock, createDir(_)).WillRepeatedly(Return(RDM_SUCCESS));

    EXPECT_EQ(rdmDwnlCreateFolder("/mnt/storage", "TestApp"), RDM_SUCCESS);
}

// **Test Folder Creation Failure**
TEST_F(RdmDownloadTest, Test_rdmDwnlCreateFolder_Failure) {
    EXPECT_CALL(*gMock, createDir(_)).WillOnce(Return(RDM_FAILURE));

    EXPECT_EQ(rdmDwnlCreateFolder("/mnt/storage", "TestApp"), RDM_FAILURE);
}

// **Test rdmDwnlDirect Download Success**
TEST_F(RdmDownloadTest, Test_rdmDwnlDirect_Success) {
    RDMAPPDetails appDetails;
    SetupRdmAppDetails(appDetails);

    EXPECT_CALL(*gMock, doCurlInit()).WillOnce(Return((void*)1));
    EXPECT_CALL(*gMock, doHttpFileDownload(_, _, _, _, _, _)).WillOnce(Return(0));

    char output[256];
    EXPECT_EQ(rdmDwnlDirect("https://test.com", "/tmp", "test.pkg", output, 0), RDM_SUCCESS);
}

// **Test rdmDwnlDirect Download Failure**
TEST_F(RdmDownloadTest, Test_rdmDwnlDirect_Failure) {
    EXPECT_CALL(*gMock, doCurlInit()).WillOnce(Return(nullptr));

    char output[256];
    EXPECT_EQ(rdmDwnlDirect("https://test.com", "/tmp", "test.pkg", output, 0), RDM_FAILURE);
}

// **Main Function**
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
