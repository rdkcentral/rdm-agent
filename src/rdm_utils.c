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
#include "libIBus.h"
#include "rdmMgr.h"
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_download.h"
#include "rdm_downloadutils.h"
#include <system_utils.h>

#ifdef RDK_LOGGER_ENABLED
INT32  g_rdk_logger_enabled = 0;
#endif

void RDMLOGInit()
{
#ifdef RDK_LOGGER_ENABLED
    if (0 == rdk_logger_init(DEBUG_INI_NAME)) {
        g_rdk_logger_enabled = 1;
    }
#endif
}
/*****************************************************************************
                           IARM Bus Operations
*****************************************************************************/
/** @brief This Function sends events to RDM manager using IARM bus.
 *
 *  @param[in]  status Event Status
 *
 *  @return  Returns the status of the operation.
 *  @retval  Returns RDM_SUCCESS on success, RDM_FAILURE otherwise.
 */
INT32 rdmIARMEvntSendStatus(UINT8 status)
{
    INT32 ret = RDM_SUCCESS;

//#ifdef PLATFORM_SUPPORTS_RDMMGR
    IARM_Result_t iarmRetCode = IARM_RESULT_SUCCESS;

    iarmRetCode = IARM_Bus_Init("AppDownloadEvent");
    if(iarmRetCode != IARM_RESULT_SUCCESS) {
        RDMError("IARM bus init failed err: %d\n", iarmRetCode);
        ret = RDM_FAILURE;
        goto exit2;
    }
    iarmRetCode = IARM_Bus_Connect();
    if(iarmRetCode != IARM_RESULT_SUCCESS) {
        RDMError("IARM bus connection failed err: %d\n", iarmRetCode);
        ret = RDM_FAILURE;
        goto exit1;
    }

    iarmRetCode = IARM_Bus_BroadcastEvent(IARM_BUS_RDMMGR_NAME,
                                          (IARM_EventId_t)IARM_BUS_RDMMGR_EVENT_APPDOWNLOADS_CHANGED,
                                          (VOID *)&status, sizeof(status));
    if(iarmRetCode != IARM_RESULT_SUCCESS) {
        RDMError("Unable to Broadcast IARM Event event: %d, err: %d\n", status, iarmRetCode);
        ret = RDM_FAILURE;
    }

    IARM_Bus_Disconnect();
exit1:
    IARM_Bus_Term();
exit2:
//#endif

    return ret;
}

INT32 rdmIARMEvntSendPayload(CHAR *pkg_name, CHAR *pkg_ver,
                             CHAR *pkg_path, INT32 pkg_status)
{
    IARM_Bus_RDMMgr_EventData_t eventData;
    INT32 ret = RDM_SUCCESS;
    IARM_Result_t iarmRetCode = IARM_RESULT_SUCCESS;

    iarmRetCode = IARM_Bus_Init("AppDownloadEvent");
    if(iarmRetCode != IARM_RESULT_SUCCESS) {
        RDMError("IARM bus init failed err: %d\n", iarmRetCode);
        ret = RDM_FAILURE;
        goto exit2;
    }
    iarmRetCode = IARM_Bus_Connect();
    if(iarmRetCode != IARM_RESULT_SUCCESS) {
        RDMError("IARM bus connection failed err: %d\n", iarmRetCode);
        ret = RDM_FAILURE;
        goto exit1;
    }

    strncpy(eventData.rdm_pkg_info.pkg_name, pkg_name, RDM_PKG_NAME_MAX_SIZE - 1);
    eventData.rdm_pkg_info.pkg_name[RDM_PKG_NAME_MAX_SIZE - 1] = '\0';

    strncpy(eventData.rdm_pkg_info.pkg_version, pkg_ver, RDM_PKG_VERSION_MAX_SIZE - 1);
    eventData.rdm_pkg_info.pkg_version[RDM_PKG_VERSION_MAX_SIZE - 1] = '\0';

    strncpy(eventData.rdm_pkg_info.pkg_inst_path, pkg_path, RDM_PKG_INST_PATH_MAX_SIZE - 1);
    eventData.rdm_pkg_info.pkg_inst_path[RDM_PKG_INST_PATH_MAX_SIZE - 1] = '\0';

    eventData.rdm_pkg_info.pkg_inst_status = (IARM_RDMMgr_Status_t) pkg_status;

    iarmRetCode = IARM_Bus_BroadcastEvent(IARM_BUS_RDMMGR_NAME,
                                          (IARM_EventId_t)IARM_BUS_RDMMGR_EVENT_APP_INSTALLATION_STATUS,
                                          (void *)&eventData,
                                          sizeof(eventData));
    if(iarmRetCode != IARM_RESULT_SUCCESS) {
        RDMError("Unable to Broadcast IARM Event Payload err: %d\n", iarmRetCode);
        ret = RDM_FAILURE;
    }

    IARM_Bus_Disconnect();
exit1:
    IARM_Bus_Term();
exit2:
    return ret;
}
/** @brief This Function finds the directory inside the directory.
 *
 *  @param[in]   pRdmAppDet    App details
 *.
 *  @retval  Returns RDM_SUCCESS on success, RDM_FAILURE otherwise.
 */

INT32 rdmListDirectory(CHAR *rdmDirToList, CHAR **pDirList, INT32 *pNumOfDir)
{
    struct dirent *de;  // Pointer for directory entry
    INT32 status = RDM_SUCCESS;
    INT32 idx = 0;

    // opendir() returns a pointer of DIR type.
    if(rdmDirToList != NULL){
        DIR *dr = opendir(rdmDirToList);

        if (dr == NULL){  // opendir returns NULL if couldn't open directory
            RDMError("Could not open current directory\n" );
            status = RDM_FAILURE;
            goto error;
        }

        // for readdir()
        while ((de = readdir(dr)) != NULL){
            pDirList[idx] = (CHAR *)malloc(RDM_APPNAME_LEN);

            if( pDirList[idx] == NULL ){
                RDMError("Malloc for Dir List is failed");
                status = RDM_FAILURE;
                break;
            }

            memset(pDirList[idx], 0, RDM_APPNAME_LEN);

            RDMInfo("%s\n", de->d_name);
            strcpy(pDirList[idx],de->d_name);
            idx += 1;
        }

        closedir(dr);
        *pNumOfDir = idx;
    }
    else{
        RDMError("Invalid argument as DIR to list\n");
        status = RDM_FAILURE;
    }

error:
    return status;
}
