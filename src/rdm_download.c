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
#include <libgen.h>

#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_downloadutils.h"
#include "rdm_download.h"
#ifndef GTEST_ENABLE
#include <system_utils.h>
#else
#include "unittest/mocks/system_utils.h"
#endif

static INT32 rdmDownloadCheckFs(RDMAPPDetails *pRdmAppDet)
{
    INT32 status = RDM_SUCCESS;
    INT32 found = 0;
    CHAR tmp_file[RDM_APP_PATH_LEN];

    /* check we are able to remove the file as well.
       Sometimes, corrupted disk don't allow us to delete a file */
    if(!checkFileSystem(pRdmAppDet->app_mnt)) {
        RDMError("%s is not in working condition.\n", pRdmAppDet->app_mnt);
        return RDM_FAILURE;
    }
    else {
        RDMInfo("%s disk check is fine\n", pRdmAppDet->app_mnt);
    }

    found = findPFile(pRdmAppDet->app_mnt, pRdmAppDet->pkg_name, tmp_file);
    if(found) {
        RDMInfo("%s already downloaded on secondary storage. So skip the download and validate the package.\n", pRdmAppDet->app_name);
        pRdmAppDet->dwld_status = 1;
    }
    else {
        UINT32 fs_freespace = 0;
        INT32  fs_status = RDM_SUCCESS;

        fs_status = emptyFolder(pRdmAppDet->app_dwnl_path);
        if(fs_status) {
            RDMError("Failed to empty the folder: %s\n", pRdmAppDet->app_dwnl_path);
        }

        fs_status = emptyFolder(pRdmAppDet->app_home);
        if(fs_status) {
            RDMError("Failed to empty the folder: %s\n", pRdmAppDet->app_home);
        }

        fs_freespace = getFreeSpace(pRdmAppDet->app_mnt);
        if(fs_freespace > pRdmAppDet->app_size_mb) {
            RDMInfo("secondary storage scratchpad will be used for App download\n");
        }
        else {
            RDMError("Not enough space available for App download on %s. Downloading the App on tmp dir.\n", APP_MOUNT_PATH);
            return RDM_FAILURE;
        }
    }
    return status;
}

static VOID rdmDownloadUpdateDef(RDMAPPDetails *pRdmAppDet)
{
    snprintf(pRdmAppDet->app_mnt, RDM_APP_PATH_LEN, "%s", APPLN_HOME_PATH_DEF);
    snprintf(pRdmAppDet->app_home, RDM_APP_PATH_LEN, "%s/%s", APPLN_HOME_PATH_DEF, pRdmAppDet->app_name);
    snprintf(pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN, "%s/rdm/downloads/%s", APPLN_HOME_PATH_DEF, pRdmAppDet->app_name);
    snprintf(pRdmAppDet->app_dwnl_info, RDM_APP_PATH_LEN, "%s", RDM_DL_INFO_BR);
}

