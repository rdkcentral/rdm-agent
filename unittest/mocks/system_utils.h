
/*
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

#ifndef VIDEO_UTILS_SYSTEM_UTILS_H_
#define VIDEO_UTILS_SYSTEM_UTILS_H_

#include "rdk_fwdl_utils.h"
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_jsonquery.h"
#include <cjson/cJSON.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_OUT_BUFF_POPEN 4096
#define RDK_API_SUCCESS 0
#define RDK_API_FAILURE -1
#define RDK_FILEPATH_LEN    128
#define RDK_APP_PATH_LEN   256
#define RDK_MB_SIZE        (1024 * 1024)
#define DIRECT_BLOCK_FILENAME "/tmp/.lastdirectfail_cdl"

typedef enum {
    T2ERROR_SUCCESS,
    T2ERROR_FAILURE
} T2ERROR;

typedef struct credential {
    char cert_name[64];
    char cert_type[16];
    char key_pas[32];
}MtlsAuth_t;

typedef struct CommonDownloadData {
    void* pvOut;
    size_t datasize;        // data size
    size_t memsize;         // allocated memory size (if applicable)
} DownloadData;

typedef struct hashParam {
    char *hashvalue;
    char *hashtime;
}hashParam_t;

typedef struct filedwnl {
        char *pPostFields;
        char *pHeaderData;
        DownloadData *pDlData;
        DownloadData *pDlHeaderData;
        int chunk_dwnl_retry_time;
        char url[128];
        char pathname[64];
        bool sslverify;
        hashParam_t *hashData;
}FileDwnl_t;

typedef cJSON   JSON;

/** Description: File present check.
 *
 *  @param file_name: The pointer to hold the file name.
 *  @return The function return status is int.
 *          RDK_API_SUCCESS 0
 *      RDK_API_FAILURE -1
 */
int filePresentCheck(const char *file_name);


/** Description: Execute the Linux commands from process and return the output back.
 *
 *  @param cmd the pointer to hold the linux command to be executed.
 *  @param output the pointer to hold the command output.
 *  @size_buff: Buffer size of output parameter.
 *  @return The function return status int.
 *          RDK_API_SUCCESS 0
 *          RDK_API_FAILURE -1
 *  NOTE: The size_buff should not less than equal zero or greater than 4096 byte
 */
int cmdExec(const char *cmd, char *output, unsigned int size_buff);

int getFileSize(const char *file_name);
int logFileData(const char *file_path);
int createDir(const char *dirname);
int eraseFolderExcePramaFile(const char *folder, const char* file_name, const char *model_num);

int createFile(const char *file_name);
int eraseTGZItemsMatching(const char *folder, const char* file_name);
size_t GetHwMacAddress( char *iface, char *pMac, size_t szBufSize );
/* Filesystem functions */
UINT32 getFreeSpace(const char *path);
INT32 checkFileSystem(const char *path);
int  findSize(char *fileName);
int  findFile(char *dir, char *search);
INT32  findPFile(const char *dir, const char *search, char *out);
int  findPFileAll(char *path, char *search, char **out, int *found_t, int max_list);
INT32  emptyFolder(const char *dir);
int  removeFile(char *filePath);
int  copyFiles(char *src, char *dst);
int  fileCheck(char *pFilepath);
const char* getExtension(char *filename);
char*  getPartStr(char *fullpath, char *delim);
char*  getPartChar(char *fullpath, char delim);
int  folderCheck(char *path);
int  tarExtract(char *in_file, char *out_path);
int  arExtract(char *in_file, char *out_path);
void   copyCommandOutput (char *cmd, char *out, int len);
unsigned int getFileLastModifyTime(char *file_name);
time_t getCurrentSysTimeSec(void);
int  getProcessID(char *in_file, char *out_path);
/* String Operations */
int strSplit(char *in, char *tok, char **out, int len);
void  qsString(char *arr[], unsigned int length);
int strRmDuplicate(char **in, int len);
int isDataInList(char **pList,char *pData,int count);
void getStringValueFromFile(char* path, char* strtokvalue, char* string, char* outValue);
JSON *ParseJsonStr(char *pJsonStr);
size_t GetJsonVal( JSON *pJson, char *pValToGet, char *pOutputVal, size_t maxlen );
int FreeJson(JSON *pJson);
void t2CountNotify(char *marker, int val);
void t2ValNotify(char *marker, char *val);
void t2_init(char *component);
void t2_uninit(void);
T2ERROR t2_event_s(char* marker, char* value);
T2ERROR t2_event_d(char* marker, int value);

#ifdef __cplusplus
}
#endif

#endif /* VIDEO_UTILS_SYSTEM_UTILS_H_ */

