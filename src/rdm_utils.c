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
#include <errno.h>
#include <fcntl.h>

#ifndef GTEST_ENABLE
#ifdef IARMBUS_SUPPORT
#include "libIBus.h"
#include "rdmMgr.h"
#endif
#else
#include "unittest/mocks/libIBus.h"
#include "unittest/mocks/rdmMgr.h"
#endif
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_download.h"
#include "rdm_downloadutils.h"
#ifndef GTEST_ENABLE
#include <system_utils.h>
#else
#include "unittest/mocks/system_utils.h"
#endif

#ifdef RDK_LOGGER_ENABLED
#include "rdk_logger.h"
#endif

#ifdef RDK_LOGGER_ENABLED
INT32  g_rdk_logger_enabled = 0;
#endif

void RDMLOGInit()
{
#ifdef RDK_LOGGER_ENABLED
    /* Initialize RDK Logger */	
#ifndef IARMBUS_SUPPORT
    if (0 == rdk_logger_init(DEBUG_INI_NAME)) {
        g_rdk_logger_enabled = 1;
    }
#else
    rdk_logger_ext_config_t config = {
        .pModuleName = "LOG.RDK.RDMAGENT",        /* Module name */
        .loglevel = RDK_LOG_INFO,                 /* Default log level */
        .output = RDKLOG_OUTPUT_CONSOLE,          /* Output to console (stdout/stderr) */
        .format = RDKLOG_FORMAT_WITH_TS,          /* Timestamped format */
        .pFilePolicy = NULL                       /* Not using file output, so NULL */
    };

    if (rdk_logger_ext_init(&config) != RDK_SUCCESS) {
        printf("RDMAGENT : ERROR - Extended logger init failed\n");
    }

    g_rdk_logger_enabled = 1;
#endif
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
#ifdef IARMBUS_SUPPORT
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
#endif
    return ret;
}

INT32 rdmIARMEvntSendPayload(CHAR *pkg_name, CHAR *pkg_ver,
                             CHAR *pkg_path, INT32 pkg_status)
{
    INT32 ret = RDM_SUCCESS;
#ifdef IARMBUS_SUPPORT
    IARM_Bus_RDMMgr_EventData_t eventData;

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
#endif
    return ret;
}
static INT32 rdmEmptyDirectoryFd(INT32 fd)
{
    DIR *dir = fdopendir(fd);
    struct dirent *entry;
    INT32 status = RDM_SUCCESS;

    if (dir == NULL) {
        RDMError("Unable to read cleanup directory: %s\n", strerror(errno));
        close(fd);
        return RDM_FAILURE;
    }

    for (;;) {
        errno = 0;
        entry = readdir(dir);
        if (entry == NULL) {
            if (errno != 0) {
                RDMError("Unable to read cleanup entry: %s\n", strerror(errno));
                status = RDM_FAILURE;
            }
            break;
        }
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        INT32 child = openat(dirfd(dir), entry->d_name,
                             O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        INT32 flags = 0;
        if (child >= 0) {
            /* Keep traversal anchored to the opened directory, even if renamed. */
            if (rdmEmptyDirectoryFd(child) != RDM_SUCCESS) {
                status = RDM_FAILURE;
                break;
            }
            flags = AT_REMOVEDIR;
        } else if (errno == ENOENT) {
            continue;
        } else if (errno != ENOTDIR && errno != ELOOP) {
            RDMError("Unable to open cleanup entry %s: %s\n", entry->d_name, strerror(errno));
            status = RDM_FAILURE;
            break;
        }

        if (unlinkat(dirfd(dir), entry->d_name, flags) != 0 && errno != ENOENT) {
            RDMError("Unable to remove cleanup entry %s: %s\n", entry->d_name, strerror(errno));
            status = RDM_FAILURE;
            break;
        }
    }

    if (closedir(dir) != 0) {
        RDMError("Unable to close cleanup directory: %s\n", strerror(errno));
        status = RDM_FAILURE;
    }
    return status;
}

INT32 rdmEmptyFolderNoFollow(const CHAR *path)
{
    CHAR path_copy[RDM_APP_PATH_LEN];
    CHAR *saveptr = NULL;
    CHAR *component;
    INT32 fd;

    if (path == NULL || path[0] == '\0' || strlen(path) >= sizeof(path_copy)) {
        RDMError("Invalid directory cleanup path\n");
        return RDM_FAILURE;
    }
    memcpy(path_copy, path, strlen(path) + 1);
    component = strtok_r(path_copy, "/", &saveptr);
    if (component == NULL) {
        RDMError("Refusing to empty filesystem root\n");
        return RDM_FAILURE;
    }
    fd = open(path[0] == '/' ? "/" : ".", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (fd < 0) {
        RDMError("Unable to open cleanup root: %s\n", strerror(errno));
        return RDM_FAILURE;
    }

    while (component != NULL) {
        INT32 next;
        if (strcmp(component, ".") == 0 || strcmp(component, "..") == 0) {
            RDMError("Invalid cleanup path component: %s\n", component);
            close(fd);
            return RDM_FAILURE;
        }
        next = openat(fd, component, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        if (next < 0) {
            INT32 error = errno;
            close(fd);
            if (error == ENOENT) {
                return RDM_SUCCESS;
            }
            RDMError("Unable to open cleanup path component %s: %s\n",
                     component, strerror(error));
            return RDM_FAILURE;
        }
        close(fd);
        fd = next;
        component = strtok_r(NULL, "/", &saveptr);
    }

    return rdmEmptyDirectoryFd(fd);
}

/** @brief This Function finds the directory inside the directory.
 *
 *  @param[in]   pRdmAppDet    App details
 *.
 *  @retval  Returns RDM_SUCCESS on success, RDM_FAILURE otherwise.
 */
INT32 rdmListDirectory(CHAR *rdmDirToList, CHAR **pDirList, INT32 *pNumOfDir, INT32 maxDir)
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
            size_t nameLen = strlen(de->d_name);

            /* Names that do not fit cannot be valid app names */
            if (nameLen >= RDM_APPNAME_LEN) {
                continue;
            }

            if (idx >= maxDir) {
                RDMError("Too many entries in directory\n");
                status = RDM_FAILURE;
                break;
            }

            pDirList[idx] = (CHAR *)malloc(RDM_APPNAME_LEN);

            if( pDirList[idx] == NULL ){
                RDMError("Malloc for Dir List is failed");
                status = RDM_FAILURE;
                break;
            }

            memset(pDirList[idx], 0, RDM_APPNAME_LEN);

            RDMInfo("%s\n", de->d_name);
            memcpy(pDirList[idx], de->d_name, nameLen + 1);
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
