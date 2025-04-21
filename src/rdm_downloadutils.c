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
#ifndef GTEST_ENABLE
#include <system_utils.h>
#include <json_parse.h>
#include <downloadUtil.h>
#include "codebigUtils.h"
#include "rdmMgr.h"
#else
#include "unittest/mocks/system_utils.h"
#include "unittest/mocks/rdmMgr.h"
#endif


#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_download.h"
#include "rdm_rbus.h"
#include "rdm_openssl.h"
#include "rdm_downloadutils.h"
#include "rdm_packagemgr.h"

#ifdef GTEST_ENABLE
char* retStr = NULL;
#endif

UINT32 rdmDwnlIsBlocked(CHAR *file, INT32 block_time)
{
    UINT32 modification_time = 0;
#ifndef GTEST_ENABLE
    UINT32 current_time = 0;
    UINT32 last_mod_time = 0;

    INT32  remtime = 0;

    last_mod_time = getFileLastModifyTime(file);
    if (last_mod_time != 0) {
        current_time = getCurrentSysTimeSec();
        modification_time = current_time - last_mod_time;
        remtime = (block_time/60) - (modification_time/60);

        if (modification_time <= block_time) {
            RDMInfo("ImageUpgrade: Last failed blocking is still valid for %d mins, preventing direct\n", remtime);
        } else {
            RDMInfo("ImageUpgrade: Last failed blocking has expired, removing %s, allowing direct\n", file);
            unlink(file);
            modification_time = 0;
        }
    }
#endif
    return modification_time;
}

INT32 rdmDwnlUpdateURL(CHAR *pUrl)
{
    INT32 status = RDM_SUCCESS;
#ifndef GTEST_ENABLE
    CHAR dwnl_url[MAX_BUFF_SIZE];
    FILE *fp  = NULL;


    if(fileCheck(RDM_DWNL_URL)) {
        copyFiles(RDM_DWNL_URL, RDM_DWNLSSR_URL);

        if(folderCheck(RDM_NVRAM_PATH)) {
            copyFiles(RDM_DWNLSSR_URL, RDM_NVRAM_DL_PATH);
        }
    }
    fp = fopen(RDM_DWNLSSR_URL, "r");
    if(fp) {
        fgets(dwnl_url, MAX_BUFF_SIZE, fp);
        if(strstr(dwnl_url, "http")) {
            if(dwnl_url[strlen(dwnl_url)-1] == '\n') {
                dwnl_url[strlen(dwnl_url)-1] = '\0';
            }
            strncpy(pUrl, dwnl_url, MAX_BUFF_SIZE);
	    RDMInfo("Download URL available in %s is %s\n", RDM_DWNLSSR_URL, pUrl);
        }
        fclose(fp);
    }
    else {
	RDMInfo("Download URL is not available in %s\n", RDM_DWNLSSR_URL);
	RDMInfo("Using RDM Default url %s to download from the Xconf Server\n", pUrl);
    }

    if(strstr(pUrl, "http")) {
        /* Replace http with https */
        if(!strstr(pUrl, "https")) {
	    RDMInfo("Replacing http with https in curl download request\n");
            memset(dwnl_url, 0, MAX_BUFF_SIZE);
            strncpy(dwnl_url, "https", MAX_BUFF_SIZE);
            strncpy(dwnl_url+5, pUrl+4, MAX_BUFF_SIZE-5);
            strncpy(pUrl, dwnl_url, MAX_BUFF_SIZE);
        }
	RDMInfo("RDM App Download URL Location is %s\n", pUrl);
    }
    else {
        RDMError("RDM download url is not available in both %s and RFC parameter. Exiting...\n", RDM_DWNLSSR_URL);
        status = RDM_FAILURE;
    }
#endif
    return status;
}

INT32 rdmDwnlCreateFolder(CHAR *pAppPath, CHAR *pAppname)
{
    INT32 status = RDM_SUCCESS;
#ifndef GTEST_ENABLE
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
#endif
    return status;
}

VOID rdmDwnlCleanUp(CHAR *pDwnlPath)
{
#ifndef GTEST_ENABLE
    INT32  fs_status = RDM_SUCCESS;

    fs_status = emptyFolder(pDwnlPath);
    if(fs_status) {
        RDMError("Failed to empty the folder: %s\n", pDwnlPath);
    }

    removeFile(pDwnlPath);
#endif
}

