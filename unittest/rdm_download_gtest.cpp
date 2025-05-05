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
#include <gmock/gmock-actions.h>
#include "mocks/mock_rdm_utils.h"
#include "mocks/mock_rdm_downloadutils.h"
#include "mocks/mock_iarm_bus.h"
#include "mocks/system_utils.h"
#include "mocks/mock_rdm_rbus.h"

// Declare the C functions with extern "C"
extern "C" {
    #include "rdm_download.h"
    #include "rdm_types.h"
    #include "mocks/libIBus.h"
    #include "mocks/rdmMgr.h"
    #include "rdm_downloadutils.h"
    #include "rdm_downloadverapp.h"
}

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmdownload_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256
using ::testing::_;
using ::testing::Return;
using ::testing::StrEq;


MockRdmUtils* mockRdmUtils = nullptr;
MockRdmRbus* mockRdmRbus = new MockRdmRbus();
MockRdmDownloadUtils* mockRdmDownloadUtils;
MockIARM* mockIARM = new MockIARM();
extern "C"{
    rbusValueType_t rbusValue_GetType(void* paramName) {
        return mockRdmRbus->rbusValue_GetType(paramName);
    }

    INT32 rbus_get(void* handle, INT8* val, void** paramName) {
        return mockRdmRbus->rbus_get(handle, val, paramName);
    }

    bool rbusValue_GetBoolean(void* paramName) {
        return mockRdmRbus->rbusValue_GetBoolean(paramName);
    }

    INT8* rbusValue_ToString(void* str, void* paramName, int len) {
        return mockRdmRbus->rbusValue_ToString(str, paramName, len);
    }

    void rbusValue_Release(void* ptr) {
        return mockRdmRbus->rbusValue_Release(ptr);
    }

    INT32 rbus_checkStatus() {
        return mockRdmRbus->rbus_checkStatus();
    }

    INT32 rbus_open(void* handle, INT8 *name) {
        return mockRdmRbus->rbus_open(handle, name);
    }

    INT32 rbus_close(void* handle) {
        return mockRdmRbus->rbus_close(handle);
    }

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

    int copyFiles(char *dest, char *src) {
        return mockRdmUtils->copyFiles(dest, src);
    }

    int removeFile(char *file) {
        return mockRdmUtils->removeFile(file);
    }

    int strSplit(char *in, char *tok, char **out, int len) {
        return mockRdmUtils->strSplit(in, tok, out, len);
    }

   int findPFileAll(char *path, char *search, char **out, int *found_t, int max_list) {
      return mockRdmUtils->findPFileAll(path, search, out, found_t, max_list);
    }

    

    void qsString(char *arr[], unsigned int length) {
        return mockRdmUtils->qsString(arr, length);
    }

    int strRmDuplicate(char **in, int len) {
        return mockRdmUtils->strRmDuplicate(in, len);
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

    void copyCommandOutput(char *filePath, char *fileOut, int len) {
        return mockRdmUtils->copyCommandOutput(filePath, fileOut, len);
    }

    void rdmInitSslLib() {
        return mockRdmUtils->rdmInitSslLib();
    }

    INT32 rdmOpensslRsafileSignatureVerify(const CHAR *data_file, size_t file_len, const CHAR *sig_file, const CHAR *vkey_file, CHAR *reply_msg, INT32 *reply_msg_len) {
        return mockRdmUtils->rdmOpensslRsafileSignatureVerify(data_file, file_len, sig_file, vkey_file, reply_msg, reply_msg_len);
    }

    INT32 rdmDecryptKey(CHAR *outKey) {
        return mockRdmUtils->rdmDecryptKey(outKey);
    }

    unsigned int getFileLastModifyTime(CHAR *file) {
        return mockRdmUtils->getFileLastModifyTime(file);
    }

    time_t getCurrentSysTimeSec() {
        return mockRdmUtils->getCurrentSysTimeSec();
    }

    int folderCheck(char *dir) {
        return mockRdmUtils->folderCheck(dir);
    }

    JSON* ParseJsonStr(char *pJsonStr) {
        return mockRdmUtils->ParseJsonStr(pJsonStr);
    }

    size_t GetJsonVal(JSON *pJson, char *pValToGet, char *pOutputVal, size_t maxlen) {
        return mockRdmUtils->GetJsonVal(pJson, pValToGet, pOutputVal, maxlen);
    }

    int FreeJson(JSON *pJson) {
        return mockRdmUtils->FreeJson(pJson);
    }

    int isDataInList(char **pList, char *pData, int count) {
        return mockRdmUtils->isDataInList(pList, pData, count);
    }

}




using ::testing::NiceMock;

class RdmDownloadVerAppTest : public ::testing::Test {
protected:
    NiceMock<MockRdmUtils> mockUtilsInstance;
   

    void SetUp() override {
         mockRdmUtils = &mockUtilsInstance;
        // Default mocks used by rdmDownloadVerApp() internals
        ON_CALL(mockUtilsInstance, rdmJSONGetLen(_, _)).WillByDefault(Return(1));
        ON_CALL(mockUtilsInstance, rdmJSONGetAppNames(_, _)).WillByDefault(Return(0));
        ON_CALL(mockUtilsInstance, rdmJSONGetAppDetName(_, _)).WillByDefault(Return(0));
        ON_CALL(mockUtilsInstance, rdmDwnlValidation(_, _)).WillByDefault(Return(0));
        ON_CALL(mockUtilsInstance, getFileLastModifyTime(_)).WillByDefault(Return(123456));
        ON_CALL(mockUtilsInstance, getCurrentSysTimeSec()).WillByDefault(Return(123556));
        ON_CALL(mockUtilsInstance, rdmDownloadMgr(_)).WillByDefault(Return(0));
        ON_CALL(mockUtilsInstance, rdmDwnlUnInstallApp(_, _)).WillByDefault(Return());
    }
   void TearDown() override {
       mockRdmUtils = nullptr;
   }
};





/*
TEST_F(RdmDownloadVerAppTest, SkipsInstallIfVersionIsTooOld) {
    // Arrange
    const char* jsonData = strdup(R"({"versionlist":[{"name":"App1","version":"1.0"}]})");

    EXPECT_CALL(mockUtils, getFileLastModifyTime(_)).WillOnce(Return(123456));
    EXPECT_CALL(mockUtils, getCurrentSysTimeSec()).WillOnce(Return(123456 + 800000)); // Older than threshold
    EXPECT_CALL(mockUtils, rdmDownloadMgr(_)).Times(0); // Should not be called

    // Act
    int result = rdmDownloadVerApp(const_cast<char*>(jsonData));

    // Assert
    EXPECT_EQ(result, 0);
}

TEST_F(RdmDownloadVerAppTest, HandlesUninstallCase) {
    // Arrange
    const char* jsonData = strdup(R"({"versionlist":[{"name":"App2","version":""}]})");

    EXPECT_CALL(mockUtils, rdmJSONGetLen(_, _)).WillOnce(Return(1));
    EXPECT_CALL(mockUtils, rdmJSONGetAppNames(_, _)).WillOnce(Return(0));
    EXPECT_CALL(mockUtils, rdmJSONGetAppDetName(_, _)).WillOnce(Invoke([](char*, RDMAPPDetails* det) {
        strcpy(det->name, "App2");
        det->version[0] = '\0'; // empty version = uninstall
        return 0;
    }));

    EXPECT_CALL(mockUtils, rdmDwnlUnInstallApp(_, _)).Times(1);

    // Act
    int result = rdmDownloadVerApp(const_cast<char*>(jsonData));

    // Assert
    EXPECT_EQ(result, 0);
}*/

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
    appDetails.is_versioned = true;
    appDetails.is_usb = 0;

    INT32 DLStatus = RDM_DL_NOERROR;

    EXPECT_CALL(*mockRdmUtils, checkFileSystem(::testing::_))
        .WillRepeatedly(Return(1));
    EXPECT_CALL(*mockRdmUtils, findPFile(::testing::_,::testing::_,::testing::_))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*mockRdmUtils, emptyFolder(::testing::_))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*mockRdmUtils, emptyFolder(::testing::_))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*mockRdmUtils, getFreeSpace(::testing::_))
        .WillRepeatedly(Return(200));
    EXPECT_CALL(*mockRdmUtils, createDir(_))
        .WillRepeatedly(Return(RDM_SUCCESS));

    EXPECT_EQ(rdmDownloadApp(&appDetails, &DLStatus), RDM_SUCCESS);
    EXPECT_EQ(DLStatus, RDM_DL_NOERROR);
}


