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
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <json_parse.h>
#include <downloadUtil.h>
#include <system_utils.h>

#ifdef RDM_ENABLE_CODEBIG
#include "codebigUtils.h"
#endif
#include "rdmMgr.h"
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_download.h"
#include "rdm_rbus.h"
#include "rdm_openssl.h"
#include "rdm_downloadutils.h"
#include "rdm_packagemgr.h"

#ifdef LIBRDKCONFIG_BUILD
#include "rdkconfig.h"
#endif

#include "secure_wrapper.h"
#include <sys/stat.h>

UINT32 rdmDwnlIsBlocked(CHAR *file, INT32 block_time)
{
    UINT32 current_time = 0;
    UINT32 last_mod_time = 0;
    UINT32 modification_time = 0;
    INT32  remtime = 0;

    last_mod_time = getFileLastModifyTime(file);
    if (last_mod_time != 0) {
        current_time = getCurrentSysTimeSec();
        modification_time = current_time - last_mod_time;
        remtime = (block_time/60) - (modification_time/60);

        if (modification_time <= block_time) {
            RDMInfo("Direct Download attempt blocked due to Previous Failure. Blocking period Valid for %d minutes ", remtime);
        } else {
            RDMInfo("Blocking Period Expired. Removing %s. Allowing Direct Download \n", file); 
            unlink(file);
            modification_time = 0;
        }
    }
    return modification_time;
}

INT32 rdmDwnlUpdateURL(CHAR *pUrl)
{
    CHAR dwnl_url[MAX_BUFF_SIZE];
    FILE *fp  = NULL;
    INT32 status = RDM_SUCCESS;

    if(fileCheck(RDM_DWNL_URL)) {
        copyFiles(RDM_DWNL_URL, RDM_DWNLSSR_URL);

        if(folderCheck(RDM_NVRAM_PATH)) {
            copyFiles(RDM_DWNLSSR_URL, RDM_NVRAM_DL_PATH);
        }
    }
#if 0
    else if(BOX_TYPE = "XB3"){

    }
#endif
    fp = fopen(RDM_DWNLSSR_URL, "r");
    if(fp) {
        fgets(dwnl_url, MAX_BUFF_SIZE, fp);
        if(strstr(dwnl_url, "http")) {
            if(dwnl_url[strlen(dwnl_url)-1] == '\n') {
                dwnl_url[strlen(dwnl_url)-1] = '\0';
            }
            strncpy(pUrl, dwnl_url, MAX_BUFF_SIZE);
	    RDMInfo("Download URL available in %s is %s\n", RDM_DWNLSSR_URL, pUrl);
	    t2ValNotify("RDM_INFO_DownloadSSRURL", pUrl);
        }
        fclose(fp);
    }
    else {
	RDMInfo("Download URL is not available in %s\n", RDM_DWNLSSR_URL);
	RDMInfo("Using RDM Default URL to download from RFC: %s\n", pUrl);
	t2ValNotify("RDM_INFO_DefaultURL", pUrl);
    }

    if(strstr(pUrl, "http")) {
        /* Replace http with https */
        if(!strstr(pUrl, "https")) {
            memset(dwnl_url, 0, MAX_BUFF_SIZE);
            strncpy(dwnl_url, "https", MAX_BUFF_SIZE);
            strncpy(dwnl_url+5, pUrl+4, MAX_BUFF_SIZE-5);
            strncpy(pUrl, dwnl_url, MAX_BUFF_SIZE);
        }
	RDMInfo("RDM App Download URL Location is %s\n", pUrl);
    }
    else {
        RDMError("RDM download url is not available in both %s and RFC parameter. Exiting...\n", RDM_DWNLSSR_URL);
	t2ValNotify("SYST_ERR_RDMMISSING", RDM_DWNLSSR_URL);
        status = RDM_FAILURE;
    }
    return status;
}

INT32 rdmDwnlCreateFolder(CHAR *pAppPath, CHAR *pAppname)
{
    INT32 status = RDM_SUCCESS;
    CHAR  tmp[RDM_APP_PATH_LEN]  = {0};

    strncpy(tmp, pAppPath, RDM_APP_PATH_LEN);
    strcat(tmp, "/rdm");
    status = createDir(tmp);
    if(status) {
        return RDM_FAILURE;
    }

    strcat(tmp, "/downloads");
    status = createDir(tmp);
    if(status) {
        return RDM_FAILURE;
    }

    strcat(tmp, "/");
    strcat(tmp, pAppname);
    status = createDir(tmp);
    if(status) {
        return RDM_FAILURE;
    }

    strncpy(tmp, pAppPath, RDM_APP_PATH_LEN);
    strcat(tmp, "/");
    strcat(tmp, pAppname);
    status = createDir(tmp);
    if(status) {
        return RDM_FAILURE;
    }

    return status;
}

VOID rdmDwnlCleanUp(CHAR *pDwnlPath)
{
    INT32  fs_status = RDM_SUCCESS;

    fs_status = emptyFolder(pDwnlPath);
    if(fs_status) {
        RDMError("RDM Download Cleanup Failed : Unable to empty the folder %s\n", pDwnlPath);
    }

    removeFile(pDwnlPath);
}

VOID rdmDwnlAppCleanUp(CHAR *pAppPath)
{
    INT32  fs_status = RDM_SUCCESS;

    fs_status = emptyFolder(pAppPath);
    if(fs_status) {
        RDMError("RDM Downmload Application Cleanup Failed : Unable to empty the folder: %s\n", pAppPath);
    }

    removeFile(pAppPath);
}

VOID rdmRemvDwnlAppInfo(CHAR *pAppName, CHAR *pDwnlInfoFile)
{
    FILE *fp_met = NULL;

    fp_met = fopen(pDwnlInfoFile, "a+");
    if(fp_met) {
        char buffer[4096]={0};
        char line[256];
        size_t buffer_index = 0;

        RDMInfo("File %s Opened successfully for cleanup\n", pDwnlInfoFile);
        fseek(fp_met, 0, SEEK_SET);
        while (fgets(line, sizeof(line), fp_met)) {
            if(strncmp(line, pAppName, strlen(pAppName)) != 0 ){
                strncpy(buffer + buffer_index, line, sizeof(buffer) - buffer_index);
                buffer_index += strlen(line);
            }
        }

        ftruncate(fileno(fp_met), 0);
        fseek(fp_met, 0, SEEK_SET);

        fputs(buffer, fp_met);

        fseek(fp_met, 0, SEEK_END);
        fclose(fp_met);
        }
    else{
            RDMInfo("RDM Application removal Failed : Unable to open file %s", pDwnlInfoFile);
        }
}

#ifdef LIBRDKCERTSELECTOR
#define FILESCHEME "file://"
/* Description: Use for get all mtls related certificate and key.
 * @param sec: This is a pointer hold the certificate, key and type of certificate.
 * @return : MTLS_CERT_FETCH_SUCCESS on success, MTLS_CERT_FETCH_FAILURE on mtls cert failure.
 * */
MtlsAuthStatus rdmDwnlGetCert(MtlsAuth_t *sec, rdkcertselector_h* pthisCertSel) {
    char *certUri = NULL;
    char *certPass = NULL;
    char *engine = NULL;
    char *certFile = NULL;
    rdkcertselectorStatus_t certStat = rdkcertselector_getCert(*pthisCertSel, &certUri, &certPass);
    if (certStat != certselectorOk || certUri == NULL || certPass == NULL) {
        RDMInfo("%s, Failed to retrieve certificate for MTLS\n",  __FUNCTION__);
        rdkcertselector_free(pthisCertSel);
        if(*pthisCertSel == NULL){
            RDMInfo("%s, Cert selector memory free\n", __FUNCTION__);
        }else{
            RDMInfo("%s, Cert selector memory free failed\n", __FUNCTION__);
        }
        return MTLS_CERT_FETCH_FAILURE; // Return error
    }
    certFile = certUri;
    if (strncmp(certFile, FILESCHEME, sizeof(FILESCHEME)-1) == 0) {
        certFile += (sizeof(FILESCHEME)-1); // Remove file scheme prefix
    }
    strncpy(sec->cert_name, certFile, sizeof(sec->cert_name) - 1);
    sec->cert_name[sizeof(sec->cert_name) - 1] = '\0';
    strncpy(sec->key_pas, certPass, sizeof(sec->key_pas) - 1);
    sec->key_pas[sizeof(sec->key_pas) - 1] = '\0';
    engine = rdkcertselector_getEngine(*pthisCertSel);
    if (engine == NULL) {
        sec->engine[0] = '\0';
    }else{
        strncpy(sec->engine, engine, sizeof(sec->engine) - 1);
        sec->engine[sizeof(sec->engine) - 1] = '\0';
    }
    strncpy(sec->cert_type, "P12", sizeof(sec->cert_type) - 1);
    sec->cert_type[sizeof(sec->cert_type) - 1] = '\0';
    RDMInfo("%s, MTLS dynamic/static cert success. cert=%s, type=%s, engine=%s\n", __FUNCTION__, sec->cert_name, sec->cert_type, sec->engine);
    return MTLS_CERT_FETCH_SUCCESS; // Return success
}
#endif