VOID rdmDwnlAppCleanUp(CHAR *pAppPath)
{
#ifndef GTEST_ENABLE
    INT32  fs_status = RDM_SUCCESS;

    fs_status = emptyFolder(pAppPath);
    if(fs_status) {
        RDMError("Failed to empty the folder: %s\n", pAppPath);
    }

    removeFile(pAppPath);
#endif
}

VOID rdmRemvDwnlAppInfo(CHAR *pAppName, CHAR *pDwnlInfoFile)
{
    FILE *fp_met = NULL;

    fp_met = fopen(pDwnlInfoFile, "a+");
    if(fp_met) {
        char buffer[4096]={0};
        char line[256];
        size_t buffer_index = 0;

        RDMInfo("Opened %s file successfully for cleanup\n", pDwnlInfoFile);
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
            RDMInfo("Unable to open file %s", pDwnlInfoFile);
        }
}

//Check for StateRed. Returns 1 if in StateRed.
INT32 rdmDwnlIsInStateRed()
{
    return 0;
}

INT32 rdmDwnlGetCert(MtlsAuth_t *sec)
{
    //Read your Cert details here
    strncpy(sec->cert_name, "MyCertName", sizeof(sec->cert_name) - 1);
    sec->cert_name[sizeof(sec->cert_name) - 1] = '\0';  // Ensure null termination
    strncpy(sec->cert_type, "MyCertType", sizeof(sec->cert_type) - 1);
    sec->cert_type[sizeof(sec->cert_type) - 1] = '\0';  // Ensure null termination
    strncpy(sec->key_pas, "MyCertkey", sizeof(sec->key_pas) - 1);
    sec->key_pas[sizeof(sec->key_pas) - 1] = '\0';  // Ensure null termination
    RDMInfo("success. cert=%s, cert type=%s and key=%s\n", sec->cert_name, sec->cert_type, sec->key_pas);
    return RDM_SUCCESS;
}

//Check for OCSPEnable. Returns 1 if enabled.
INT32 rdmDwnlIsOCSPEnable(void)
{
    return 0;
}

INT32 rdmDwnlDirect(CHAR *pUrl, CHAR *pDwnlPath, CHAR *pPkgName, CHAR *pOut, INT32 isMtls)
{
    VOID *curl = NULL;
    INT32 status = RDM_SUCCESS;
    INT32 curl_ret_code = -1;
    INT32 httpCode;
    INT32 max_dwnl_speed = 0;
    FileDwnl_t file_dwnl;
    MtlsAuth_t sec;

    memset(&file_dwnl, 0, sizeof(FileDwnl_t));
    memset(&sec, 0, sizeof(MtlsAuth_t));

    /* Update the file download details */
    file_dwnl.chunk_dwnl_retry_time = 0;
    strncpy(file_dwnl.url, pUrl, sizeof(file_dwnl.url) - 1);
    file_dwnl.url[sizeof(file_dwnl.url) - 1] = '\0';  // Ensure null termination
    strncpy(file_dwnl.pathname, pDwnlPath, sizeof(file_dwnl.pathname) -1);
    strcat(file_dwnl.pathname, "/");
    strcat(file_dwnl.pathname, pPkgName);
    file_dwnl.pathname[sizeof(file_dwnl.pathname) - 1] = '\0';  // Ensure null termination
    strcpy(pOut, file_dwnl.pathname);

    if(rdmDwnlIsOCSPEnable()) {
        file_dwnl.sslverify = true;
    }
    file_dwnl.pDlData = NULL;
    file_dwnl.pPostFields = NULL;

    if(isMtls) {
        /* Update the Certificate */
        status = rdmDwnlGetCert(&sec);
        if(status) {
            RDMError("Failed to get MTLS certificate\n");
            return RDM_FAILURE;
        }
    }
    curl = doCurlInit();
    if(curl == NULL) {
        RDMError("Failed init curl\n");
        return status;
    }
    RDMInfo("Downloading The Package %s \n",file_dwnl.pathname);
    curl_ret_code = doHttpFileDownload(curl, &file_dwnl, &sec,
                                       max_dwnl_speed, NULL, &httpCode);

    RDMInfo("curl_ret_code:%d httpCode:%d\n", curl_ret_code, httpCode);

    if(curl_ret_code && httpCode != 200) {
        RDMError("Download failed\n");
        status = RDM_FAILURE;
    }

    doStopDownload(curl);
    return status;
}

