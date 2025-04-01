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

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <cJSON.h>

#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"

static INT32 is_index(CHAR const* s)
{
    INT32 index = 0;

    if (s) {
        while (s && *s) {
            if (isdigit((INT32) *s)) {
                index *= 10;
                index += *s - 48;
            }
            else {
                index = -1;
                break;
            }
            s++;
        }
    }
    else {
        index = -1;
    }
    return index;
}

static cJSON* cJSON_Search(cJSON const* json, CHAR const* path)
{
    cJSON const* item = NULL;

    if (!json) {
        RDMError("Input argument json is NULL\n");
        return NULL;
    }

    if (!path || strlen(path) == 0) {
        RDMError("Input argument path is NULL\n");
        return NULL;
    }

    if (path[0] != '/') {

        item = cJSON_GetObjectItem(json, path);
    }
    else {
        CHAR* str = strdup(path + 1);
        CHAR* token = NULL;
        CHAR* temp = str;
        INT32 count = 0;

        item = json;
        while ((token = strtok_r(str, "/", &str)) != NULL) {
            INT32 index = -1;
            INT32 length = 0;

            if ((index = is_index(token)) != -1) {
                if (item->type != cJSON_Array) {
                    RDMError("previous object is not array\n");
                    break;
                }

                length = cJSON_GetArraySize(item);
                if (index >= length) {
                    RDMError("%d is out of range, max is %d\n", index, (length -1));
                    break;
                }

                item = cJSON_GetArrayItem(item, index);
#if 1//def RDM_BACKWARD_COMPATIBLE
                item = item->child;
            }
            else if (count == 1) {
                INT32 array_count = cJSON_GetArraySize(item);
                INT32 i;
                for (i = 0 ; i < array_count; i++) {
                    cJSON* new = cJSON_GetArrayItem(item, i);
                    CHAR *app_name = cJSON_GetArrayItem(new, 0)->string;
                    if (strcmp (app_name, token) == 0) {
                        item = cJSON_GetObjectItem(new, token);
                        break;
                    }
                }
#endif
            }
            else {
                item = cJSON_GetObjectItem(item, token);
            }
            count++;
        }
        free(temp);
    }
    return cJSON_Duplicate(item, 1);
}

static cJSON* cJSON_SearchFile(CHAR const* fname, CHAR const* path)
{
    INT32 ret = 0;
    struct stat buf;
    CHAR* text = NULL;
    cJSON* json = NULL;
    cJSON* item = NULL;
    FILE* infile = NULL;
    size_t file_size = 0;
    size_t bytes_read = 0;

    memset(&buf, 0, sizeof(buf));

    ret = stat(fname, &buf);
    if (ret == -1) {
        RDMError("failed to stat:%s. %s\n", fname, strerror(errno));
        return NULL;
    }

    file_size = buf.st_size;
    text = (CHAR *) malloc(file_size + 1);
    memset(text, 0, file_size + 1);
    infile = fopen(fname, "r");
    if (infile) {
        bytes_read = fread(text, 1, file_size, infile);
        if (bytes_read == file_size) {
            json = cJSON_Parse(text);
            item = cJSON_Search(json, path);
            cJSON_Delete(json);
        }
        else {
            RDMWarn("only read %zd of %zd from %s\n", bytes_read, file_size, fname);
        }
        fclose(infile);
    }
    else {
        RDMError("failed to open:%s. %s", fname, strerror(errno));
    }

    if (text != NULL) {
        free(text);
        text = NULL;
    }

    return item;
}

/** @brief Function returns the json file entries
 *
 *  @param[in ]  pfName  json file name
 *  @param[in ]  pPath   path
 *  @param[out]  pPath   array length
 *  @param[out]  pOut    query result
 *
 *  @return  status of the operation.
 *  @retval  Returns RDM_SUCCESS on Success, RDM_FAILURE otherwise.
 */
INT32 rdmJSONGetLen(CHAR const* pfName,
                   INT32 *pLen)
{
    INT32 ret    = RDM_SUCCESS;

    cJSON* item = NULL;

    if (!pfName) {
        RDMError("Invalid file name\n");
        return RDM_FAILURE;
    }

    if (!pLen) {
        RDMError("Invalid length pointer\n");
        return RDM_FAILURE;
    }

    *pLen = 0;
    item = cJSON_SearchFile(pfName, RDM_MANIFEST_PATH);
    if (item) {
        if (item->type == cJSON_Array) {
            *pLen = cJSON_GetArraySize(item);
        }

        cJSON_Delete(item);
    }
    else {
        ret = RDM_FAILURE;
    }

    return ret;
}

