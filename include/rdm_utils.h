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

#ifndef _RDM_UTILS_H_
#define _RDM_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef RDK_LOGGER_ENABLED
#include "rdk_logger.h"
#endif

/* RDM Error Codes */
#define RDM_SUCCESS         0
#define RDM_FAILURE        -1


/* RDM Log functions */
#define DEBUG_INI_NAME  "/etc/debug.ini"

#define PREFIX(format)  "[RDMAGENT] %s[%d]: " format

#ifdef RDK_LOGGER_ENABLED

extern int g_rdk_logger_enabled;

#define LOG_ERROR(format, ...)   if(g_rdk_logger_enabled) {\
    RDK_LOG(RDK_LOG_ERROR,  "LOG.RDK.RDMAGENT", format, __VA_ARGS__);\
    } else {\
    fprintf (stderr, format, __VA_ARGS__);\
}
#define LOG_WARN(format, ...)    if(g_rdk_logger_enabled) {\
    RDK_LOG(RDK_LOG_WARN,   "LOG.RDK.RDMAGENT", format, __VA_ARGS__);\
    } else {\
    fprintf (stderr, format, __VA_ARGS__);\
}
#define LOG_INFO(format, ...)    if(g_rdk_logger_enabled) {\
    RDK_LOG(RDK_LOG_INFO,   "LOG.RDK.RDMAGENT", format, __VA_ARGS__);\
    } else {\
    fprintf (stderr, format, __VA_ARGS__);\
}
#define LOG_DEBUG(format, ...)   if(g_rdk_logger_enabled) {\
    RDK_LOG(RDK_LOG_DEBUG,  "LOG.RDK.RDMAGENT", format, __VA_ARGS__);\
    } else {\
    fprintf (stderr, format, __VA_ARGS__);\
}
#define LOG_TRACE(format, ...)   if(g_rdk_logger_enabled) {\
    RDK_LOG(RDK_LOG_TRACE1, "LOG.RDK.RDMAGENT", format, __VA_ARGS__);\
    } else {\
    fprintf (stderr, format, __VA_ARGS__);\
}
#else

#define LOG_ERROR(format, ...)            fprintf(stderr, format, __VA_ARGS__)
#define LOG_WARN(format,  ...)            fprintf(stderr, format, __VA_ARGS__)
#define LOG_INFO(format,  ...)            fprintf(stderr, format, __VA_ARGS__)
#define LOG_DEBUG(format, ...)            fprintf(stderr, format, __VA_ARGS__)
#define LOG_TRACE(format, ...)            fprintf(stderr, format, __VA_ARGS__)

#endif  //RDK_LOGGER_ENABLED

#define RDMError(format, ...)  LOG_ERROR(PREFIX(format), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define RDMWarn(format,  ...)  LOG_WARN(PREFIX(format),  __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define RDMInfo(format,  ...)  LOG_INFO(PREFIX(format),  __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define RDMDebug(format, ...)  LOG_DEBUG(PREFIX(format), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define RDMTrace(format, ...)  LOG_TRACE(PREFIX(format), __FUNCTION__, __LINE__, ##__VA_ARGS__)

/* String Operations */
INT32 rdmStrSplit(CHAR *in, CHAR *tok, CHAR **out, INT32 len);
VOID  rdmQSString(CHAR *arr[], UINT32 length);
INT32 rdmStrRmDuplicate(CHAR **in, INT32 len);

/* IARM function */
INT32 rdmIARMEvntSendStatus(UINT8 status);
INT32 rdmIARMEvntSendPayload(CHAR *pkg_name, CHAR *pkg_ver,
                             CHAR *pkg_path, INT32 pkg_status);
/**
 * List directory entries, including "." and "..", skipping names too long for
 * RDM_APPNAME_LEN. The array grows in chunks of 20 pointers.
 * Pass an output pointer that does not own an existing allocation.
 * On success, the caller must free each of the *pNumOfDir strings, then *pDirList.
 * On failure, allocations are freed and valid outputs are NULL/zero.
 * The count retains its INT32 range; allocation and count overflow return failure.
 */
INT32 rdmListDirectory(CHAR *rdmDirToList, CHAR ***pDirList, INT32 *pNumOfDir);
/**
 * Recursively append matching paths to preallocated out buffers of outSize bytes.
 * Paths must fit RDM_APP_PATH_LEN, including the terminator. Directory symlinks
 * are not traversed. Returns RDM_SUCCESS when complete or maxCount is reached.
 * On failure, count includes partial results; callers must not process them.
 */
INT32 rdmFindPackageFiles(const CHAR *path, const CHAR *pattern, CHAR **out,
                          size_t outSize, INT32 *count, INT32 maxCount);
/* Empty a directory without following symlinks in its path or contents. */
INT32 rdmEmptyFolderNoFollow(const CHAR *path);
void RDMLOGInit();
#endif //_RDM_UTILS_H_