#ifdef RDM_ENABLE_CODEBIG
INT32 rdmDwnlCodebig(CHAR *pUrl, CHAR *pDwnlPath, CHAR *pPkgName, CHAR *pOut)
{
    return RDM_SUCCESS;
}
#endif

INT32 rdmDwnlApplication(CHAR *pUrl, CHAR *pDwnlPath, CHAR *pPkgName, CHAR *pOut, INT32 isMtls)
{
    INT32 status = RDM_SUCCESS;
    INT32 retry_count = 0;

    /* Check for direct download is blocked */
    if(!(rdmDwnlIsBlocked(DIRECT_BLOCK_FILENAME, DIRECT_BLOCK_TIME))) {
        while(retry_count <= 2) {
	    if(retry_count == 1) {
	        RDMInfo("appliactionDownload: Attempting retry = %d\n", retry_count);
	        sleep(10);
	    }
	    else if(retry_count == 2) {
	        RDMInfo("appliactionDownload: Attempting retry = %d\n", retry_count);
	        sleep(30);
	    }
            status = rdmDwnlDirect(pUrl, pDwnlPath, pPkgName, pOut, isMtls);
            if(status == RDM_SUCCESS) {
	        return status;
	    }
            else {
                RDMWarn("applicationDownload: Direct App download failed\n");
	        retry_count++;
	    }
        }
    }
    else {
        RDMInfo("appliactionDownload: Direct download is blocked\n");
        status = RDM_FAILURE;
    }
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
    INT32 ret = 0;
#ifndef GTEST_ENABLE
    JSON *pJson = NULL;
    INT8  status[8];
    ret = -1;

    if (token == NULL || pJsonStr == NULL) {
        RDMError( "Parameter is NULL\n");
        return ret;
    }
    *status = 0;
    pJson = ParseJsonStr(pJsonStr);

    if( pJson != NULL ) {
        GetJsonVal(pJson, "token", token, token_size);
        GetJsonVal(pJson, "success", status, sizeof(status));
        RDMInfo( "status: %s\n", status);
        FreeJson(pJson);
        if (0 == strncmp(status, "true", 4) && 0 != *token) {
            ret = 0;
        }
    }
#endif
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
            RDMError( "rdmMemDLAlloc: Failed to allocate memory for XCONF download\n" );
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

INT32 rdmDwnlRunPostScripts(CHAR *pAppHome)
{
    CHAR tmp_file[RDM_APP_PATH_LEN];
    CHAR filePath[RDM_APP_PATH_LEN];
    DIR *dir;
    struct dirent *entry;

    RDMInfo("RDM Start Post script Execution\n");

    strncpy(tmp_file, pAppHome, RDM_APP_PATH_LEN);
    strcat(tmp_file, RDM_POSTSCRIPT_PATH);

    RDMInfo("Before opendir()\n");
    dir = opendir(tmp_file);
    RDMInfo("After opendir()\n");

    if (dir == NULL) {
        RDMError("Error opening directory: %s\n", tmp_file);
        return RDM_FAILURE;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0  ||
            strcmp(entry->d_name, "..") == 0 ||
            entry->d_type == DT_DIR) {
            continue;
        }
	RDMInfo("Before getExtension()\n");
	RDMInfo("Entry->d_name = %s\n", entry->d_name);
	//RDMInfo("getExtension() = %s\n", getExtension(entry->d_name));
        if(strcmp(getExtension(entry->d_name), "sh")) {
	    RDMInfo("Not an sh extension\n");
            continue;
        }
	/*if (strcmp("sh", strrchr(entry->d_name, '.') + 1)) {
	    RDMInfo("Not an sh extension\n");
	    continue;
	}*/
	RDMInfo("After getExtension()\n");

        strncpy(filePath, tmp_file, RDM_APP_PATH_LEN - 1);
	filePath[sizeof(filePath) - 1] = '\0';  // Ensure null termination
        strcat(filePath, "/");
        strcat(filePath, entry->d_name);
	filePath[sizeof(filePath) - 1] = '\0';  // Ensure null termination

        RDMInfo("RDM Post script Execution %s\n", filePath);

	RDMInfo("Before copyCommandOutput()\n");
        copyCommandOutput (filePath, NULL, 0);
	RDMInfo("After copyCommandOutput()\n");
    }

    closedir(dir);
    return RDM_SUCCESS;
}
#ifndef GTEST_ENABLE
VOID rdmDwnlUnInstallApp(CHAR *pDwnlPath, CHAR *pAppPath)
{
    rdmDwnlCleanUp(pDwnlPath);
    rdmDwnlAppCleanUp(pAppPath);
}
#endif

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
    CHAR *buff = NULL;
    RDMInfo("FIVE.ONE.ONE\n");
    fpin = fopen(pInManifest, "r");
    if(fpin == NULL) {
        RDMError("Unable to open input file: %s\n", pInManifest);
        return RDM_FAILURE;
    }
    RDMInfo("FIVE.ONE.TWO\n");

    fpout = fopen(pOutManifest, "w");
    if(fpout == NULL) {
        RDMError("Unable to open output file: %s\n", pOutManifest);
        status = RDM_FAILURE;
        goto error;
    }
    RDMInfo("FIVE.ONE.THREE\n");

    buff = calloc(MAX_BUFF_SIZE, 1);
    if(buff == NULL) {
        RDMError("Failed to allocate memory\n");
        status = RDM_FAILURE;
        goto error;
    }
    RDMInfo("FIVE.ONE.FOUR\n");

    while (fgets(buff, MAX_BUFF_SIZE, fpin)) {
        fprintf(fpout, "%s/%s", update, buff);
    }

    RDMInfo("FIVE.ONE.FIVE\n");
    RDMInfo("padding = %s\n", padding);
    fprintf(fpout, "%s/pkg_padding", padding);
    RDMInfo("FIVE.ONE.SIX\n"); //debug
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
    RDMInfo("FIVE.ONE.SEVEN\n");
    return status;
}