/***************************************************************************************************
*   Function Name    :    rdmDownloadApp
*   Description      :    Dowloads the packages given as input and updates the status in
*                         rdmDownloadInfo.txt
*   Input(s)         :    pRdmAppDet - Contains all Info about the package to be downloaded
*   Output           :    pDLStatus- Download status of the package
*                         status   - Download status of the package
***************************************************************************************************/
INT32 rdmDownloadApp(RDMAPPDetails *pRdmAppDet, INT32 *pDLStatus)
{
    INT32 status = RDM_SUCCESS;
    CHAR *rdm_status = "FAILURE";
    CHAR *DLInfoPath = NULL;
    CHAR *DLInfoFile = NULL;
    INT32 DLInfoStatus = RDM_SUCCESS;
    FILE  *fp_met = NULL;

    *pDLStatus = RDM_DL_NOERROR;

    RDMInfo("Downloading %s...\n", pRdmAppDet->app_name);

    if(pRdmAppDet->bFsCheck) {
        INT32 fs_status = RDM_SUCCESS;

        fs_status = rdmDownloadCheckFs(pRdmAppDet);
        if(fs_status) {
            rdmDownloadUpdateDef(pRdmAppDet);
            RDMWarn("%s is not in working condition.\n", APP_MOUNT_PATH);
        }
        else {
            RDMInfo("%s Disk check is fine\n", APP_MOUNT_PATH);
        }
    }
    RDMInfo("HOME PATH for APP = %s\n", pRdmAppDet->app_home);

    /*Clearing pkg type for the usb thunder plugin download*/
    if(pRdmAppDet->is_usb == 1){
        strncpy(pRdmAppDet->pkg_type, "\0", sizeof(pRdmAppDet->pkg_type));
    }

    /* Versioned app installation */
    if(pRdmAppDet->is_versioned) {
        status = rdmDownloadVerApp(pRdmAppDet);
	if(status == RDM_SUCCESS) {
	    rdm_status = "SUCCESS";
	}
    }
    /* Legacy app installation */
    else {
        INT32 count = 0;
        while(count < RDM_RETRY_COUNT) {
            *pDLStatus = rdmDownloadMgr(pRdmAppDet);
            if(*pDLStatus == RDM_DL_NOERROR) {
                RDMInfo("rdmDownloadMgr is sucess\n");
                rdm_status = "SUCCESS";
                break;
            }

            rdmDwnlUnInstallApp(pRdmAppDet->app_dwnl_path, pRdmAppDet->app_home);
            count++;
        }
    }

    /* Keeps the meta-data about the Apps in following format in /opt/persistent/rdmDownloadInfo.txt
       Meta data contains App Name, Package Name, App Home path, App Size, Download Status */

    /*Creating the path for storing Package info if it doesn't exist*/
    DLInfoFile = strndup (pRdmAppDet->app_dwnl_info, strlen(pRdmAppDet->app_dwnl_info));
    if (DLInfoFile != NULL){

        DLInfoPath = dirname(DLInfoFile); 
        if(createDir(DLInfoPath) != RDM_SUCCESS) {
            RDMWarn("Unable to create the package info path: %s !\n", DLInfoPath);
            DLInfoStatus = RDM_FAILURE;
        }
        else{
            RDMInfo("Package info path %s exists", DLInfoPath);
        }
    }

    /*If the rdmDownloadInfo.txt file doesn't exist, it will be created*/
    if(DLInfoStatus == RDM_SUCCESS) {
        fp_met = fopen(pRdmAppDet->app_dwnl_info, "a+");
        if(fp_met) {
            char buffer[4096]={0};
            char line[256];
            size_t buffer_index = 0;

            RDMInfo("Opened %s file successfully", pRdmAppDet->app_dwnl_info);
            fseek(fp_met, 0, SEEK_SET);
            while (fgets(line, sizeof(line), fp_met)) {
                if(strncmp(line, pRdmAppDet->app_name, strlen(pRdmAppDet->app_name)) != 0 ){
                    strncpy(buffer + buffer_index, line, sizeof(buffer) - buffer_index);
                    buffer_index += strlen(line);
                }
            }

            ftruncate(fileno(fp_met), 0);
            fseek(fp_met, 0, SEEK_SET);

            fputs(buffer, fp_met);

            fseek(fp_met, 0, SEEK_END);
            fprintf(fp_met, "%s %s %s/%s %d %s\n", pRdmAppDet->app_name,
                                                   pRdmAppDet->pkg_name,
                                                   pRdmAppDet->app_home,
                                                   pRdmAppDet->app_name,
                                                   pRdmAppDet->app_size_kb,
                                                   rdm_status);
            fclose(fp_met);
        }
        else {
            RDMWarn("Unable to open meta data file: %s\n", pRdmAppDet->app_dwnl_info);
        }
    }
    else{
        RDMWarn("Package info path %s does not exist!\n", DLInfoPath);
    }
    if(DLInfoFile != NULL) {
        free(DLInfoFile);
        DLInfoPath = NULL;
        DLInfoFile = NULL;
    }
    return status;
}
