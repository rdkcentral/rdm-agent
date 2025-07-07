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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <curl/curl.h>

#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
##include "rdm_curldownload.h"

INT32 rdmCurlInit(VOID **pCurlHandle)
{
    CURL *curl;
    curl = curl_easy_init();
    if(curl) {
        *pCurlHandle = curl;
    }
    else {
        RDMError("curl init failed\n");
        return RDM_FAILURE;
    }
    return RDM_SUCCESS;
}

VOID rdmCurlUnInit(VOID *pCurlHandle)
{
    CURL *curl = (CURL *)pCurlHandle;
    curl_easy_cleanup(curl);
}

INT32 rdmCurlSetParams(VOID *pCurlHandle, RDMCurlParams *curl_param)
{
    CURL *curl = (CURL *)pCurlHandle;
    CURLcode code = CURLE_OK;
    RDMCertParam *pCert_params;

    if(curl_param == NULL) {
        RDMError("Invalid curl params\n");
        return RDM_FAILURE;
    }
    if(curl == NULL) {
        RDMError("Invalid curl handle\n");
        return RDM_FAILURE;
    }

    pCert_params = &curl_param->cert_param;

    code = curl_easy_setopt(curl, CURLOPT_URL, curl_param->url);
    if(code != CURLE_OK) {
        RDMError("Curl set opts[CURLOPT_URL] failed with error %s \n", curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    code = curl_easy_setopt(curl, CURLOPT_SSLVERSION, curl_param->ssl_version);
    if(code != CURLE_OK) {
        RDMError("Curl set opts[CURLOPT_SSLVERSION] failed with error %s \n", curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    code = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, curl_param->conn_timeout);
    if(code != CURLE_OK) {
        RDMError("Curl set opts[CURLOPT_CONNECTTIMEOUT] failed with error %s \n", curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, curl_param->full_conn_timeout);
    if(code != CURLE_OK) {
        RDMError("Curl set opts[CURLOPT_TIMEOUT] failed with error %s \n", curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_param->callback);
    if(code != CURLE_OK) {
        RDMError("Curl set opts[CURLOPT_WRITEFUNCTION] failed with error %s \n", curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, curl_param->callback_param);
    if(code != CURLE_OK) {
        RDMError("Curl set opts[CURLOPT_WRITEDATA] failed with error %s \n", curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    /* Set Certificate details */
    code = curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, pCert_params->def_val);
    if(code != CURLE_OK) {
        RDMError("Curl set opts[CURLOPT_SSLENGINE_DEFAULT] failed with error %s \n", curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    code = curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, pCert_params->sslcert_type);
    if(code != CURLE_OK) {
        RDMError("Curl set opts[CURLOPT_SSLCERTTYPE] failed with error %s \n", curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    code = curl_easy_setopt(curl, CURLOPT_SSLCERT, pCert_params->sslcert_path);
    if(code != CURLE_OK) {
        RDMError("Curl set opts[CURLOPT_SSLCERT] failed with error %s \n", curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    code = curl_easy_setopt(curl, CURLOPT_KEYPASSWD, pCert_params->sslkey_pass);
    if(code != CURLE_OK) {
        RDMError("Curl set opts[CURLOPT_KEYPASSWD] failed with error %s \n", curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    /* disconnect if authentication fails */
    code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, pCert_params->ssl_peerverify);
    if(code != CURLE_OK) {
        RDMError("Curl set opts[CURLOPT_SSL_VERIFYPEER] failed with error %s \n", curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    return RDM_SUCCESS;
}

INT32 rdmCurlDwnStart(VOID *pCurlHandle)
{
    CURL *curl = (CURL *)pCurlHandle;
    CURLcode code = CURLE_OK;
    INT64 http_code = 0;

    if(curl == NULL) {
        RDMError("Invalid curl handle\n");
        return RDM_FAILURE;
    }

    code = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if(http_code == 200 && code == CURLE_OK) {
        RDMInfo("RDM XCONF communication success\n");
    }
    else {
        RDMError("RDM XCONF communication Failed with http code : %ld Curl code : %u (%s) \n", http_code, code, curl_easy_strerror(code));
        return RDM_FAILURE;
    }

    return RDM_SUCCESS;
}