// Remove Signing header here
static INT32 rdmDwnlReadSigFile(CHAR *sig_file, CHAR *pout)
{
    INT32 status = RDM_SUCCESS;
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
    INT32 status = RDM_SUCCESS;
    CHAR pkg_file[RDM_APP_PATH_LEN];
    CHAR tmp_file[RDM_APP_PATH_LEN];
    CHAR out_file[RDM_APP_PATH_LEN];
    CHAR dwnl_path[RDM_APP_PATH_LEN];
    CHAR app_home[RDM_APP_PATH_LEN];
    INT32 ssl_status = 0;
    INT32 outputMsgLen = REPLY_MSG_LEN;
    CHAR *out_buf = calloc(RDM_SIGFILE_LEN, 1);

    RDMInfo("ONE\n");

    if(out_buf == NULL) {
        RDMError("Failed to allocate memory\n");
        return RDM_FAILURE;
    }

    RDMInfo("TWO\n");
    strncpy(dwnl_path, pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN);
    dwnl_path[RDM_APP_PATH_LEN - 1] = '\0';  // Ensure null termination
    if(pVer) {
        strcat(dwnl_path, "/v");
        strcat(dwnl_path, pVer);
    }
    RDMInfo("THREE\n");
    strncpy(app_home, pRdmAppDet->app_home, RDM_APP_PATH_LEN);
    if(pVer) {
        strcat(app_home, "/v");
        strcat(app_home, pVer);
        strcat(app_home, "/package");
    }

    RDMInfo("FOUR\n");
    if(findPFile(dwnl_path, "*-pkg.sig", pkg_file)) {
        /* Read the signature file */
	    RDMInfo("FOUR.ONE\n");
        status = rdmDwnlReadSigFile(pkg_file, out_buf);
	RDMInfo("FOUR.TWO\n");
        if(status) {
            RDMError("Failed to read Signature file: %s\n", pkg_file);
	    if(out_buf)
                free(out_buf);
            return status;
        }
    }

    RDMInfo("FIVE\n");
    if(pRdmAppDet->sig_type == RDM_SIG_TYPE_KMS) {
        status = rdmDecryptKey(RDM_KMS_PUB_KEY);

        RDMInfo("Validate the Package\n");

        strncpy(tmp_file, RDM_CPEMANIFEST_PATH, RDM_APP_PATH_LEN);
        strcat(tmp_file, "/");
        strcat(tmp_file, pRdmAppDet->app_name);
        strcat(tmp_file, "_cpemanifest");

        strncpy(out_file, app_home, RDM_APP_PATH_LEN);
        strcat(out_file, "/");
        strcat(out_file, pRdmAppDet->app_name);
        strcat(out_file, "_cpemanifest");
	RDMInfo("out_file: %s\n", out_file);

	RDMInfo("FIVE.ONE\n");
        status = rdmDwnlUpdateManifest(tmp_file, out_file,
                                       app_home,
                                       dwnl_path);
	RDMInfo("FIVE.TWO\n");
        if(status) {
            RDMWarn("Failed to process manifest file\n");
        }
        else {
	    RDMInfo("FIVE.THREE\n");

            rdmInitSslLib();

	    RDMInfo("FIVE.FOUR\n");

            ssl_status = rdmOpensslRsafileSignatureVerify(out_file, -1,
                                                          out_buf,
                                                          RDM_KMS_PUB_KEY, tmp_file,
                                                          &outputMsgLen);
	    RDMInfo("FIVE.FIVE\n");
	    RDMInfo("ssl_status: %d\n", ssl_status);
	    RDMInfo("Expected retcode_success: %d\n", retcode_success);
            if (ssl_status == retcode_success) {
                RDMInfo("RSA Signature Validation Success\n");
            } else {
                RDMError("RSA Signature Verification Failed\n");
                status = RDM_FAILURE;
            }
        }
    }
    else if(pRdmAppDet->sig_type == RDM_SIG_TYPE_OPENSSL) {
      /*In the script, there is no call for this sig type. So not handled*/
	    RDMInfo("SIX\n");
    }
    else {
	    RDMInfo("SEVEN\n");
        RDMError("Unknown Signature Type\n");
        status = RDM_FAILURE;
    }

    RDMInfo("EIGHT\n");
    if(out_buf) {
        free(out_buf);
    }
    RDMInfo("NINE\n");
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
#ifndef GTEST_ENABLE
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
#endif
    return ret;
}