/** @brief Function parse the json file and retruns result
 *
 *  @param[in ]  pfName  json file name
 *  @param[in ]  pPath   path
 *  @param[out]  pPath   array length
 *  @param[out]  pOut    query result
 *
 *  @return  status of the operation.
 *  @retval  Returns RDM_SUCCESS on Success, RDM_FAILURE otherwise.
 */
INT32 rdmJSONQuery(CHAR const* pfName,
                   CHAR const* pPath,
                   CHAR *pOut)
{
    INT32 ret    = RDM_SUCCESS;

    cJSON* item = NULL;

    if (!pfName) {
        RDMError("Invalid file name\n");
        return RDM_FAILURE;
    }

    if (!pPath) {
        RDMError("Invalid file path\n");
        return RDM_FAILURE;
    }

    item = cJSON_SearchFile(pfName, pPath);
    if (item) {
        if (item->type == cJSON_Array) {
        }
        else {
            CHAR* s;
            CHAR* t;

            s = cJSON_Print(item);
            t = NULL;
            if (s) {
                size_t len = strlen(s);
                if (len > 0) {
                    t = s;
                    if (t[len - 1] == '"')
                        t[len - 1] = '\0';
                    if (t[0] == '"')
                        t++;
                }
            }
            strncpy(pOut, t, strlen(t));
	    pOut[strlen(t)] = '\0';  // Ensure null termination
            free(s);
        }
        cJSON_Delete(item);
    }
    else {
        ret = RDM_FAILURE;
    }

    return ret;
}
/** @brief Function parse the json file and gets app name lists
 *
 *  @param[in ]  idx        index
 *  @param[out]  pRdmAppDet App names
 *
 *  @return  status of the operation.
 *  @retval  Returns RDM_SUCCESS on Success, RDM_FAILURE otherwise.
 */
INT32 rdmJSONGetAppNames(INT32          idx,
                         CHAR           *pAppName)
{
    INT32 ret = RDM_SUCCESS;
    CHAR  json_path[RDM_JSONPATH_LEN] = {0};

    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%d/%s", RDM_MANIFEST_PATH, idx, RDM_JSONQ_APPNAME);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pAppName);
    if(ret) {
        RDMWarn("Unable to get App Name\n");
        ret = RDM_FAILURE;
    }

    return ret;
}
/** @brief Function parse the json file and gets app details
 *
 *  @param[in ]  idx        index
 *  @param[out]  pRdmAppDet App details
 *
 *  @return  status of the operation.
 *  @retval  Returns RDM_SUCCESS on Success, RDM_FAILURE otherwise.
 */
INT32 rdmJSONGetAppDetID(INT32          idx,
                         RDMAPPDetails *pRdmAppDet)
{
    INT32 ret = RDM_SUCCESS;
    CHAR  json_path[RDM_JSONPATH_LEN] = {0};
    CHAR  jsonq[RDM_JSONQ_LEN]        = {0};

    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%d/%s", RDM_MANIFEST_PATH, idx, RDM_JSONQ_APPNAME);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pRdmAppDet->app_name);
    if(ret) {
        RDMWarn("Unable to get App Name\n");
    }

    pRdmAppDet->dwld_on_demand = 0;
    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%d/%s", RDM_MANIFEST_PATH, idx, RDM_JSONQ_DOND);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, jsonq);
    if(ret) {
        RDMWarn("Unable to get dwld_on_demand\n");
        pRdmAppDet->dwld_on_demand = 0;
    }
    else if(!strcmp(jsonq, "yes")) {
        pRdmAppDet->dwld_on_demand = 1;
    }

    memset(jsonq, 0, sizeof(jsonq));
    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%d/%s", RDM_MANIFEST_PATH, idx, RDM_JSONQ_DMC);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pRdmAppDet->dwld_method_controller);
    if(ret) {
        RDMWarn("Unable to get dwld_method_controller\n");
        strncpy(pRdmAppDet->dwld_method_controller, "None", sizeof(pRdmAppDet->dwld_method_controller));
    }

    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%d/%s", RDM_MANIFEST_PATH, idx, RDM_JSONQ_PKGNAME);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pRdmAppDet->pkg_name);
    if(ret) {
        RDMWarn("Unable to get package Name\n");
    }

    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%d/%s", RDM_MANIFEST_PATH, idx, RDM_JSONQ_PKGTYPE);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pRdmAppDet->pkg_type);
    if(ret) {
        RDMWarn("Unable to get package type\n");
    }

    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%d/%s", RDM_MANIFEST_PATH, idx, RDM_JSONQ_VER);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pRdmAppDet->pkg_ver);
    if(ret) {
        RDMWarn("Unable to get package version\n");
        strncpy(pRdmAppDet->pkg_ver, "NA", sizeof(pRdmAppDet->pkg_ver));
    }

    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%d/%s", RDM_MANIFEST_PATH, idx, RDM_JSONQ_APPSIZE);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pRdmAppDet->app_size);
    if(ret) {
        RDMWarn("Unable to get app size\n");
    }
    else {
        if(strchr(pRdmAppDet->app_size, 'K') ||
           strchr(pRdmAppDet->app_size, 'k')) {
            pRdmAppDet->app_size_kb = atof(pRdmAppDet->app_size);
            pRdmAppDet->app_size_mb = pRdmAppDet->app_size_kb/1024;
        }
        else {
            pRdmAppDet->app_size_mb = atof(pRdmAppDet->app_size);
            pRdmAppDet->app_size_kb = pRdmAppDet->app_size_mb*1024;
        }
    }

    pRdmAppDet->is_versioned = 0;
    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%d/%s", RDM_MANIFEST_PATH, idx, RDM_JSONQ_ISVER);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, jsonq);
    if(ret) {
        RDMWarn("Unable to get version details\n");
    }
    else if(!strcmp(jsonq, "true")) {
        pRdmAppDet->is_versioned = 1;
    }

    return ret;
}

