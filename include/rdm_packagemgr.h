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

#ifndef _RDM_PACKAGEMGR_H_
#define _RDM_PACKAGEMGR_H_

#define RDM_WPEPROCESS              "WPEProcess -l libWPEFrameworkPackager.so"
#define RDM_CURL_CONTENTTYPE        "Content-Type: application/json"
#define RDM_WPE_SECUTIL             "/usr/bin/WPEFrameworkSecurityUtility"
#define RDM_JSONRPC_URL             "http://127.0.0.1:9998/jsonrpc"
#define PACKAGE_SIGN_VERIFY_SUCCESS "/tmp/.opkg_rdm_sign_verify_success"
#define PACKAGE_DOWNLOAD_FAILED     "/tmp/.opkg_rdm_download_failed"
#define PACKAGE_SIGN_VERIFY_FAILED  "/tmp/.opkg_rdm_sign_verify_failed"
#define PACKAGE_SIGN_VERIFY_SUCCESS "/tmp/.opkg_rdm_sign_verify_success"
#define PACKAGE_EXTRACTION_FAILED   "/tmp/.opkg_rdm_extract_failed"

#define MAX_LOOP_COUNT         50
#define PACKAGER_RETRY_COUNT   2
#define PACKAGER_RETRY_DELAY   5
#define RDM_DEFAULT_DL_ALLOC   1024

#endif //_RDM_DOWNLOAD_H_
