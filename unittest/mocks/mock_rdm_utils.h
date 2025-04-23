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

#ifndef MOCK_RDM_UTILS_H
#define MOCK_RDM_UTILS_H

#include <gmock/gmock.h>

extern "C" {
    #include "rdm_types.h"
    #include "rdm.h"
    #include "rdm_utils.h"
    #include "system_utils.h"
}

class MockRdmUtils {
public:
    MOCK_METHOD1(checkFileSystem, INT32(const CHAR*));
    MOCK_METHOD3(findPFile, INT32(const CHAR*, const CHAR*, CHAR*));
    MOCK_METHOD1(emptyFolder, INT32(const CHAR*));
    MOCK_METHOD1(getFreeSpace, UINT32(const CHAR*));
    MOCK_METHOD1(createDir, INT32(const CHAR*));
    MOCK_METHOD(void*, doCurlInit, (), ());
    MOCK_METHOD(INT32, doHttpFileDownload, (void*, FileDwnl_t*, MtlsAuth_t*, unsigned int, char*, int*), ());
    MOCK_METHOD(void, doStopDownload, (void *), ());
    MOCK_METHOD(void, copyCommandOutput, (char*, char*, int), ());
    MOCK_METHOD(const char*, getExtension, (char*), ());
    MOCK_METHOD(int, tarExtract, (char*, char*), ());
    MOCK_METHOD(int, fileCheck, (char*), ());
    MOCK_METHOD(void, rdmInitSslLib, (), ());
    MOCK_METHOD(INT32, rdmOpensslRsafileSignatureVerify, (const CHAR*, size_t, const CHAR*, const CHAR*, CHAR*, INT32*), ());
    MOCK_METHOD(INT32, rdmDecryptKey, (CHAR*), ());
    MOCK_METHOD(unsigned int, getFileLastModifyTime, (CHAR*), ());
    MOCK_METHOD(time_t, getCurrentSysTimeSec, (), ());
    MOCK_METHOD(int, copyFiles, (char*, char*), ());
    MOCK_METHOD(int, folderCheck, (char*), ());
    MOCK_METHOD(int, removeFile, (char*), ());
    MOCK_METHOD(JSON*, ParseJsonStr, (char*), ());
    MOCK_METHOD(size_t, GetJsonVal, (JSON*, char*, char*, size_t), ());
    MOCK_METHOD(int, FreeJson, (JSON*), ());
    MOCK_METHOD(int, isDataInList, (char**, char*, int), ());
    MOCK_METHOD(int, strSplit, (char*, char*, char**, int), ());
    MOCK_METHOD(int, findPFileAll, (char*, char*, char**, int*, int), ());
    MOCK_METHOD(void, qsString, (char**, unsigned int), ());
    MOCK_METHOD(int, strRmDuplicate, (char**, int), ());
};

#endif // MOCK_RDM_UTILS_H
