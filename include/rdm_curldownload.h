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

#ifndef _RDM_CURLDOWNLOAD_H_
#define _RDM_CURLDOWNLOAD_H_

typedef struct _rdmMTLSParam {
    INT32 def_val;
    CHAR *sslcert_type;
    CHAR *sslcert_path;
    CHAR *sslkey_pass;
    INT32 ssl_peerverify;
}RDMCertParam;

typedef struct _rdmCurlParams {
    CHAR *url;
    INT32 ssl_version;
    INT32 conn_timeout;
    INT32 full_conn_timeout;
    VOID *callback;
    VOID *callback_param;
    RDMCertParam cert_param;

}RDMCurlParams;


INT32 rdmCurlInit(VOID **pCurlHandle);
VOID  rdmCurlUnInit(VOID *pCurlHandle);
INT32 rdmCurlSetParams(VOID *pCurlHandle, RDMCurlParams *curl_param);
INT32 rdmCurlDwnStart(VOID *pCurlHandle);

#endif //_RDM_CURLDOWNLOAD_H_