INT32 rdmDwnlIsOCSPEnable(void)
{
    if(fileCheck(EnableOCSPStapling) || fileCheck(EnableOCSP)) {
        return 1;
    }
    return 0;
}

INT32 rdmDwnlDirect(CHAR *pUrl, CHAR *pDwnlPath, CHAR *pPkgName, CHAR *pOut)
{
    VOID *curl = NULL;
    INT32 status = RDM_SUCCESS;
    INT32 curl_ret_code = -1;
    INT32 httpCode;
    INT32 max_dwnl_speed = 0;
    FileDwnl_t file_dwnl;
    MtlsAuth_t sec;

#ifdef LIBRDKCERTSELECTOR
    int cert_ret_code = -1;
    MtlsAuthStatus ret = MTLS_CERT_FETCH_SUCCESS;
#endif
    memset(&file_dwnl, 0, sizeof(FileDwnl_t));

    /* Update the file download details */
    file_dwnl.chunk_dwnl_retry_time = 0;
    strncpy(file_dwnl.url, pUrl, sizeof(file_dwnl.url) - 1);
    file_dwnl.url[sizeof(file_dwnl.url) - 1] = '\0';  // Ensure null termination
    strncpy(file_dwnl.pathname, pDwnlPath, sizeof(file_dwnl.pathname) - 1);
    file_dwnl.pathname[sizeof(file_dwnl.pathname) - 1] = '\0';  // Ensure null termination
    strcat(file_dwnl.pathname, "/");
    strcat(file_dwnl.pathname, pPkgName);
    file_dwnl.pathname[sizeof(file_dwnl.pathname) - 1] = '\0';  // Ensure null termination
    strcpy(pOut, file_dwnl.pathname);

    if(rdmDwnlIsOCSPEnable()) {
        file_dwnl.sslverify = true;
    }
    file_dwnl.pDlData = NULL;
    file_dwnl.pPostFields = NULL;

    /* Update the Certificate */
    curl = doCurlInit();
    if(curl == NULL) {
        RDMError("Failed init curl\n");
        return status;
    }
#ifdef LIBRDKCERTSELECTOR
    static rdkcertselector_h thisCertSel = NULL;
    INT32 mtls_retry = 0;
    RDMInfo("Initializing CertSelector\n");
    if (thisCertSel == NULL)
    {
        const char* certGroup = "MTLS";
        thisCertSel = rdkcertselector_new(NULL, NULL, certGroup);
        if (thisCertSel == NULL) {
             RDMInfo("Cert Selector Initialisation Failed\n");
             return cert_ret_code;
        } else {
             RDMInfo("Cert Selector Initialisation is Successful\n");
        }
    } else {
        RDMInfo("Cert selector already initialized, reusing existing instance\n");
    }
    memset(&sec, '\0', sizeof(MtlsAuth_t));
    do {
        RDMInfo("Fetching MTLS credential for SSR/XCONF\n");
        ret = rdmDwnlGetCert(&sec, &thisCertSel);
        RDMInfo("rdmDwnlGetCert function ret value = %d\n", ret);
        if (ret == MTLS_CERT_FETCH_FAILURE) {
            RDMInfo("MTLS cert Failed ret= %d\n", ret);
            return cert_ret_code;
        } else {
            RDMInfo("MTLS is enable\nMTLS creds for SSR fetched ret=%d\n", ret);
        }
        RDMInfo("Downloading The Package %s \n",file_dwnl.pathname);
        curl_ret_code = doHttpFileDownload(curl, &file_dwnl, &sec,
                                           max_dwnl_speed, NULL, &httpCode);
        RDMInfo("curl_ret_code:%d httpCode:%d\n", curl_ret_code, httpCode);
        if (curl_ret_code != 0) {
            mtls_retry++;
            if (mtls_retry >= RDM_DWNLD_MAX_RETRIES) {
                RDMError("Max MTLS retry attempts reached (%d). Aborting.\n", RDM_DWNLD_MAX_RETRIES);
                break;
            }
       }
    }while (rdkcertselector_setCurlStatus(thisCertSel, curl_ret_code, file_dwnl.url) == TRY_ANOTHER);
              
    if(curl_ret_code && httpCode != 200) {
            RDMError("Download failed\n");
            status = RDM_FAILURE;
    }
    if (curl != NULL) {
        RDMInfo("Stopping Curl Download\n");
        doStopDownload(curl);
        curl = NULL;
    }
    return status;
#endif
    RDMInfo("Downloading The Package %s \n",file_dwnl.pathname);
    t2ValNotify("RDM_INFO_package_download", file_dwnl.pathname);

    curl_ret_code = doHttpFileDownload(curl, &file_dwnl, &sec,
                                       max_dwnl_speed, NULL, &httpCode);

    RDMInfo("curl_ret_code:%d httpCode:%d\n", curl_ret_code, httpCode);

    if(curl_ret_code && httpCode != 200) {
        RDMError("Direct Download failed\n");
	      t2CountNotify("RDM_ERR_package_failed", 1);
        status = RDM_FAILURE;
    }

    doStopDownload(curl);

    return status;
}

#ifdef RDM_ENABLE_CODEBIG
INT32 rdmDwnlGetCodebigUrl(char *pUrl)
{
    return RDM_FAILURE;
}

INT32 rdmDwnlCodebig(CHAR *pUrl, CHAR *pDwnlPath, CHAR *pPkgName, CHAR *pOut)
{
    VOID *curl                     = NULL;
    INT32 server_type              = HTTP_SSR_CODEBIG;
    INT32 status                   = RDM_SUCCESS;
    INT32 httpCode                 = 0;
    INT32 curl_ret_code            = -1;
    INT32 signFailed               = 1;
    CHAR oAuthHeader[BIG_BUF_LEN]  = "Authorization: OAuth realm=\"\", ";
    CHAR headerInfoFile[256];
    FileDwnl_t file_dwnl;
    size_t len                     = 0;

    memset(&file_dwnl, 0, sizeof(FileDwnl_t));
    /* Update the file download details */
    file_dwnl.chunk_dwnl_retry_time = 0;

    if( (signFailed=doCodeBigSigning(server_type, pUrl, file_dwnl.url, sizeof(file_dwnl.url),
                                        oAuthHeader, sizeof(oAuthHeader) )) == 0 )
    {
        strncpy(file_dwnl.pathname, pDwnlPath, sizeof(file_dwnl.pathname) - 1);
	file_dwnl.pathname[sizeof(file_dwnl.pathname) - 1] = '\0';  // Ensure null termination
        strcat(file_dwnl.pathname, "/");
        strcat(file_dwnl.pathname, pPkgName);
	file_dwnl.pathname[sizeof(file_dwnl.pathname) - 1] = '\0';  // Ensure null termination
        strcpy(pOut, file_dwnl.pathname);
        file_dwnl.pDlData = NULL;
        file_dwnl.pHeaderData = oAuthHeader;
        file_dwnl.pPostFields = NULL;
        snprintf(headerInfoFile, sizeof(headerInfoFile), "%s.header", file_dwnl.pathname);

        curl = doCurlInit();
        if(curl == NULL) {
            RDMError("CurlInit Failed\n");
            return status;
        }

        curl_ret_code = doAuthHttpFileDownload(curl, &file_dwnl, &httpCode);

        RDMInfo("curl_ret_code:%d httpCode:%d len %d server_type %d\n", curl_ret_code, httpCode,len,server_type);

        if(curl_ret_code && httpCode != 200) {
            RDMError("Codebig Download failed\n");
	          t2CountNotify("RDM_ERR_package_failed", 1);
            status = RDM_FAILURE;
        }

        doStopDownload(curl);
    }
    else{
        RDMError("signing the URL has been failed [%d]\n",signFailed);
        status = RDM_FAILURE;
    }

    return status;
}
#endif

