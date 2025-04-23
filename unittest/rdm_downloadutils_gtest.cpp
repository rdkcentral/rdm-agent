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
#include "mocks/mock_rdm_utils.h"
#include "mocks/system_utils.h"
#include "mocks/mock_rdm_rbus.h"

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
MockRdmRbus* mockRdmRbus = new MockRdmRbus();
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
    //retStr = (char*)malloc(3 * sizeof(char)); // Allocate memory for the string
    /*if (retStr != nullptr)
        strcpy(retStr, "sh"); // Copy the value "sh" into retStr
    */
    EXPECT_CALL(*mockRdmUtils, getExtension(::testing::_))
        .WillOnce(Return("sh"));
    EXPECT_CALL(*mockRdmUtils, copyCommandOutput(::testing::_, ::testing::_, ::testing::_));
    EXPECT_EQ(rdmDwnlRunPostScripts(pAppHome), RDM_SUCCESS);

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
    CHAR pkg_file[RDM_APP_PATH_LEN];

    system("mkdir -p /etc/rdm && touch /etc/rdm/test_app_cpemanifest");
    system("mkdir -p /home/test/ && touch /home/test/test_app_cpemanifest");

    EXPECT_CALL(*mockRdmUtils, findPFile(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(Return(1));

    EXPECT_CALL(*mockRdmUtils, rdmDecryptKey(::testing::_))
	.WillOnce(Return(RDM_SUCCESS));

    EXPECT_CALL(*mockRdmUtils, rdmInitSslLib());

    EXPECT_CALL(*mockRdmUtils, rdmOpensslRsafileSignatureVerify(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
	.WillOnce(Return(354705069));

    EXPECT_EQ(rdmDwnlValidation(&appDetails, NULL), RDM_SUCCESS);
}

TEST(rdmDwnlValidation, rdmDwnlValidation_Failure) {
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
    CHAR pkg_file[RDM_APP_PATH_LEN];

    system("mkdir -p /etc/rdm && touch /etc/rdm/test_app_cpemanifest");
    system("mkdir -p /home/test/ && touch /home/test/test_app_cpemanifest");

    EXPECT_CALL(*mockRdmUtils, findPFile(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(Return(1));

    EXPECT_CALL(*mockRdmUtils, rdmDecryptKey(::testing::_))
        .WillOnce(Return(RDM_SUCCESS));

    EXPECT_CALL(*mockRdmUtils, rdmInitSslLib());

    EXPECT_CALL(*mockRdmUtils, rdmOpensslRsafileSignatureVerify(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(Return(0));

    EXPECT_EQ(rdmDwnlValidation(&appDetails, NULL), RDM_FAILURE);
    delete mockRdmUtils;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
