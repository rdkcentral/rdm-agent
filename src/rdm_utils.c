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
#include <fnmatch.h>
#include <limits.h>
#include <stdint.h>
#include <sys/stat.h>

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

static INT32 rdmFindPackageFilesInDir(const CHAR *path, const CHAR *pattern,
                                     CHAR **out, size_t outSize,
                                     INT32 *count, INT32 maxCount)
{
    DIR *dir = opendir(path);
    INT32 status = RDM_SUCCESS;

    if (dir == NULL) {
        RDMError("Unable to open package directory %s: %s\n", path, strerror(errno));
        return RDM_FAILURE;
    }

    while (*count < maxCount) {
        struct dirent *entry;
        struct stat info;
        CHAR fullPath[RDM_APP_PATH_LEN];
        INT32 length;

        errno = 0;
        entry = readdir(dir);
        if (entry == NULL) {
            if (errno != 0) {
                RDMError("Unable to read package directory %s: %s\n", path, strerror(errno));
                status = RDM_FAILURE;
            }
            break;
        }
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        length = snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);
        if (length < 0 || (size_t)length >= sizeof(fullPath)) {
            RDMError("Package path exceeds supported length in %s\n", path);
            status = RDM_FAILURE;
            break;
        }
        if (lstat(fullPath, &info) != 0) {
            RDMError("Unable to inspect package path %s: %s\n", fullPath, strerror(errno));
            status = RDM_FAILURE;
            break;
        }
        if (S_ISDIR(info.st_mode)) {
            status = rdmFindPackageFilesInDir(fullPath, pattern, out, outSize, count, maxCount);
            if (status != RDM_SUCCESS) {
                break;
            }
        } else if (fnmatch(pattern, entry->d_name, 0) == 0) {
            if ((size_t)length >= outSize) {
                RDMError("Package path does not fit destination: %s\n", fullPath);
                status = RDM_FAILURE;
                break;
            }
            memcpy(out[*count], fullPath, (size_t)length + 1);
            (*count)++;
        }
    }

    if (closedir(dir) != 0) {
        RDMError("Unable to close package directory %s: %s\n", path, strerror(errno));
        status = RDM_FAILURE;
    }
    return status;
}

INT32 rdmFindPackageFiles(const CHAR *path, const CHAR *pattern, CHAR **out,
                          size_t outSize, INT32 *count, INT32 maxCount)
{
    if (path == NULL || pattern == NULL || out == NULL || count == NULL ||
        outSize == 0 || maxCount <= 0 || *count < 0 || *count > maxCount) {
        RDMError("Invalid package file search argument\n");
        return RDM_FAILURE;
    }
    if (path[0] == '\0' || strlen(path) >= RDM_APP_PATH_LEN) {
        RDMError("Invalid package directory path length\n");
        return RDM_FAILURE;
    }
    for (INT32 i = *count; i < maxCount; i++) {
        if (out[i] == NULL) {
            RDMError("Missing package path buffer\n");
            return RDM_FAILURE;
        }
    }
    return rdmFindPackageFilesInDir(path, pattern, out, outSize, count, maxCount);
}

INT32 rdmListDirectory(CHAR *rdmDirToList, CHAR ***pDirList, INT32 *pNumOfDir)
{
    struct dirent *de;
    DIR *dr;
    CHAR **dirList = NULL;
    size_t capacity = 0;
    INT32 status = RDM_SUCCESS;
    INT32 idx = 0;

    if (pDirList != NULL) {
        *pDirList = NULL;
    }
    if (pNumOfDir != NULL) {
        *pNumOfDir = 0;
    }
    if (pDirList == NULL || pNumOfDir == NULL) {
        RDMError("Invalid directory list output argument\n");
        return RDM_FAILURE;
    }

    if (rdmDirToList == NULL) {
        RDMError("Invalid argument as DIR to list\n");
        return RDM_FAILURE;
    }

    dr = opendir(rdmDirToList);
    if (dr == NULL) {
        RDMError("Could not open current directory: %s\n", strerror(errno));
        return RDM_FAILURE;
    }

    for (;;) {
        errno = 0;
        de = readdir(dr);
        if (de == NULL) {
            if (errno != 0) {
                RDMError("Could not read directory: %s\n", strerror(errno));
                status = RDM_FAILURE;
            }
            break;
        }

        size_t nameLen = strlen(de->d_name);

        /* Names that do not fit cannot be valid app names */
        if (nameLen >= RDM_APPNAME_LEN) {
            continue;
        }

        if (idx == INT_MAX) {
            RDMError("Directory entry count exceeds INT32 range\n");
            status = RDM_FAILURE;
            break;
        }

        if ((size_t)idx == capacity) {
            size_t newCapacity = capacity + 20;
            CHAR **newList;

            if (newCapacity < capacity || newCapacity > SIZE_MAX / sizeof(*dirList)) {
                RDMError("Directory list allocation size overflow\n");
                status = RDM_FAILURE;
                break;
            }
            newList = (CHAR **)realloc(dirList, newCapacity * sizeof(*dirList));
            if (newList == NULL) {
                RDMError("Realloc for Dir List failed\n");
                status = RDM_FAILURE;
                break;
            }
            dirList = newList;
            capacity = newCapacity;
        }

        dirList[idx] = (CHAR *)malloc(RDM_APPNAME_LEN);
        if (dirList[idx] == NULL) {
            RDMError("Malloc for Dir List failed\n");
            status = RDM_FAILURE;
            break;
        }

        memset(dirList[idx], 0, RDM_APPNAME_LEN);
        RDMInfo("%s\n", de->d_name);
        memcpy(dirList[idx], de->d_name, nameLen + 1);
        idx += 1;
    }

    if (closedir(dr) != 0) {
        RDMError("Could not close directory: %s\n", strerror(errno));
        status = RDM_FAILURE;
    }

    if (status != RDM_SUCCESS) {
        for (INT32 index = 0; index < idx; index++) {
            free(dirList[index]);
        }
        free(dirList);
        return status;
    }

    *pDirList = dirList;
    *pNumOfDir = idx;
    return RDM_SUCCESS;
}