INT32 rdmDwnlApplication(CHAR *pUrl, CHAR *pDwnlPath, CHAR *pPkgName, CHAR *pOut)
{
    INT32 status = RDM_SUCCESS;
    INT32 retry_count = 0;

    /* Check for direct download is blocked */
    if(!(rdmDwnlIsBlocked(DIRECT_BLOCK_FILENAME, DIRECT_BLOCK_TIME))) {
        while(retry_count <= RDM_DWNLD_MAX_RETRIES) {
	    if(retry_count == 1) {
	        RDMInfo("RDM applicationDownload Failed : Attempting retry = %d\n", retry_count);
	        sleep(10);
	    }
	    else if(retry_count == RDM_DWNLD_MAX_RETRIES) {
	        RDMInfo("RDM applicationDownload Failed : Attempting retry = %d\n", retry_count);
	        sleep(30);
	    }
            status = rdmDwnlDirect(pUrl, pDwnlPath, pPkgName, pOut);
            if(status == RDM_SUCCESS) {
	        return status;
	    }
            else {
                RDMWarn("RDM applicationDownload : Direct App download failed\n");
	        retry_count++;
	    }
        }
    }
    else {
        RDMInfo("RDM applicationDownload: Direct download is blocked\n");
        t2CountNotify("RDM_INFO_DirectBlocked", 1);
        status = RDM_FAILURE;
    }
#ifdef RDM_ENABLE_CODEBIG
        RDMWarn("Retrying to RDM Application download via CodeBig server\n");
	      t2CountNotify("RDM_INFO_CodebigRetry", 1);
	      retry_count = 0;

        /* Check for codebig download is blocked */
        if(!(rdmDwnlIsBlocked(CB_BLOCK_FILENAME, CB_BLOCK_TIME))) {
	    while(retry_count <=1) {
	        if(retry_count == 1) {
		    RDMInfo("applicationDownload: Attempting retry = %d for CodeBig\n", retry_count);
		    sleep(10);
	        }
                status = rdmDwnlCodebig(pUrl, pDwnlPath, pPkgName, pOut);
	        if(status == RDM_SUCCESS) {
		    //If Direct Failed and CodeBig succeeded, Block Direct for 24hrs
		    int fd_dbf = open(DIRECT_BLOCK_FILENAME, O_CREAT|O_EXCL);
		    if (fd_dbf != -1) {
                        close(fd_dbf);  // Close the file handle immediately, we just want to create the file
                    }
		    RDMInfo("Use Codebig connection and Block Direct for 24 hrs\n");
	            t2CountNotify("RDM_INFO_CodebigConnection", 1);
		    return status;
	        }
	        else {
                    RDMError("applicationDownload: Codebig App download failed\n");
		    retry_count++;
		    //If CodeBig also fails, block CodeBig for 30 mins
		    int fd_cbf = open(CB_BLOCK_FILENAME, O_CREAT|O_EXCL);
		    if (fd_cbf != -1) {
                        close(fd_cbf);  // Close the file handle immediately, we just want to create the file
                    }
		    RDMInfo("No App download attempts allowed, Blocking Codebig for 30 mins\n");
	        }
            }
        }
        else {
            RDMInfo("applicationDownload: CodeBig download is blocked\n");
	          t2CountNotify("RDM_INFO_CodebigBlocked", 1);
            status = RDM_FAILURE;
        }
#endif
    return status;
}

/* Description: Use For parsing jsonrpc token
 * @param: token : Pointer to receive token
 * @param: pJsonStr : Full json data
 * @param: token_size : token buffer size
 * @return int:
 * */
INT32 rdmJRPCTokenData(CHAR *token, CHAR *pJsonStr, UINT32 token_size)
{
    JSON *pJson = NULL;
    INT8  status[8];
    INT32 ret = -1;

    if (token == NULL || pJsonStr == NULL) {
        RDMError( "Parameter is NULL\n");
        return ret;
    }
    *status = 0;
    pJson = ParseJsonStr(pJsonStr);

    if( pJson != NULL ) {
        GetJsonVal(pJson, "token", token, token_size);
        GetJsonVal(pJson, "success", status, sizeof(status));
        RDMInfo( "ParseJson string status: %s\n", status);
        FreeJson(pJson);
        if (0 == strncmp(status, "true", 4) && 0 != *token) {
            ret = 0;
        }
    }
    return ret;
}

/* function rdmMemDLAlloc - allocates a memory block and fills in data structure used for curl
                          memory downloads.

Usage: int rdmMemDLAlloc <DownloadData *pDwnData>

pDwnData - pointer to a DownloadData structure to hold download allocation data.

RETURN - 0 on success, non-zero otherwise

Function Notes - The caller is responsible for freeing the dynamically allocated
memory pointed to by the pvOut member of the DownloadData structure pointer.
*/
INT32 rdmMemDLAlloc(VOID *pvDwnData, size_t szDataSize)
{
    VOID *ptr;
    INT32 iRet = RDM_FAILURE;

    DownloadData *pDwnData = (DownloadData *)pvDwnData;

    if(pDwnData != NULL) {
        pDwnData->datasize = 0;
        ptr = malloc(szDataSize);
        pDwnData->pvOut = ptr;
        pDwnData->datasize = 0;

        if(ptr != NULL) {
            pDwnData->memsize = szDataSize;
            *(CHAR *)ptr = 0;
            iRet = RDM_SUCCESS;
        }
        else {
            pDwnData->memsize = 0;
            RDMError( "Failed to allocate memory for XCONF download\n" );
        }
    }

    return iRet;
}

VOID rdmMemDLFree(VOID *pvDwnData)
{
    DownloadData *pDwnData = (DownloadData *)pvDwnData;

    if(pDwnData->pvOut) {
        free(pDwnData->pvOut);
    }
    memset(pvDwnData, 0, sizeof(DownloadData));
}

INT32 rdmExecuteScript(RDMAPPDetails *pRdmAppDet)
{
    char *pkgjson_path = NULL;
    char *script_path = NULL;
    CHAR script_value[RDM_APP_PATH_LEN] = {0};

    if (pRdmAppDet == NULL) {
        RDMError("Invalid Parameter\n");
        return RDM_FAILURE;
    }

    if (asprintf(&pkgjson_path, "%s/package.json", pRdmAppDet->app_dwnl_path) < 0 ||
        pkgjson_path == NULL) {
        RDMError("Failed to allocate package.json path\n");
        return RDM_FAILURE;
    }

    if (fileCheck(pkgjson_path)) {
        RDMInfo("%s found in %s\n", pkgjson_path, pRdmAppDet->app_dwnl_path);
    } else {
        RDMInfo("No %s found in %s\n", pkgjson_path, pRdmAppDet->app_dwnl_path);
		free(pkgjson_path);
        return RDM_FAILURE;
    }

    getStringValueFromFile(pkgjson_path, "\":, ", "installScript", script_value);
    if (script_value[0] == '\0') {
        RDMInfo("No 'installScript' entry in %s; skipping JSON-based post-install\n", pkgjson_path);
        free(pkgjson_path);
        return RDM_SUCCESS;
    }

    if (asprintf(&script_path, "%s/%s", pRdmAppDet->app_home, script_value) < 0 ||
        script_path == NULL) {
        RDMError("Failed to allocate install script path\n");
        free(pkgjson_path);
        return RDM_FAILURE;
    }

    if (!fileCheck(script_path)) {
        RDMError("Install script not found: %s\n", script_path);
        free(pkgjson_path);
        free(script_path);
        return RDM_FAILURE;
    }

    RDMInfo("Executing post-install script: %s\n", script_path);

    INT32 ret = v_secure_system("/bin/sh '%s'", script_path);

    free(script_path);
    free(pkgjson_path);
    if (ret != 0) {
        RDMError("Post-install script failed with status %d: %s\n", ret, script_path);
        return RDM_FAILURE;
    }

    return RDM_SUCCESS;
}