/*TEST(RdmDownloadVerAppTest, TestSuccessfulDownloadAndInstall) {
    RDMAPPDetails appDet;
    memset(&appDet, 0, sizeof(RDMAPPDetails));

    // Fill minimal fields for processing
    strcpy(appDet.app_name, "MyApp");
    strcpy(appDet.pkg_ver, "pkg-v1 pkg-v2");
    strcpy(appDet.app_home, "/opt/apps/MyApp");
    strcpy(appDet.app_dwnl_path, "/tmp/downloads/MyApp");

    // Mock JSON query behavior
    EXPECT_CALL(*mockRdmUtils, findPFileAll(_, _, _, _, _))
        .WillOnce([](const char*, const char*, char** pkg_json, int* num, int) {
            strcpy(pkg_json[0], "/opt/apps/MyApp/v1/package.json");
            *num = 1;
        });

    EXPECT_CALL(*mockRdmUtils, rdmJSONQuery(_, _, _))
        .WillOnce([](const char*, const char*, char* ver_out) {
            strcpy(ver_out, "v1");
        });

    EXPECT_CALL(*mockRdmUtils, rdmDwnlValidation(_, _))
        .WillRepeatedly(Return(0)); // assume validation success

    // Mock uninstall and install behavior
    EXPECT_CALL(*mockRdmDownloadMgr, rdmDownloadMgr(_))
        .WillOnce(Return(0)); // install success

    EXPECT_CALL(*mockRdmDownloadMgr, rdmDwnlUnInstallApp(_, _))
        .Times(::testing::AtMost(1)); // may or may not be called depending on input

    EXPECT_CALL(*mockSystemUtils, removeFile(_))
        .Times(::testing::AnyNumber());

    // Execute
    INT32 status = rdmDownloadVerApp(&appDet);

    // Verify
    EXPECT_EQ(status, RDM_SUCCESS);
}*/
                    
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

      EXPECT_CALL(*mockRdmUtils, checkFileSystem(::testing::_))
        .WillRepeatedly(Return(1));
     EXPECT_CALL(*mockRdmUtils, findPFile(::testing::_,::testing::_,::testing::_))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*mockRdmUtils, emptyFolder(::testing::_))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*mockRdmUtils, getFreeSpace(::testing::_))
        .WillRepeatedly(Return(appDetails.app_size_mb + 1));

    EXPECT_CALL(*mockRdmUtils, createDir(::testing::_))
        .WillRepeatedly(Return(RDM_FAILURE));

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