/** @brief This Function uninstalls the apps that not present in the manifest
 *
 *  @return  Returns status of the function.
 *  @retval  Returns RDM_SUCCESS on success, RDM_FAILURE otherwise.
 **/
INT32 rdmUnInstallApps(INT32  is_broadband)
{
    INT32 status                         = RDM_SUCCESS;
#ifndef GTEST_ENABLE
    CHAR *app_manifests[RDM_TMP_LEN_64]  = {0};
    CHAR *app_installed[RDM_TMP_LEN_64]  = {0};

    INT32 numOfAppsInstalled             = 0;
    INT32 numOfAppsManifest              = 0;
    INT32 isAppUninstalled               = 0;
    bool  isFileFound                    = 0;
    RDMAPPDetails *pApp_det              = NULL;
    INT32 ret                            = RDM_SUCCESS;
    CHAR  pkg_file[RDM_APP_PATH_LEN]     = {0};

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
                            }
                            RDMInfo(" Installed App %s is found in Manifest, so Skipping to next... \n", app_installed[index]);
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
                                }
                                RDMInfo(" Installed App %s is found in Manifest, so Skipping to next... \n", app_installed[index]);
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
#endif
   return status;
}

INT32 rdmUpdateAppDetails(RDMHandle *prdmHandle,
                          RDMAPPDetails *pRdmAppDet,
                          INT32 is_broadband)
{
#ifndef GTEST_ENABLE
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


    ret = rdmRbusGetRfc(prdmHandle->pRbusHandle,
                        RDM_RFC_MTLS,
                        &pRdmAppDet->is_mtls);

    if (ret != RDM_SUCCESS) {
        RDMWarn("Failed to Get mtls status from rbus\n");
        pRdmAppDet->is_mtls = 1;
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
#endif
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

//Required for packagemanager based RDM downloads
INT32 rdmPackageMgrStateChange(RDMAPPDetails *pRdmAppDet)
{
    return RDM_SUCCESS;
}

/* Description: Use For parsing jsonrpc results
 * @param: result : Pointer to receive result
 * @param: pJsonStr : Full json data
 * @param: result size : result size
 * @return int:
 * */
INT32 rdmJRPCResultData(CHAR *result, CHAR *pJsonStr, UINT32 result_size)
{
    INT32 ret = 0;
#ifndef GTEST_ENABLE
    JSON *pJson = NULL;
    ret = -1;

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
#endif
    return ret;
}