INT32 rdmDwnlRunPostScripts(RDMAPPDetails *pRdmAppDet, INT32 versioned_app)
{
    CHAR tmp_file[RDM_APP_PATH_LEN];
    CHAR filePath[RDM_APP_PATH_LEN];
    DIR *dir;
    struct dirent *entry;

    RDMInfo("RDM Start Post script Execution dwnl_path = %s\n", pRdmAppDet->app_dwnl_path);

    INT32 status = rdmExecuteScript(pRdmAppDet);
    if (status == RDM_SUCCESS) {
        RDMInfo("Post-Install Script Execution successful....");
        return RDM_SUCCESS;
    } else {
        RDMWarn("Install script not found in package.json...checking in directory");
    }

    strncpy(tmp_file, pRdmAppDet->app_home, RDM_APP_PATH_LEN - 1);
    tmp_file[sizeof(tmp_file) - 1] = '\0';
    strcat(tmp_file, RDM_POSTSCRIPT_PATH);
    tmp_file[sizeof(tmp_file) - 1] = '\0';  // Ensure null termination

    dir = opendir(tmp_file);

    if (dir == NULL) {
	if (versioned_app) {
            RDMInfo("No Install Script found \n");
	    return RDM_SUCCESS;
	} else {
            RDMError("Error opening directory: %s\n", tmp_file);
            return RDM_FAILURE;
	}
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0  ||
            strcmp(entry->d_name, "..") == 0 ||
            entry->d_type == DT_DIR) {
            continue;
        }
        if(strcmp(getExtension(entry->d_name), "sh")) {
            continue;
        }

        strncpy(filePath, tmp_file, RDM_APP_PATH_LEN - 1);
	    filePath[sizeof(filePath) - 1] = '\0';  // Ensure null termination
        strcat(filePath, entry->d_name);
	    filePath[sizeof(filePath) - 1] = '\0';  // Ensure null termination

        RDMInfo("RDM Post script Execution %s\n", filePath);

	INT32 cmdStatus = v_secure_system("/bin/sh '%s'", filePath);
	if (cmdStatus != 0) {
	    RDMError("Post script execution failed for %s with status %d\n", filePath, cmdStatus);
	}
    }

    closedir(dir);

    return RDM_SUCCESS;
}

VOID rdmDwnlUnInstallApp(CHAR *pDwnlPath, CHAR *pAppPath)
{
    rdmDwnlCleanUp(pDwnlPath);
    rdmDwnlAppCleanUp(pAppPath);
}

/** @brief This Function updates the manifest file.
 *
 *  @param[in]   pInManifest   input manifest file path
 *  @param[out]  pOutManifest  output manifest file path
 *  @param[in]   update        path prefix
 *
 *  @return  Returns status of the function.
 *  @retval  Returns RDM_SUCCESS on success, RDM_FAILURE otherwise.
 */
INT32 rdmDwnlUpdateManifest(CHAR *pInManifest,
                            CHAR *pOutManifest,
                            CHAR *update,
                            CHAR *padding)
{
    FILE *fpin;
    FILE *fpout;
    INT32 status = RDM_SUCCESS;
    CHAR *buff;

    RDMInfo("DEBUG: Listing contents of %s\n", RDM_CPEMANIFEST_PATH);
    DIR *dbg_dir = opendir(RDM_CPEMANIFEST_PATH);
    if (dbg_dir) {
        struct dirent *dbg_entry;
        while ((dbg_entry = readdir(dbg_dir)) != NULL) {
            if (strstr(dbg_entry->d_name, "cpemanifest")) {
                RDMInfo("DEBUG:   found: %s\n", dbg_entry->d_name);
	    }
        }
        closedir(dbg_dir);
    } else {
        RDMError("DEBUG: Cannot open dir %s\n", RDM_CPEMANIFEST_PATH);
    }
    fpin = fopen(pInManifest, "r");
    if(fpin == NULL) {
        RDMError("Unable to open input file: %s\n", pInManifest);
        return RDM_FAILURE;
    }

    fpout = fopen(pOutManifest, "w");
    if(fpin == NULL) {
        RDMError("Unable to open output file: %s\n", pOutManifest);
        status = RDM_FAILURE;
        goto error;
    }

    buff = calloc(MAX_BUFF_SIZE, 1);
    if(buff == NULL) {
        RDMError("Failed to allocate memory\n");
        status = RDM_FAILURE;
        goto error;
    }

    while (fgets(buff, MAX_BUFF_SIZE, fpin)) {
        fprintf(fpout, "%s/%s", update, buff);
    }

    fprintf(fpout, "%s/pkg_padding", padding);
error:
    if(fpin) {
        fclose(fpin);
    }
    if(fpout) {
        fclose(fpout);
    }
    if(buff) {
        free(buff);
    }

    return status;
}

static INT32 rdmDwnlReadSigFile(CHAR *sig_file, CHAR *pout)
{
    INT32 status = RDM_SUCCESS;
    FILE *fp = fopen(sig_file, "rb");
    INT32 len = 0;

    if(fp == NULL) {
        RDMError("Failed to open Signature file: %s\n", sig_file);
        return RDM_FAILURE;
    }

    // Since KMS is adding 6 Bytes of Header, need to remove this before validation
    // KMS Header Removal from the Signature
    fseek(fp, RDM_KMS_SKIP_BYTES, SEEK_CUR);

    len = fread(pout, 1, RDM_SIGFILE_LEN, fp);
    if(len == 0) {
        RDMError("Failed to read data from signature file: %s\n", sig_file);
        status = RDM_FAILURE;
    }

    fclose(fp);

    return status;
}

INT32 rdmDwnlUpdateManifest_versionedapp(CHAR *pInManifest,
                            CHAR *pOutManifest,
                            CHAR *update,
                            CHAR *padding)
{
    FILE *fpin = NULL;
    FILE *fpout = NULL;
    INT32 status = RDM_SUCCESS;
    CHAR *buff = NULL;

    fpin = fopen(pInManifest, "r");
    if(fpin == NULL) {
        RDMError("Unable to open input file: %s\n", pInManifest);
	status = RDM_FAILURE;
	goto error;
    }

    fpout = fopen(pOutManifest, "w");
    if(fpin == NULL) {
        RDMError("Unable to open output file: %s\n", pOutManifest);
        status = RDM_FAILURE;
        goto error;
    }

    buff = calloc(MAX_BUFF_SIZE, 1);
    if(buff == NULL) {
        RDMError("Failed to allocate memory\n");
        status = RDM_FAILURE;
        goto error;
    }

    while (fgets(buff, MAX_BUFF_SIZE, fpin)) {
	RDMInfo("buff = %s\n", buff);
	if(strncmp(buff, "package.json", strlen("package.json")) == 0)
	    fprintf(fpout, "%s/%s", padding, buff);
	else
	    fprintf(fpout, "%s/%s", update, buff);
    }
    fprintf(fpout, "%s/pkg_padding", padding);
error:
    if(fpin) {
        fclose(fpin);
    }
    if(fpout) {
        fclose(fpout);
    }
    if(buff) {
        free(buff);
    }

    return status;
}

/** @brief This Function updates the manifest file.
 *
 *  @param[in]   pRdmAppDet    App details
 *  @param[in]   pVer          Version details
 *
 *  @return  Returns status of the function.
 *  @retval  Returns RDM_SUCCESS on success, RDM_FAILURE otherwise.
 */