TEST_F(RDMDownloadTest, rdmDownloadVerApp_Integration) {
    RDMAPPDetails appDetails = {};
    strncpy(appDetails.pkg_ver, "1.0 2.0", sizeof(appDetails.pkg_ver) - 1);
    strncpy(appDetails.app_name, "test_app", sizeof(appDetails.app_name) - 1);
    strncpy(appDetails.app_dwnl_path, "/downloads/test", sizeof(appDetails.app_dwnl_path) - 1);
    strncpy(appDetails.app_home, "/home/test", sizeof(appDetails.app_home) - 1);

    EXPECT_CALL(*mockRdmUtils, strSplit(_, _, _, _))
        .WillOnce(testing::Invoke([](const char* input, const char* delimiter, char** output, int max_list) {
            output[0] = strdup("1.0");
            output[1] = strdup("2.0");
            return 2; // Simulate two valid versions
        }));

    EXPECT_CALL(*mockRdmUtils, findPFileAll(_, _, _, _, _))
        .WillOnce(testing::Invoke([](const char* path, const char* search, char** output, int* found, int max_list) {
            output[0] = strdup("/home/test/v1/package.json");
            *found = 1;
            return 0;
        }));

    EXPECT_CALL(*mockRdmUtils, rdmDwnlUnInstallApp(_, _))
    .Times(1);
    
    EXPECT_CALL(*mockRdmUtils, rdmDownloadMgr(_))
        .WillRepeatedly(Return(0)); // Simulate successful installation

    EXPECT_CALL(*mockRdmUtils, qsString(_, _)).Times(::testing::AnyNumber());
    EXPECT_CALL(*mockRdmUtils, strRmDuplicate(_, _))
        .WillOnce(Return(1)); // Simulate removing duplicate versions

    INT32 status = rdmDownloadVerApp(&appDetails);
    EXPECT_EQ(status, RDM_SUCCESS);
}
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
                                  
                                                 					
