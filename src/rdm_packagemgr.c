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


#include <unistd.h>
#include "urlHelper.h"
#include "downloadUtil.h"
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_download.h"
#ifdef IARMBUS_SUPPORT
#include "rdmMgr.h"
#endif
#include "rdm_openssl.h"
#include "rdm_downloadutils.h"
#include "rdm_packagemgr.h"
#include <system_utils.h>

static INT32 rdmPkgDwnlValidation(RDMAPPDetails *pRdmAppDet)
{
    INT32 status = RDM_SUCCESS;
    INT32 loopcount = 1;
    CHAR  dwndlocation[768] = "";
    INT64 filesize;

    while(loopcount < MAX_LOOP_COUNT) {
        if(fileCheck(PACKAGE_SIGN_VERIFY_SUCCESS)) {
            strcat(dwndlocation, pRdmAppDet->app_dwnl_path);
            strcat(dwndlocation, pRdmAppDet->app_dwnl_url);

            if(fileCheck(dwndlocation)) {
                filesize = findSize(dwndlocation);
                RDMInfo("Size Info After Download %s %ld\n",dwndlocation,filesize);
            }

            RDMInfo("RSA Signature Validation Success for %s package\n",pRdmAppDet->app_name);
	    t2CountNotify("RDM_INFO_rsa_valid_signature", 1);

            removeFile(PACKAGE_SIGN_VERIFY_SUCCESS);
            break;
        }
        else if(fileCheck(PACKAGE_DOWNLOAD_FAILED)    ||
                fileCheck(PACKAGE_EXTRACTION_FAILED)  ||
                fileCheck(PACKAGE_SIGN_VERIFY_FAILED) ||
                (loopcount == MAX_LOOP_COUNT)) {

            if(fileCheck(PACKAGE_DOWNLOAD_FAILED)) {
                RDMError("%s package download failed\n",pRdmAppDet->app_name);
                removeFile(PACKAGE_DOWNLOAD_FAILED);
            }
            else if(fileCheck(PACKAGE_EXTRACTION_FAILED)) {
                RDMError("%s package extraction failed\n",pRdmAppDet->app_name);
                removeFile(PACKAGE_EXTRACTION_FAILED);
            }
            else if(fileCheck(PACKAGE_SIGN_VERIFY_FAILED)) {
                RDMError("%s package signature verification failed\n",pRdmAppDet->app_name);
                removeFile(PACKAGE_SIGN_VERIFY_FAILED);
            }
            else{
                RDMError("Max time reached. Failure in processing %s package\n",pRdmAppDet->app_name);
            }

            RDMError("Packager execution not successful. Cleanup and exit\n");
            removeFile(pRdmAppDet->app_dwnl_path);
            removeFile(pRdmAppDet->app_home);

            status = RDM_FAILURE;
            break;
        }
        else{
            RDMInfo("loop %d\n",loopcount);
            loopcount = loopcount + 1;
            sleep(6);
        }
    } //while(loopcount)

    return status;
}

/** @brief Install Packager RDM Apps 
 *
 *  @param[in] pUrl XConf SSR Download URL
 *
 *  @return Returns the status of the operation.
 *  @retval Returns RDM_SUCCESS on Success, RDM_FAILURE otherwise.
 */