INT32 rdmDwnlValidation(RDMAPPDetails *pRdmAppDet, CHAR *pVer)
{
    CHAR pkg_file[RDM_APP_PATH_LEN];
    CHAR tmp_file[RDM_APP_PATH_LEN];
    CHAR out_file[RDM_APP_PATH_LEN];
    CHAR dwnl_path[RDM_APP_PATH_LEN];
    CHAR app_home[RDM_APP_PATH_LEN];
    INT32 status = RDM_SUCCESS;
    INT32 ssl_status = 0;
    INT32 outputMsgLen = REPLY_MSG_LEN;
    CHAR *out_buf = calloc(RDM_SIGFILE_LEN, 1);

    if(out_buf == NULL) {
        RDMError("Failed to allocate memory\n");
        return RDM_FAILURE;
    }

    strncpy(dwnl_path, pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN -1);
    dwnl_path[sizeof(dwnl_path) - 1] = '\0';  // Ensure null termination
    if(pVer) {
        strcat(dwnl_path, "/v");
        strcat(dwnl_path, pVer);
	dwnl_path[sizeof(dwnl_path) - 1] = '\0';  // Ensure null termination
    }
    strncpy(app_home, pRdmAppDet->app_home, RDM_APP_PATH_LEN - 1);
    app_home[sizeof(app_home) - 1] = '\0';  // Ensure null termination
    if(pVer) {
        strcat(app_home, "/v");
        strcat(app_home, pVer);
        strcat(app_home, "/package");
	app_home[sizeof(app_home) - 1] = '\0';  // Ensure null termination
    }

    if(pRdmAppDet->is_versioned_app) {
        if(findPFile(dwnl_path, "*.sig", pkg_file)) {
            /* Read the signature file */
            status = rdmDwnlReadSigFile(pkg_file, out_buf);
            if(status) {
                RDMError("Failed to read Signature file: %s\n", pkg_file);
		if(out_buf)
                    free(out_buf);
		return status;
            }
	}
    } else {
        if(findPFile(dwnl_path, "*-pkg.sig", pkg_file)) {
            /* Read the signature file */
            status = rdmDwnlReadSigFile(pkg_file, out_buf);
            if(status) {
                RDMError("Failed to read Signature file: %s\n", pkg_file);
		if(out_buf)
                    free(out_buf);
                return status;
            }
	}
    }

    if(pRdmAppDet->sig_type == RDM_SIG_TYPE_KMS) {
        status = rdmDecryptKey(RDM_KMS_PUB_KEY);

        RDMInfo("Validate the Package\n");

	if (pRdmAppDet->is_versioned_app) {
	    RDMInfo("Generating cpemanifest for RDM Bundle in %s\n", dwnl_path);
            strncpy(tmp_file, dwnl_path, RDM_APP_PATH_LEN -1);
	    tmp_file[sizeof(tmp_file) - 1] = '\0';  // Ensure null termination
	    strcat(tmp_file, "/");
	    strcat(tmp_file, "pkg_cpemanifest");
	    tmp_file[sizeof(tmp_file) - 1] = '\0';  // Ensure null termination

	    strncpy(out_file, dwnl_path, RDM_APP_PATH_LEN -1);
	    out_file[sizeof(out_file) - 1] = '\0';  // Ensure null termination
	    strcat(out_file, "/");
	    strcat(out_file, "pkg_cpemanifest.updated");
	    out_file[sizeof(out_file) - 1] = '\0';  // Ensure null termination

	    RDMInfo("app_home = %s\n", app_home);
	    RDMInfo("dwnl_path = %s\n", dwnl_path);
            status = rdmDwnlUpdateManifest_versionedapp(tmp_file, out_file,
                                       app_home,
                                       dwnl_path);
	} else {
	    RDMInfo("Generating cpemanifest for RDM Apps in %s\n", app_home);
            strncpy(tmp_file, RDM_CPEMANIFEST_PATH, RDM_APP_PATH_LEN - 1);
	    tmp_file[sizeof(tmp_file) - 1] = '\0';  // Ensure null termination
            strcat(tmp_file, "/");
            strcat(tmp_file, pRdmAppDet->app_name);
            strcat(tmp_file, "_cpemanifest");
	    tmp_file[sizeof(tmp_file) - 1] = '\0';  // Ensure null termination

	    RDMInfo("DEBUG: RDM_CPEMANIFEST_PATH=[%s]\n", RDM_CPEMANIFEST_PATH);
	    RDMInfo("DEBUG: app_name=[%s]\n", pRdmAppDet->app_name);
	    RDMInfo("DEBUG: tmp_file (input manifest)=[%s] exists=%d\n", tmp_file, fileCheck(tmp_file));

            strncpy(out_file, app_home, RDM_APP_PATH_LEN);
	    out_file[sizeof(out_file) - 1] = '\0';  // Ensure null termination
            strcat(out_file, "/");
            strcat(out_file, pRdmAppDet->app_name);
            strcat(out_file, "_cpemanifest");
	    out_file[sizeof(out_file) - 1] = '\0';  // Ensure null termination

	    RDMInfo("DEBUG: out_file (output manifest)=[%s]\n", out_file);
	    RDMInfo("DEBUG: dwnl_path=[%s]\n", dwnl_path);

            status = rdmDwnlUpdateManifest(tmp_file, out_file,
                                       app_home,
                                       dwnl_path);
       }
       if(status) {
           RDMWarn("Failed to process manifest file\n");
       } else {

                rdmInitSslLib();

                ssl_status = rdmOpensslRsafileSignatureVerify(out_file, -1,
                                                          out_buf,
                                                          RDM_KMS_PUB_KEY, tmp_file,
                                                          &outputMsgLen);

                if (ssl_status == retcode_success) {
                    RDMInfo("RSA Signature Validation Success\n");
		    t2CountNotify("RDM_INFO_rsa_valid_signature", 1);
                } else {
                    RDMError("RSA Signature Verification Failed\n");
		    t2CountNotify("RDM_INFO_rsa_verify_signature_failure", 1);
                    status = RDM_FAILURE;
                }
       }
    }
    else if(pRdmAppDet->sig_type == RDM_SIG_TYPE_OPENSSL) {
      /*In the script, there is no call for this sig type. So not handled*/
    }
    else {
        RDMError("Unknown Signature Type\n");
        status = RDM_FAILURE;
    }

    if(out_buf) {
        free(out_buf);
    }

    return status;
}

/** @brief This Function updates the list of manifest names.
*   @param[in]   pAppsInManifest    Manifest list
 *  @param[in]   numOfApps          Number of Apps Installed
 *
 *  @return  Returns status of the function.
 *  @retval  Returns RDM_SUCCESS on success, RDM_FAILURE otherwise.
 **/
INT32 rdmGetManifestApps(CHAR **pAppsInManifest, INT32 *numOfApps)
{
    INT32 ret           = RDM_SUCCESS;
    INT32 len           = 0;
    INT32 idx           = 0;

    ret = rdmJSONGetLen(RDM_MANIFEST, &len);

    if(ret || len == 0) {
        RDMError("Invalid json file\n");
        goto error;
    }

    do{

        pAppsInManifest[idx] = (CHAR *)malloc(RDM_APPNAME_LEN);

        if(pAppsInManifest[idx] == NULL){
            RDMError("pAppsInManifest malloc failed");
            ret = RDM_FAILURE;
            break;
        }

        memset(pAppsInManifest[idx], 0, RDM_APPNAME_LEN);

        ret = rdmJSONGetAppNames(idx,pAppsInManifest[idx]);

        if ( ret == RDM_SUCCESS ){
            RDMInfo("Packages listed in manifest %s idx %d\n",pAppsInManifest[idx], idx);
        }
        else{
            RDMError("rdmJSONGetAppNames Failed\n");
            break;
        }

        idx += 1;

        if(idx >= len) {
            RDMWarn("Reached end of manifest file\n");
            break;
        }

    }while(1);

    *numOfApps = idx;

error:
    return ret;
}


/** @brief This Function uninstalls the apps that not present in the manifest
 *
 *  @return  Returns status of the function.
 *  @retval  Returns RDM_SUCCESS on success, RDM_FAILURE otherwise.
 **/
