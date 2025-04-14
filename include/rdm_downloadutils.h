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

#ifndef _RDM_DOWNLOADUTILS_H_
#define _RDM_DOWNLOADUTILS_H_

//Define required file paths here
#define RDM_DWNL_URL            "/path/to/my/url/file"
#define RDM_DWNLSSR_URL         "/path/to/my/url/file"
#define RDM_NVRAM_PATH          "/nvram"
#define RDM_NVRAM_DL_PATH       "/nvram/path"
#define EnableOCSPStapling      "/path/to/EnableOCSPStapling"
#define EnableOCSP              "/path/to/EnableOCSPCA"
#define RDM_POSTSCRIPT_PATH     "/etc/rdm/post-services/"

#define CERT_DYNAMIC            "/path/to/dynamic/cert"
#define CERT_STATIC             "/path/to/static/cert"
#define KEY_STATIC              "/path/to/cert/key"
#define MTLS_CERT               "/path/to/mtls/cert"
#define MTLS_KEY                "/path/to/mtls/key"
#define GET_CONFIG_FILE         "/usr/bin/GetConfigFile"
#define SYS_CMD_GET_CONFIG_FILE "/usr/bin/GetConfigFile %s"
#define KEY_GEN_BIN             "/path/to/keygen"

#define RDKSSACLI           "/path/to/rdkssacli"

#ifdef RDM_ENABLE_CODEBIG
#define HTTP_XCONF_CODEBIG  3
#define DEFAULT_DL_ALLOC   1024
#define JSON_STR_LEN        1000
#define URL_MAX_LEN 512
#endif

#define DIRECT_BLOCK_TIME 86400
#define CB_BLOCK_TIME     1800

#define DOWNLOAD_APP_RETRY_COUNT 2
#define DOWNLOAD_APP_DIRECT_RETRY_DELAY 30
#define DOWNLOAD_RETRY_COUNT 12

INT32  rdmDwnlUpdateURL(CHAR *pUrl);
INT32  rdmDwnlApplication(CHAR *pUrl, CHAR *pDwnlPath, CHAR *pPkgName, CHAR *pOut, INT32 isMtls);
INT32  rdmDwnlIsOCSPEnable(void);
INT32  rdmDwnlCreateFolder(CHAR *pAppPath, CHAR *pAppname);
VOID   rdmDwnlCleanUp(CHAR *pDwnlPath);
VOID   rdmDwnlAppCleanUp(CHAR *pAppPath);
VOID   rdmRemvDwnlAppInfo(CHAR *pAppName, CHAR *pDwnlInfoFile);
INT32  rdmDwnlRunPostScripts(CHAR *pAppHome);
UINT32 rdmDwnlIsBlocked(CHAR *file, INT32 block_time);
VOID   rdmDwnlUnInstallApp(CHAR *pDwnlPath, CHAR *pAppPath);
INT32  rdmJRPCTokenData(CHAR *token,CHAR *pJsonStr,UINT32 token_size);
INT32  rdmMemDLAlloc(VOID *pDwnData, size_t szDataSize);
VOID   rdmMemDLFree(VOID *pvDwnData);
INT32  rdmDwnlValidation(RDMAPPDetails *pRdmAppDet, CHAR *pVer);
INT32 rdmUnInstallApps(INT32 is_broadband );
INT32 rdmGetManifestApps(CHAR **pAppsInManifest, INT32 *numOfApps);
INT32 rdmUpdateAppDetails(RDMHandle *prdmHandle,
                          RDMAPPDetails *pRdmAppDet,
                          INT32 is_broadband);
VOID rdmPrintAppDetails(RDMAPPDetails *pRdmAppDet);
INT32 rdmPackageMgrStateChange(RDMAPPDetails *pRdmAppDet);
INT32 rdmJRPCResultData(CHAR *result, CHAR *pJsonStr, UINT32 result_size);
INT32 rdmDwnlGetCert(MtlsAuth_t *sec);
#endif //_RDM_DOWNLOADUTILS_H_