static INT32 rdmPkgDwnlApplication(CHAR *pUrl)
{
    VOID *curl = NULL;
    CHAR  token[RDM_APP_PATH_LEN] = {0};
    CHAR  jsondata[MAX_BUFF_SIZE] = {0};
    CHAR  post_data[MAX_BUFF_SIZE] = {0};
    CHAR  header[]  = RDM_CURL_CONTENTTYPE;
    CHAR  token_header[MAX_BUFF_SIZE] = {0};
    CHAR  *p_token_header = NULL;
    INT32 curl_ret_code = -1;
    INT32 httpCode = 0;
    INT32 status = RDM_SUCCESS;

    DownloadData DwnLoc;
    FileDwnl_t file_dwnl;

    copyCommandOutput(RDM_WPE_SECUTIL, jsondata, sizeof(jsondata));

    snprintf(post_data,MAX_BUFF_SIZE,"{\"jsonrpc\":\"2.0\",\"id\":\"1234567890\",\"method\":\"Packager.1.install\",\"params\":{\"package\":\"%s\",\"version\":\"1.0\",\"architecture\":\"arm\"}}",pUrl);

    RDMInfo("post_data: [%s]\n jsondata: [%s]\n token: [%s]\n", post_data, jsondata, token);

    if(rdmMemDLAlloc(&DwnLoc, RDM_DEFAULT_DL_ALLOC) == RDM_SUCCESS) {
        if (DwnLoc.pvOut != NULL) {
            file_dwnl.pHeaderData = header;
            file_dwnl.pDlHeaderData = NULL;
            file_dwnl.pPostFields = post_data;
            file_dwnl.pDlData = &DwnLoc;

            snprintf(file_dwnl.url, sizeof(file_dwnl.url), "%s", RDM_JSONRPC_URL);
            if (0 == rdmJRPCTokenData(token, jsondata, sizeof(token))) {
                snprintf(token_header, sizeof(token_header), "Authorization: Bearer %s", token);
                p_token_header = token_header;
            }

            curl = doCurlInit();
            if(curl == NULL) {
                RDMError("Failed init curl\n");
                status = RDM_FAILURE;
               goto exit;
            }
            else{
                curl_ret_code = getJsonRpcData(curl, &file_dwnl, p_token_header, &httpCode);

                RDMInfo("curl_ret_code:%d httpCode:%d\n", curl_ret_code, httpCode);

                if(curl_ret_code && httpCode != 200) {
                    RDMError("Download failed\n");
                    status = RDM_FAILURE;
                }

                doStopDownload(curl);
            }
        } //if (DwnLoc.pvOut)
        else{
            RDMError("Failed to allocate memory pvOut\n");
            status = RDM_FAILURE;
        }
    } //if(rdmMemDLAlloc)
    else {
        RDMError("Failed to allocate Memory for the Dwnld Loc\n");
        status = RDM_FAILURE;
    }
exit:
    rdmMemDLFree(&DwnLoc);

    return status;
}

static INT32 rdmInvokePackage(RDMAPPDetails *pRdmAppDet)
{
    INT32 numofretries = 0;
    INT32 status = RDM_SUCCESS;

    while ( numofretries < PACKAGER_RETRY_COUNT ) {

        if (getProcessID(RDM_WPEPROCESS, NULL)) {
            RDMInfo("Packager is running. Sending the request\n");

            status = rdmPkgDwnlApplication(pRdmAppDet->app_dwnl_url);

            if(status == RDM_FAILURE) {
                RDMError("Failed to download the package\n");
                t2CountNotify("NF_INFO_rdm_package_failure", 1);
                break;
            }
            else if(status == RDM_SUCCESS) {
                RDMInfo("Success\n");
                break;
            }
        }
        else{
            RDMError("Packager not running. Retrying after %d sec\n", PACKAGER_RETRY_DELAY);
            sleep(PACKAGER_RETRY_DELAY);
        }

        numofretries = numofretries + 1;

        if (numofretries > PACKAGER_RETRY_COUNT) {
            RDMError("Packager retries exhausted. Exiting");
            status = RDM_FAILURE;
            break;
        }
    }//while()

    return status;
}

INT32 rdmPackageMgr(RDMAPPDetails *pRdmAppDet)
{
    INT32 status = RDM_SUCCESS;

    status = rdmInvokePackage(pRdmAppDet);

    if(status) {
        RDMError("Failed to extract the package\n");
        goto error;
    }

    status = rdmPkgDwnlValidation(pRdmAppDet);
    if(status) {
        RDMError("signature validation failed\n");
	t2CountNotify("RDM_ERR_rsa_signature_failed", 1);
        rdmDwnlUnInstallApp(pRdmAppDet->app_dwnl_path, pRdmAppDet->app_home);
        goto error;
    }
    else{
        RDMInfo("Package execution completed successfully\n");
    }

    rdmDwnlRunPostScripts(pRdmAppDet->app_home, 0);

error:
    return status;

}