INT32 rdmUnInstallApps(RDMHandle *prdmHandle, INT32  is_broadband)
{
    CHAR *app_manifests[RDM_TMP_LEN_64]  = {0};
    CHAR *app_installed[RDM_TMP_LEN_64]  = {0};
    INT32 status                         = RDM_SUCCESS;
    INT32 numOfAppsInstalled             = 0;
    INT32 numOfAppsManifest              = 0;
    INT32 isAppUninstalled               = 0;
    bool  isFileFound                    = 0;
    RDMAPPDetails *pApp_det              = NULL;
    INT32 ret                            = RDM_SUCCESS;
    CHAR  pkg_file[RDM_APP_PATH_LEN]     = {0};
    CHAR   rfc_app[256]                  = {0};
    INT32 app_rfc_status                 = 0;
    INT32 removedDownloads               = 0;
    CHAR infoPath[RDM_APP_PATH_LEN]      = {0};

    RDMInfo("Uninstall the old packages that are not listed in current manifest\n");

    pApp_det = (RDMAPPDetails *)malloc(sizeof(RDMAPPDetails));
    if(pApp_det == NULL) {
        RDMError("Failed to allocate memory\n");
        return RDM_FAILURE;
    }

    status = rdmGetManifestApps(app_manifests, &numOfAppsManifest);

    if(status == RDM_SUCCESS){

        status = rdmListDirectory(APP_MOUNT_PATH, app_installed, &numOfAppsInstalled);

        if ( status == RDM_SUCCESS ){

            for ( INT32 index = 0 ; index < numOfAppsInstalled; index++){

                if( ( strcmp(app_installed[index],"etc") == 0 )  || \
                    (strcmp(app_installed[index],"rdm")  == 0 )  || \
                    (strcmp(app_installed[index],"..")   == 0 )  || \
                    (strcmp(app_installed[index],".")    == 0 ) ) {
                    RDMInfo("Path is [%s]. Not required to check, so skipping it\n",app_installed[index]);
                    continue;
                }
                else{
                    CHAR path[RDM_APPNAME_LEN]        = { 0 };
                    CHAR searchFile[RDM_TMP_LEN_64]   = { 0 };

                    snprintf(path, sizeof(path), "%s/%s/", APP_MOUNT_PATH, app_installed[index]);
                    snprintf(searchFile, sizeof(searchFile), "%s_cpemanifest", app_installed[index]);

                    isFileFound = findPFile(path, searchFile, pkg_file);

                    if(isFileFound){

                        INT32 isFoundinManifest = 0;

                        isFoundinManifest = isDataInList(app_manifests,app_installed[index],numOfAppsManifest);

                        if(isFoundinManifest){

                            memset(pApp_det, 0, sizeof(RDMAPPDetails));

                            /* Parse the JSON file and get the app details */
                            ret = rdmJSONGetAppDetName(app_installed[index], pApp_det);
                            if(ret) {
                                RDMInfo("Failed to get app details with status %d\n", ret);
                            }

                            if(is_broadband) {
                                snprintf(pApp_det->app_dwnl_info, RDM_APP_PATH_LEN, "%s", RDM_DL_INFO_BR);
                            }
                            else {
                                snprintf(pApp_det->app_dwnl_info, RDM_APP_PATH_LEN, "%s", RDM_DL_INFO);
                            }

                            if(pApp_det->dwld_on_demand) {
                                RDMInfo("dwld_on_demand set to yes!!! Check RFC value of the APP to be downloaded\n");
                                if(strcmp(pApp_det->dwld_method_controller, "RFC") != 0) {
				    isAppUninstalled = 1;
                                    RDMInfo("Packages to be uninstalled %s \n",app_installed[index]);
                                    rdmDwnlAppCleanUp(path);
                                    rdmRemvDwnlAppInfo(app_installed[index], pApp_det->app_dwnl_info);
                                    continue;
                                }
                                else {
                                    RDMInfo("App is supported using RFC method for download, checking RFC status\n");
                                    snprintf(rfc_app, sizeof(rfc_app),
                                                  "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.%s.Enable",
                                                   pApp_det->app_name);

                                    ret = rdmRbusGetRfc(prdmHandle->pRbusHandle,
                                                   rfc_app,
                                                   &app_rfc_status);


                                    if (ret == RDM_SUCCESS && app_rfc_status == 0)
                                    {
                                        /* RFC says feature is disabled → delete app */
                                        RDMInfo("APP RFC disabled, Deleting App  name=%s\n", pApp_det->app_name);
                                        isAppUninstalled = 1;
                                        RDMInfo("Packages to be uninstalled %s \n",app_installed[index]);
                                        rdmDwnlAppCleanUp(path);
                                        rdmRemvDwnlAppInfo(app_installed[index], pApp_det->app_dwnl_info);
                                        continue;
                                   }
                               }								
                            }
                            RDMInfo("Installed App %s is found in Manifest, so Skipping to next... \n", app_installed[index]);
                        }
                        else{
			    isAppUninstalled = 1;
                            RDMInfo("Packages to be uninstalled %s \n",app_installed[index]);
                            rdmDwnlAppCleanUp(path);
                            rdmRemvDwnlAppInfo(app_installed[index], pApp_det->app_dwnl_info);
                            continue;
                        }
                    }
                    else{

                        isFileFound = findPFile(path,"*_package.json", pkg_file);

                        if(isFileFound){

                            INT32 isFoundinManifest = 0;

                            isFoundinManifest = isDataInList(app_manifests,app_installed[index],numOfAppsManifest);

                            if(isFoundinManifest){
                                memset(pApp_det, 0, sizeof(RDMAPPDetails));

                                /* Parse the JSON file and get the app details */
                                ret = rdmJSONGetAppDetName(app_installed[index], pApp_det);

                                if(ret) {
                                    RDMInfo("Failed to get app details with status %d\n", ret);
                                }

                                if(pApp_det->dwld_on_demand) {
                                    RDMInfo("dwld_on_demand set to yes!!! Check RFC value of the APP to be downloaded\n");
                                    if(strcmp(pApp_det->dwld_method_controller, "RFC") != 0) {
					isAppUninstalled = 1;
                                        RDMInfo("Packages to be uninstalled %s \n",app_installed[index]);
                                        rdmDwnlAppCleanUp(path);
                                        rdmRemvDwnlAppInfo(app_installed[index], pApp_det->app_dwnl_info);
                                        continue;
                                    }
                                    else {
                                        RDMInfo("App is supported using RFC method for download, checking RFC status\n");
                                        snprintf(rfc_app, sizeof(rfc_app),
                                                   "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.%s.Enable",
                                                    pApp_det->app_name);
 
                                        ret = rdmRbusGetRfc(prdmHandle->pRbusHandle,
                                                    rfc_app,
                                                    &app_rfc_status);
 
 
                                        if (ret == RDM_SUCCESS && app_rfc_status == 0)
                                        {
                                            /* RFC says feature is disabled → delete app */
                                            RDMInfo("APP RFC disabled, Deleting App  name=%s\n", pApp_det->app_name);
                                            isAppUninstalled = 1;
                                            RDMInfo("Packages to be uninstalled %s \n",app_installed[index]);
                                            rdmDwnlAppCleanUp(path);
                                            rdmRemvDwnlAppInfo(app_installed[index], pApp_det->app_dwnl_info);
                                            continue;
                                        }
                                    }
                                }
                                RDMInfo("Installed App %s is found in Manifest, so Skipping to next... \n", app_installed[index]);
                            }
                        }
		    }
                }
            }
	    if ( isAppUninstalled == 0 ) {
	    RDMInfo("CPE contains only the packages that are listed in manifest. No old packages were found to uninstall\n");
	    }
        }
        else{
            RDMError("Failed to list the %s directory\n",APP_MOUNT_PATH);
        }
    }
    else{
        RDMError("Failed while reading from the JSON file\n");
        goto error;
    }

    if (is_broadband) {
        snprintf(infoPath, sizeof(infoPath), "%s", RDM_DL_INFO_BR);
    } else {
        snprintf(infoPath, sizeof(infoPath), "%s", RDM_DL_INFO);
    }

    removedDownloads = rdmDeleteStalePackages(infoPath, app_manifests, numOfAppsManifest);
    if (removedDownloads > 0) {
        RDMInfo("Removed %d stale packages from downloads not in manifest\n", removedDownloads);
    } else {
        RDMInfo("No stale packages found in downloads beyond installed apps\n");
    }

error:
   for ( INT32 index = 0; index < numOfAppsInstalled; index++ ){
       if ( app_installed[index] ){
           free(app_installed[index]);
       }
   }

   for ( INT32 index = 0; index < numOfAppsManifest; index++ ){
       if ( app_manifests[index] ){
           free(app_manifests[index]);
       }
   }
   free(pApp_det);

   return status;
}

