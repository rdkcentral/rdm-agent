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

#ifndef _RDM_DOWNLOAD_H_
#define _RDM_DOWNLOAD_H_

#define APP_MOUNT_PATH          "/media/apps"
#define APPLN_HOME_PATH_DEF     "/tmp"
#define RDM_DL_INFO             "/opt/persistent/rdmDownloadInfo.txt"
#define RDM_DL_INFO_BR          "/nvram/persistent/rdmDownloadInfo.txt"
#define RDM_CPEMANIFEST_PATH    "/etc/rdm/"

#define RDM_APPHOME_LEN    256
#define RDM_APPDLP_LEN     256
#define RDM_SIGFILE_LEN    1024

#define RDM_KMS_SKIP_BYTES 6

#define RDM_RETRY_COUNT 2

typedef enum _RDM_DOWNLOAD_STATUS {
    RDM_DL_NOERROR    = 0,
    RDM_DL_PARAMERR   = 1,
    RDM_DL_VALERR     = 2,
    RDM_DL_DWNLERR    = 3,
    RDM_DL_MISC       = 4
}RDM_DOWNLOAD_STATUS;

INT32 rdmDownloadApp(RDMAPPDetails *pRdmAppDet, INT32 *pDLStatus);
INT32 rdmDownloadVerApp(RDMAPPDetails *pRdmAppDet);
INT32 rdmDownloadMgr(RDMAPPDetails *pRdmAppDet);
INT32 rdmPackageMgr(RDMAPPDetails *pRdmAppDet);
INT32 rdmDwnlExtract(RDMAPPDetails *pRdmAppDet);
#endif //_RDM_DOWNLOAD_H_