/** @brief Function parse the json file and gets app details
 *
 *  @param[in ]  pName      App Name
 *  @param[out]  pRdmAppDet App details
 *
 *  @return  status of the operation.
 *  @retval  Returns RDM_SUCCESS on Success, RDM_FAILURE otherwise.
 */
INT32 rdmJSONGetAppDetName(CHAR          *pName,
                           RDMAPPDetails *pRdmAppDet)
{
    INT32 ret = RDM_SUCCESS;
    CHAR  json_path[RDM_JSONPATH_LEN] = {0};
    CHAR  jsonq[RDM_JSONQ_LEN]        = {0};

    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%s/%s", RDM_MANIFEST_PATH, pName, RDM_JSONQ_APPNAME);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pRdmAppDet->app_name);
    if(ret) {
        RDMError("Unable to get App Name\n");
        return ret;
    }

    pRdmAppDet->dwld_on_demand = 0;
    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%s/%s", RDM_MANIFEST_PATH, pName, RDM_JSONQ_DOND);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, jsonq);
    if(ret) {
        RDMWarn("Unable to get dwld_on_demand\n");
        pRdmAppDet->dwld_on_demand = 0;
    }
    else if(!strcmp(jsonq, "yes")) {
        pRdmAppDet->dwld_on_demand = 1;
    }

    memset(jsonq, 0, sizeof(jsonq));
    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%s/%s", RDM_MANIFEST_PATH, pName, RDM_JSONQ_DMC);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pRdmAppDet->dwld_method_controller);
    if(ret) {
        RDMWarn("Unable to get dwld_method_controller\n");
        strncpy(pRdmAppDet->dwld_method_controller, "None", sizeof(pRdmAppDet->dwld_method_controller) - 1);
	pRdmAppDet->dwld_method_controller[sizeof(pRdmAppDet->dwld_method_controller) - 1] = '\0'; //Null terminate the string
    }

    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%s/%s", RDM_MANIFEST_PATH, pName, RDM_JSONQ_PKGNAME);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pRdmAppDet->pkg_name);
    if(ret) {
        RDMError("Unable to get package Name\n");
        return ret;
    }

    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%s/%s", RDM_MANIFEST_PATH, pName, RDM_JSONQ_PKGTYPE);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pRdmAppDet->pkg_type);
    if(ret) {
        RDMWarn("Unable to get package type\n");
    }

    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%s/%s", RDM_MANIFEST_PATH, pName, RDM_JSONQ_APPSIZE);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, pRdmAppDet->app_size);
    if(ret) {
        RDMWarn("Unable to get app size\n");
    }
    else {
        if(strchr(pRdmAppDet->app_size, 'K') ||
           strchr(pRdmAppDet->app_size, 'k')) {
            pRdmAppDet->app_size_kb = atof(pRdmAppDet->app_size);
            pRdmAppDet->app_size_mb = pRdmAppDet->app_size_kb/1024;
        }
        else {
            pRdmAppDet->app_size_mb = atof(pRdmAppDet->app_size);
            pRdmAppDet->app_size_kb = pRdmAppDet->app_size_mb*1024;
        }
    }

    pRdmAppDet->is_versioned = 0;
    snprintf(json_path, RDM_JSONPATH_LEN, "%s/%s/%s", RDM_MANIFEST_PATH, pName, RDM_JSONQ_ISVER);
    ret = rdmJSONQuery(RDM_MANIFEST, json_path, jsonq);
    if(ret) {
        RDMWarn("Unable to get version details\n");
        ret = RDM_SUCCESS;
    }
    else if(!strcmp(jsonq, "true")) {
        pRdmAppDet->is_versioned = 1;
    }

    return ret;
}