INT32 rdmUpdateAppDetails(RDMHandle *prdmHandle,
                          RDMAPPDetails *pRdmAppDet,
                          INT32 is_broadband)
{
    INT32 ret = RDM_SUCCESS;

    if(is_broadband) {
        snprintf(pRdmAppDet->app_mnt, RDM_APP_PATH_LEN, "%s", APPLN_HOME_PATH_DEF);
        snprintf(pRdmAppDet->app_home, RDM_APP_PATH_LEN, "%s/%s", APPLN_HOME_PATH_DEF, pRdmAppDet->app_name);
        snprintf(pRdmAppDet->app_dwnl_home, RDM_APP_PATH_LEN, "%s/rdm/downloads", APPLN_HOME_PATH_DEF);
        snprintf(pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN, "%s/rdm/downloads/%s", APPLN_HOME_PATH_DEF, pRdmAppDet->app_name);
        snprintf(pRdmAppDet->app_dwnl_info, RDM_APP_PATH_LEN, "%s", RDM_DL_INFO_BR);
    }
    else {
        snprintf(pRdmAppDet->app_mnt, RDM_APP_PATH_LEN, "%s", APP_MOUNT_PATH);
        snprintf(pRdmAppDet->app_home, RDM_APP_PATH_LEN, "%s/%s", APP_MOUNT_PATH, pRdmAppDet->app_name);
        snprintf(pRdmAppDet->app_dwnl_home, RDM_APP_PATH_LEN, "%s/rdm/downloads", APP_MOUNT_PATH);
        snprintf(pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN, "%s/rdm/downloads/%s", APP_MOUNT_PATH, pRdmAppDet->app_name);
        snprintf(pRdmAppDet->app_dwnl_info, RDM_APP_PATH_LEN, "%s", RDM_DL_INFO);
    }

    ret = rdmRbusGetRfc(prdmHandle->pRbusHandle,
                        RDM_RFC_URL,
                        pRdmAppDet->app_dwnl_url);

    if (ret != RDM_SUCCESS) {
        RDMWarn("Failed to Get RDM url from rbus\n");
        memset(pRdmAppDet->app_dwnl_url, 0, sizeof(pRdmAppDet->app_dwnl_url));
    }


#ifdef RDM_ENABLE_CODEBIG
    ret = rdmRbusGetRfc(prdmHandle->pRbusHandle,
                        RDM_CODEBIG_STATUS,
                        &pRdmAppDet->is_codebig);

    if (ret != RDM_SUCCESS) {
        RDMWarn("Failed to Get codebig flag from rbus\n");
        pRdmAppDet->is_codebig = 0;
    }
#endif

    ret = rdmDwnlUpdateURL(pRdmAppDet->app_dwnl_url);
    if (ret != RDM_SUCCESS) {
        rdmIARMEvntSendPayload(pRdmAppDet->pkg_name,
                               pRdmAppDet->pkg_ver,
                               pRdmAppDet->app_home,
                               RDM_PKG_INVALID_INPUT);
        RDMError("Failed to Get download URL\n");
        return RDM_FAILURE;
    }

    strcat(pRdmAppDet->app_dwnl_url, "/");
    strcat(pRdmAppDet->app_dwnl_url, pRdmAppDet->pkg_name);

    return RDM_SUCCESS;
}

VOID rdmPrintAppDetails(RDMAPPDetails *pRdmAppDet)
{
    RDMInfo("App Name               : %s\n", pRdmAppDet->app_name);
    RDMInfo("PKG Name               : %s\n", pRdmAppDet->pkg_name);
    RDMInfo("PKG Type               : %s\n", pRdmAppDet->pkg_type);
    RDMInfo("PKG Ver                : %s\n", pRdmAppDet->pkg_ver);
    RDMInfo("App Size               : %s\n", pRdmAppDet->app_size);
    RDMInfo("App Size in kb         : %d\n", pRdmAppDet->app_size_kb);
    RDMInfo("App Size in mb         : %d\n", pRdmAppDet->app_size_mb);
    RDMInfo("App Home Path          : %s\n", pRdmAppDet->app_home);
    RDMInfo("App mnt  Path          : %s\n", pRdmAppDet->app_mnt);
    RDMInfo("App download Home      : %s\n", pRdmAppDet->app_dwnl_home);
    RDMInfo("App download Path      : %s\n", pRdmAppDet->app_dwnl_path);
    RDMInfo("App download File Path : %s\n", pRdmAppDet->app_dwnl_filepath);
    RDMInfo("App download URL       : %s\n", pRdmAppDet->app_dwnl_url);
    RDMInfo("App info Path          : %s\n", pRdmAppDet->app_dwnl_info);
    RDMInfo("Download On Demond     : %d\n", pRdmAppDet->dwld_on_demand);
    RDMInfo("Download Method Control: %s\n", pRdmAppDet->dwld_method_controller);
    RDMInfo("App Versioned          : %d\n", pRdmAppDet->is_versioned);
}

