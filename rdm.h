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

#ifndef _RDM_H_
#define _RDM_H_

/* Constants */
#define RDM_FILEPATH_LEN    128

#define RDM_APPNAME_LEN    128
#define RDM_JSONPATH_LEN   256
#define RDM_JSONQ_LEN      10
#define RDM_DL_PKGNAME_LEN 256
#define RDM_DL_PKGVER_LEN  128
#define RDM_DL_APPSIZE_LEN 8
#define RDM_DL_PKGTYPE_LEN 8
#define RDM_DL_JSQUERY_LEN 64
#define RDM_APP_PATH_LEN   256
#define RDM_TMP_LEN_64     64
#define MAX_BUFF_SIZE      512

typedef struct _rdmAppDetails {
    CHAR  app_name[RDM_APPNAME_LEN];
    CHAR  pkg_name[RDM_DL_PKGNAME_LEN];
    CHAR  pkg_type[RDM_DL_PKGTYPE_LEN];
    CHAR  pkg_ver[RDM_DL_PKGVER_LEN];
    CHAR  app_size[RDM_DL_APPSIZE_LEN];
    CHAR  dwld_method_controller[RDM_JSONQ_LEN];
    CHAR  app_home[RDM_APP_PATH_LEN];
    CHAR  app_mnt[RDM_APP_PATH_LEN];
    CHAR  app_dwnl_home[RDM_APP_PATH_LEN];
    CHAR  app_dwnl_path[RDM_APP_PATH_LEN];
    CHAR  app_dwnl_filepath[MAX_BUFF_SIZE];
    CHAR  app_dwnl_info[RDM_APP_PATH_LEN];
    CHAR  app_dwnl_url[MAX_BUFF_SIZE];
    CHAR  app_usb[MAX_BUFF_SIZE];
    INT32 app_size_kb;
    INT32 app_size_mb;
    INT32 dwld_on_demand;
    INT32 is_versioned;
    INT32 bFsCheck;
    INT32 is_oss;
    INT32 is_mtls;
#ifdef RDM_ENABLE_CODEBIG
    INT32 is_codebig;
#endif
    INT32 is_usb;
    INT32 is_versioned_app;
    INT32 sig_type;
    INT32 dwld_status;
}RDMAPPDetails;

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _rdmHandle
{
    VOID *pRbusHandle;
    RDMAPPDetails *pApp_det;
} RDMHandle;

INT32 rdmInit(RDMHandle *prdmHandle);
VOID  rdmUnInit(RDMHandle *prdmHandle);

void t2CountNotify(char *marker, int val);
void t2ValNotify(char *marker, char *val);

#ifdef __cplusplus
}
#endif
#endif //_RDM_H_