[OINT32 rdmPackageMgrStateChange(RDMAPPDetails *pRdmAppDet)
{
    INT32 isFileFound                                = 0;
    INT32 status                                     = RDM_SUCCESS;
    INT32 curl_ret_code                              = -1;
    INT32 httpCode                                   = 0;
    INT32 retryCount                                 = 0;
    CHAR path[MAX_BUFF_SIZE]                         = {0};
    CHAR token_header[MAX_BUFF_SIZE]                 = {0};
    CHAR file[RDM_APP_PATH_LEN]                      = {0};
    CHAR status_file[RDM_APP_PATH_LEN]               = {0};
    CHAR deactivate_status_file[RDM_APP_PATH_LEN]    = {0};
    CHAR token[RDM_APP_PATH_LEN]                     = {0};
    CHAR jsondata[RDM_APP_PATH_LEN]                  = {0};
    CHAR post_data[MAX_BUFF_SIZE]                    = {0};
    CHAR header[]                                    = RDM_CURL_CONTENTTYPE;
    VOID *curl                                       = NULL;
    DownloadData DwnLoc;
    FileDwnl_t file_dwnl;

    if ( pRdmAppDet ){
        snprintf(path,sizeof(path),"%s/%s/etc/apps/",pRdmAppDet->app_mnt,pRdmAppDet->app_name);
        snprintf(file,sizeof(file),"%s_package.json",pRdmAppDet->app_name);
        snprintf(status_file,sizeof(file),"/tmp/%s_status",pRdmAppDet->app_name);
        snprintf(deactivate_status_file,sizeof(file),"/tmp/%s_deactivateStatus",pRdmAppDet->app_name);

        isFileFound = findFile(path,file);

        if ( isFileFound ){
            CHAR callsignValue[128]  = {0};
            CHAR currentState[RDM_TMP_LEN_64]   = {0};
            CHAR method[RDM_TMP_LEN_64]         = {0};
            CHAR filepath[1024]                 = {0};

            strncpy(method, "Controller.1.status@", sizeof(method));
            snprintf(filepath,sizeof(filepath),"%s%s",path,file);
            getStringValueFromFile(filepath, "\":, ", "callsign", callsignValue );
            *jsondata = 0;
            snprintf(post_data,MAX_BUFF_SIZE,"{\"jsonrpc\":\"2.0\",\"id\":\"3\",\"method\": \"%s%s\"}",method,callsignValue);

            while( retryCount < DOWNLOAD_RETRY_COUNT ) {
                copyCommandOutput(RDM_WPE_SECUTIL, jsondata, sizeof(jsondata));
                rdmJRPCTokenData(token, jsondata, sizeof(token));

                if(rdmMemDLAlloc(&DwnLoc, RDM_DEFAULT_DL_ALLOC) == RDM_SUCCESS) {
                    if (DwnLoc.pvOut != NULL) {
                        file_dwnl.pHeaderData = header;
                        snprintf(token_header, sizeof(token_header), "Authorization: Bearer %s", token);
                        file_dwnl.pPostFields = post_data;
                        file_dwnl.pDlData = &DwnLoc;
                        snprintf(file_dwnl.url, sizeof(file_dwnl.url), "%s", RDM_JSONRPC_URL);

                        curl = doCurlInit();
                        if(curl == NULL) {
                            RDMError("Failed init curl\n");
                            status = RDM_FAILURE;
                        }
                        else{
                            curl_ret_code = getJsonRpcData(curl, &file_dwnl, token_header, &httpCode);

                            if(curl_ret_code && httpCode != 200) {
                                RDMError("Download failed\n");
	                        t2CountNotify("RDM_ERR_package_failed", 1);
                                status = RDM_FAILURE;
                            }
                            else{
                                FILE *file = fopen(status_file,"w+");
                                size_t writebytes = 0;
                                if(file){
                                    writebytes = fwrite(DwnLoc.pvOut,1,strlen((CHAR*)DwnLoc.pvOut),file);
                                    RDMInfo("writebytes %u\n",writebytes);
                                    fclose(file);
                                }
                                else{
                                    RDMError("%s file open failed",status_file);
                                }
                                getStringValueFromFile( status_file, "\":, ", "state", currentState);
                            }
                            doStopDownload(curl);
                        }
                    }
                    else{
                        RDMError("DwnLoc.pvOut is NULL\n");
                        status = RDM_FAILURE;
                    }

                    rdmMemDLFree(&DwnLoc);

                    if ( ( strcmp(currentState,"unavailable") == 0 ) || ( strcmp(currentState,"deactivated") == 0 ) ) {
                        break;
                    }
                }
                retryCount = retryCount + 1;
                sleep(5);
            }

            if( strcmp(currentState,"unavailable") == 0 ){
                strncpy(method, "Controller.1.deactivate",sizeof(method));
                snprintf(post_data,MAX_BUFF_SIZE,"{\"jsonrpc\":\"2.0\",\"id\":\"3\",\"method\": \"%s\", \"params\":{\"callsign\":\"%s\"}}",method,callsignValue);

                if(rdmMemDLAlloc(&DwnLoc, RDM_DEFAULT_DL_ALLOC) == RDM_SUCCESS) {
                    if (DwnLoc.pvOut != NULL) {
                        file_dwnl.pHeaderData = header;
                        snprintf(token_header, sizeof(token_header), "Authorization: Bearer %s", token);
                        file_dwnl.pPostFields = post_data;
                        file_dwnl.pDlData = &DwnLoc;
                        snprintf(file_dwnl.url, sizeof(file_dwnl.url), "%s", RDM_JSONRPC_URL);

                        curl = doCurlInit();
                        if(curl == NULL) {
                            RDMError("Curl Init Failed\n");
                            status = RDM_FAILURE;
                        }
                        else{
                            curl_ret_code = getJsonRpcData(curl, &file_dwnl, token_header, &httpCode);

                            RDMInfo("curl_ret_code:%d httpCode:%d\n", curl_ret_code, httpCode);

                            if(curl_ret_code && httpCode != 200) {
                                RDMError("Download failed\n");
	                        t2CountNotify("RDM_ERR_package_failed", 1);
                                status = RDM_FAILURE;
                            }
                            else{
                                FILE *file = fopen(deactivate_status_file,"w+");
                                CHAR  result[64];
                                size_t writebytes = 0;
                                if(file){
                                    writebytes = fwrite(DwnLoc.pvOut,1,strlen((CHAR*)DwnLoc.pvOut),file);
                                    RDMInfo("writebytes %u\n",writebytes);
                                    fclose(file);
                                }
                                rdmJRPCResultData(result, (CHAR *)DwnLoc.pvOut,sizeof(result) );
                                if ( strcmp(result,"\0") == 0 ){
                                    RDMInfo("%s successfully moved to deactivated state",pRdmAppDet->app_name);
                                }
                                else{
                                    RDMInfo("%s failed to move to deactivated state",pRdmAppDet->app_name);
                                    status = RDM_FAILURE;
                                }

                            }
                            doStopDownload(curl);
                        }
                    }
                    else{
                        RDMError("DwnLoc.pvOut is NULL\n");
                        status = RDM_FAILURE;
                    }

                    rdmMemDLFree(&DwnLoc);
                }
            }
            else{
                RDMInfo("Current state of %s is %s. Not deactivating it since it is not in unavailable state",pRdmAppDet->app_name,currentState);
            }
        }
        else{
            RDMError("File %s not found\n",file);
            status = RDM_FAILURE;
        }
    }
    else{
        RDMError("Invalid Parameter\n");
        status = RDM_FAILURE;
    }

    return status;
}

/* Description: Use For parsing jsonrpc results
 * @param: result : Pointer to receive result
 * @param: pJsonStr : Full json data
 * @param: result size : result size
 * @return int:
 * */
INT32 rdmJRPCResultData(CHAR *result, CHAR *pJsonStr, UINT32 result_size)
{
    JSON *pJson = NULL;
    INT32 ret = -1;

    if (result == NULL || pJsonStr == NULL) {
        RDMError( "Parameter is NULL\n");
        return ret;
    }

    pJson = ParseJsonStr(pJsonStr);

    if( pJson != NULL ) {
        GetJsonVal(pJson, "result",result,result_size);
        RDMInfo( "result: %s\n", result);
        FreeJson(pJson);
        ret = 0;
    }

    return ret;
}

/**
 * @brief Remove stale app data not present in current manifest.
 *        This leverages existing cleanup helpers in rdm_downloadutils
 *        to avoid introducing new deletion mechanisms.
 *
 * @param infoFilePath   Path to the downloads info file to update (may be NULL).
 * @param app_manifests  Array of app names present in the manifest.
 * @param numOfAppsManifest Number of entries in app_manifests.
 *
 * @return Number of apps removed.
 */
INT32 rdmDeleteStalePackages(const CHAR *infoFilePath, CHAR *app_manifests[], INT32 numOfAppsManifest)
{
    INT32 removedAppsCount = 0;

    /* Consider both homes: standard and broadband */
    const CHAR *homes[2] = { APP_MOUNT_PATH, APPLN_HOME_PATH_DEF };

    for (int h = 0; h < 2; h++) {
        CHAR downloadsHome[RDM_APP_PATH_LEN] = {0};
        snprintf(downloadsHome, sizeof(downloadsHome), "%s/rdm/downloads", homes[h]);

        CHAR *dlDirs[RDM_TMP_LEN_64] = {0};
        INT32 dlCount = 0;
        if (rdmListDirectory(downloadsHome, dlDirs, &dlCount) != RDM_SUCCESS) {
            /* Not fatal; continue with other home */
            continue;
        }

        for (INT32 i = 0; i < dlCount; i++) {
            CHAR *name = dlDirs[i];
            if (!name || !*name ||
                strcmp(name, ".") == 0 || strcmp(name, "..") == 0 ||
                strcmp(name, "etc") == 0 || strcmp(name, "rdm") == 0) {
                if (dlDirs[i]) free(dlDirs[i]);
                continue;
            }

            /* If app is in manifest → do nothing */
            if (isDataInList(app_manifests, name, numOfAppsManifest)) {
                if (dlDirs[i]) free(dlDirs[i]);
                continue;
            }

            /* Build install paths in both homes */
            CHAR installPathStd[RDM_APP_PATH_LEN] = {0};
            CHAR installPathBB[RDM_APP_PATH_LEN]  = {0};
            CHAR dlPath[RDM_APP_PATH_LEN]         = {0};
            snprintf(installPathStd, sizeof(installPathStd), "%s/%s/", APP_MOUNT_PATH, name);
            snprintf(installPathBB,  sizeof(installPathBB),  "%s/%s/", APPLN_HOME_PATH_DEF, name);

            size_t homeLen = strlen(downloadsHome);
            size_t nameLen = strlen(name);
            if (homeLen + 1 + nameLen + 1 <= sizeof(dlPath)) {
                memcpy(dlPath, downloadsHome, homeLen);
                dlPath[homeLen] = '/';
                memcpy(dlPath + homeLen + 1, name, nameLen);
                dlPath[homeLen + 1 + nameLen] = '\0';
            } else {
                RDMError("dlPath buffer too small for '%s/%s' (need %zu, have %zu)\n", downloadsHome, name, homeLen + 1 + nameLen + 1, sizeof(dlPath));
                dlPath[0] = '\0';
            }

            bool anyDeleted = false;
            if (folderCheck(installPathStd) /* exists */) {
                RDMInfo("Removing install dir for app '%s': %s\n", name, installPathStd);
                rdmDwnlAppCleanUp(installPathStd);
                anyDeleted = true;
            }
            if (folderCheck(installPathBB) /* exists */) {
                RDMInfo("Removing install dir for app '%s': %s\n", name, installPathBB);
                rdmDwnlAppCleanUp(installPathBB);
                anyDeleted = true;
            }

            if (folderCheck(dlPath) /* exists */) {
                RDMInfo("Removing downloads dir for app '%s': %s\n", name, dlPath);
                rdmDwnlCleanUp(dlPath);
                anyDeleted = true;
            }

            if (anyDeleted) {
                removedAppsCount++;
                /* Update info file: remove entries for this app */
                if (infoFilePath && *infoFilePath) {
                    rdmRemvDwnlAppInfo(name, (CHAR *)infoFilePath);
                }
            }

            if (dlDirs[i]) free(dlDirs[i]);
        }
    }

    return removedAppsCount;
}
INT32 rdmDwnlDebugToolSchedule(const CHAR *pTool, INT64 expiry_time)
{
    (void)pTool;
    (void)expiry_time;

    return RDM_SUCCESS;
}
INT32 rdmDwnlDebugToolCheckExpiry(void)
{
    return RDM_SUCCESS;
}
