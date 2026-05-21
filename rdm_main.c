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
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include <curl/curl.h>
#ifndef GTEST_ENABLE
#include "json_parse.h"
#include <downloadUtil.h>
#ifdef IARMBUS_SUPPORT
#include "rdmMgr.h"
#else
#define RDM_PKG_UNINSTALL 10
#endif
#endif
#ifdef GTEST_ENABLE
#include "unittest/mocks/rdmMgr.h"
#endif
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_download.h"
#include "rdm_downloadutils.h"
#include "rdm_curldownload.h"
#include "rdm_openssl.h"
#include "rdm_rbus.h"
#include "rdm_usbinstall.h"

/** @brief Initializes all modules
 *
 *  @param[in]  prdmHandle  RDM handle
 *
 *  @return  Returns the status of the operation.
 *  @retval  Returns RDM_SUCCESS on Success, RDM_FAILURE otherwise.
 */
INT32 rdmInit(RDMHandle *prdmHandle)
{
    INT32 ret = RDM_SUCCESS;

    if(prdmHandle == NULL) {
        RDMError("rdm init handle is NULL");
        return RDM_FAILURE;
    }

#ifdef T2_EVENT_ENABLED
    t2_init("rdmagent");
#endif

    prdmHandle->pApp_det = (RDMAPPDetails *)malloc(sizeof(RDMAPPDetails));
    if(prdmHandle->pApp_det == NULL) {
        RDMError("Failed to allocate memory\n");
        return RDM_FAILURE;
    }
    memset(prdmHandle->pApp_det, 0, sizeof(RDMAPPDetails));

    ret = rdmRbusInit(&prdmHandle->pRbusHandle, RDM_INIT_NAME);
    if (ret != RDM_SUCCESS) {
        RDMError("Failed to init rdbus\n");
    }

    return ret;
}

/** @brief De-Initializes all modules
 *
 *  @param[in]  prdmHandle  RDM handle
 *
 *  @return  None.
 *  @retval  None.
 */
VOID rdmUnInit(RDMHandle *prdmHandle)
{

#ifdef T2_EVENT_ENABLED
    t2_uninit();
#endif

    rdmRbusUnInit(prdmHandle->pRbusHandle);
    if(prdmHandle->pApp_det) {
        free(prdmHandle->pApp_det);
    }
}

/**
 * @brief Helper function to extract and parse bundles for a given type
 * 
 * @param[in]  input_str       Input string to search in
 * @param[in]  search_prefix   Prefix to search for (e.g., "dlCertBundle=")
 * @param[in]  type_prefix     Type prefix to add (e.g., "cert:")
 * @param[out] parsed_list     Output buffer to append parsed bundles
 * @param[in]  max_len         Maximum length of output buffer
 */
static VOID rdmExtractBundleType(const CHAR *input_str, 
                                  const CHAR *search_prefix,
                                  const CHAR *type_prefix,
                                  CHAR *parsed_list,
                                  INT32 max_len)
{
    CHAR *bundle_start = strstr(input_str, search_prefix);
    if (!bundle_start) {
        return;
    }
    
    bundle_start += strlen(search_prefix);
    CHAR bundle_list[MAX_BUFF_SIZE] = {0};
    
    /* Find end of bundle list (either | or end of string) */
    CHAR *pipe_pos = strchr(bundle_start, '|');
    if (pipe_pos) {
        strncpy(bundle_list, bundle_start, pipe_pos - bundle_start);
        bundle_list[pipe_pos - bundle_start < sizeof(bundle_list) ? pipe_pos - bundle_start : sizeof(bundle_list) - 1] = '\0';
    } else {
        strncpy(bundle_list, bundle_start, sizeof(bundle_list) - 1);
        bundle_list[sizeof(bundle_list) - 1] = '\0';
    }
    
    /* Parse comma-separated bundle names using strtok_r for reentrancy */
    CHAR *saveptr = NULL;
    CHAR *bundle_token = strtok_r(bundle_list, ",", &saveptr);
    while (bundle_token != NULL) {
        /* Trim leading/trailing spaces */
        while (*bundle_token == ' ') bundle_token++;
        
        if (strlen(bundle_token) > 0) {
            if (strlen(parsed_list) + strlen(type_prefix) + strlen(bundle_token) + 2 < max_len) {
                if (strlen(parsed_list) > 0) {
                    strcat(parsed_list, " ");
                }
                strcat(parsed_list, type_prefix);
                strcat(parsed_list, bundle_token);
            }
        }
        bundle_token = strtok_r(NULL, ",", &saveptr);
    }
}

/**
 * @brief Parse bundle list format and extract individual bundles
 * 
 * Parses input like "dlCertBundle=cert1,cert2|dlAppBundle=app1,app2"
 * and converts to space-separated list with prefixes: "cert:cert1 cert:cert2 app:app1 app:app2"
 * 
 * @param[in]  bundle_list_input  Input bundle list string
 * @param[out] parsed_list        Output buffer for parsed list
 * @param[in]  max_len            Maximum length of output buffer
 * @return     1 if bundle format detected and parsed successfully,
 *             0 if no bundle format detected (not an error),
 *            -1 on actual error (NULL parameters)
 */
static INT32 rdmParseBundleList(const CHAR *bundle_list_input, CHAR *parsed_list, INT32 max_len)
{
    if (!bundle_list_input || !parsed_list || max_len <= 0) {
        return -1;
    }
    
    /* Check if input contains bundle format markers */
    if (strstr(bundle_list_input, "dlCertBundle=") == NULL && 
        strstr(bundle_list_input, "dlAppBundle=") == NULL) {
        /* No bundle format detected, return as-is */
        return 0;
    }
    
    memset(parsed_list, 0, max_len);
    
    RDMInfo("Parsing bundle list: %s\n", bundle_list_input);
    
    /* Extract and add cert bundles if present - rdmExtractBundleType doesn't modify input */
    rdmExtractBundleType(bundle_list_input, "dlCertBundle=", "cert:", parsed_list, max_len);
    
    /* Extract and add app bundles if present - can safely use same input */
    rdmExtractBundleType(bundle_list_input, "dlAppBundle=", "app:", parsed_list, max_len);
    
    RDMInfo("Parsed bundle list: %s\n", parsed_list);
    return 1;
}

static VOID rdmHelp()
{
    RDMInfo("Usage:\n");
    RDMInfo("To Install apps from manifset : rdm\n");
    RDMInfo("To Install single app         : rdm -a <app_name>\n");
    RDMInfo("To Install from USB           : rdm -u <usb_path>\n");
    RDMInfo("To Install Versioned app(s)   : rdm -v <app:ver[,app:ver]>\n");
    RDMInfo("Other options\n");
    RDMInfo("-b - for broadband devices\n");
    RDMInfo("-o - for OSS\n");
    RDMInfo("To Print help                 : rdm -h\n");
}

/* Description: Use for sending telemetry Log
 * @param marker: use for send marker details
 * @return : void
 * */
void t2CountNotify(char *marker, int val) {
#ifdef T2_EVENT_ENABLED
    t2_event_d(marker, val);
#endif
}

void t2ValNotify( char *marker, char *val )
{
#ifdef T2_EVENT_ENABLED
    t2_event_s(marker, val);
#endif
}

/** @brief Main function
 *
 *  @param[in]  argc   No. of arguments
 *  @param[in]  argv   arguments array
 *
 *  @return  status.
 *  @retval  status.
 */
#ifndef GTEST_ENABLE
int main(int argc, char* argv[])
{
    INT32 download_all        = 0;
    INT32 idx                 = 0;
    INT32 download_status     = 0;
    INT32 download_singleapp  = 0;
    INT32 download_versionedapp  = 0;
    INT32 usb_install         = 0;
    CHAR  *usb_path           = NULL;
    CHAR  *app_name           = NULL;
    INT32  opt_c              = 0;
    INT32  ret                = RDM_SUCCESS;
    INT32  is_broadband       = 0;
    INT32  is_oss             = 0;
    INT32  bFsCheck           = 1;
    CHAR   rfc_app[256]       = {0};
    RDMAPPDetails *pApp_det   = NULL;
    RDMHandle     *prdmHandle = NULL;

    RDMLOGInit();
    if(argc == 1) {
        RDMInfo("download all the apps mentioned in rdm-manifest.json file\n");
        download_all = 1;
    }
    else {
        while ((opt_c = getopt (argc, argv, "a:u:v:x:hbo")) != -1) {
            switch (opt_c)
            {
                case 'a':
                    download_singleapp = 1;
                    app_name = optarg;
                    break;
                case 'u':
                    usb_install = 1;
                    usb_path = optarg;
                    break;
                case 'b':
                    is_broadband = 1;
                    break;
                case 'o':
                    is_oss = 1;
                    break;
                case 'v':
                    download_versionedapp = 1;
                    app_name = optarg;
                    break;
                case 'x':
                    is_broadband = 1;
                    download_versionedapp = 1;
                    app_name = optarg;
                    break;
                case 'h':
                default :
                    rdmHelp();
                    return RDM_FAILURE;
            }
        }
    }

    pApp_det = (RDMAPPDetails *)malloc(sizeof(RDMAPPDetails));
    if(pApp_det == NULL) {
        RDMError("Failed to allocate memory\n");
        return RDM_FAILURE;
    }
    memset(pApp_det, 0, sizeof(RDMAPPDetails));

    /* handle allocation */
    prdmHandle = malloc(sizeof(RDMHandle));
    if(prdmHandle == NULL) {
        RDMError("Rbus Handle allocation is NULL\n");
        return RDM_FAILURE;
    }

    ret = rdmInit(prdmHandle);
    if(ret != RDM_SUCCESS) {
        RDMError("Rbus Init is failure\n");
        goto error2;
    }

    pApp_det = prdmHandle->pApp_det;

    if(download_all) {
        INT32 len = 0;
        INT32 app_rfc_status = 0;

        ret = rdmJSONGetLen(RDM_MANIFEST, &len);
        if(ret || len == 0) {
            RDMError("Invalid json file\n");
            download_status = RDM_FAILURE;
            goto error1;
        }

        do {

            /* Prevent out-of-range before parsing */
            if (idx >= len) {
                RDMWarn("Reached end of manifest file (idx=%d, len=%d)\n", idx, len);
                break;
            }

            /* reset app details */
            memset(pApp_det, 0, sizeof(RDMAPPDetails));

            /* Parse the JSON file and get the app details */
            ret = rdmJSONGetAppDetID(idx, pApp_det);

            pApp_det->bFsCheck = bFsCheck;
            pApp_det->is_oss   = is_oss;

            /* Update the app paths */
            ret = rdmUpdateAppDetails(prdmHandle, pApp_det, is_broadband);
            if(ret) {
                RDMError("Failed to get downloadpath for index %d\n", idx);
                break;
            }
            rdmPrintAppDetails(pApp_det);

            if(pApp_det->dwld_on_demand) {
                RDMInfo("dwld_on_demand set to yes!!! Check RFC value of the APP to be downloaded\n");

                RDMInfo("pApp_det->dwld_method_controller value is %s\n",pApp_det->dwld_method_controller);
		        if(!strcmp(pApp_det->dwld_method_controller, "RFC")) {
                    snprintf(rfc_app, sizeof(rfc_app), "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.%s.Enable",pApp_det->app_name);

                    ret =  rdmRbusGetRfc(prdmHandle->pRbusHandle,
                                         rfc_app,
                                         &app_rfc_status);
                    RDMInfo("app_rfc_status : %d\n", app_rfc_status);
                    if(ret != RDM_SUCCESS || app_rfc_status == 0) {
                        RDMWarn("APP RFC is not enabled, skipping the download for:App:%d=>yes=>%s\n",idx, pApp_det->app_name);
                        idx++; /*advance idx before continue to avoid re-processing same entry */
                        continue;
                    }
                }
		        else
		        {
		            RDMWarn("RFC is not defined/empty  skipping the download for:App:%d=>yes=>%s\n",idx, pApp_det->app_name);
                    idx++; /*advance idx before continue to avoid re-processing same entry */
                    continue;	
		        }
            }
            RDMInfo("Start the download of App: %s\n", pApp_det->app_name);

            ret = rdmDownloadApp(pApp_det, &download_status);
            if(ret) {
                RDMError("Failed to download the App: %s, status: %d\n", pApp_det->app_name, download_status);
                break;
            }

            /* advance index after successful handling of current entry */
            idx++;

            if(idx >= len) {
                RDMWarn("Reached end of manifest file\n");
                break;
            }

        } while(1);

        RDMInfo("Download of all Apps completed with status: %d\n", ret);

    } //if(download_all)

    else if (download_singleapp) {
        INT32 app_status = 0;

        RDMInfo("Install the app: %s\n", app_name);

        /* Parse the JSON file and get the app details */
        ret = rdmJSONGetAppDetName(app_name, pApp_det);

        /* Update the app paths */
        rdmUpdateAppDetails(prdmHandle, pApp_det, is_broadband);

        rdmPrintAppDetails(pApp_det);

        //app_status = checkAppDownload app_name
        if(app_status == 0) {
            RDMWarn("%s application already downloaded\n", pApp_det->app_name);
        }
        else {
            RDMInfo("Retrying download for %s application...\n", pApp_det->app_name);

            ret = rdmDownloadApp(pApp_det, &download_status);
            if(ret) {
                RDMError("Failed to download the App: %s, status: %d\n", pApp_det->app_name, download_status);
            }

            RDMInfo("Download completed for App %s with status=%d\n", pApp_det->app_name, download_status);
            t2ValNotify("RDM_INFO_AppDownloadComplete", pApp_det->app_name );
        }
    } //else if (download_singleapp)

    else if(usb_install) {
        RDMInfo("Install App from USB path: %s\n", usb_path);

        ret = rdmUSBInstall(prdmHandle, pApp_det, usb_path);
        if(ret) {
            RDMError("Failed to Install APP from USB: %s\n", usb_path);
        }
    } //usb_install

    else if(download_versionedapp) {
	    RDMInfo("Starting versioned app download for: %s\n", app_name ? app_name : "(null)");
	    CHAR parsed_bundle_list[MAX_BUFF_SIZE * 2] = {0};
	    CHAR *bundle_list_to_use = app_name;
	    
	    /* Try to parse bundle list format */
	    INT32 parse_result = rdmParseBundleList(app_name, parsed_bundle_list, sizeof(parsed_bundle_list));
	    if (parse_result == 1) {
		    /* Successfully parsed bundle list format */
		    bundle_list_to_use = parsed_bundle_list;
		    RDMInfo("Using parsed bundle list: %s\n", bundle_list_to_use);
	    } else if (parse_result == 0) {
		    /* No bundle format detected, using original format */
		    RDMInfo("No bundle format detected, using original input: %s\n", app_name ? app_name : "(null)");
	    } else {
		    /* Error occurred during parsing */
		    RDMError("Error parsing bundle list, using original input: %s\n", app_name ? app_name : "(null)");
	    }
	    
	    /* Process each bundle in the list */
	    CHAR bundle_list_copy[MAX_BUFF_SIZE * 2] = {0};
	    strncpy(bundle_list_copy, bundle_list_to_use, sizeof(bundle_list_copy) - 1);
	    bundle_list_copy[sizeof(bundle_list_copy) - 1] = '\0';
	    
	    CHAR *saveptr = NULL;
	    CHAR *bundle_token = strtok_r(bundle_list_copy, " ,", &saveptr);
	    while (bundle_token != NULL) {
		    /* Reset app details for each bundle */
		    memset(pApp_det, 0, sizeof(RDMAPPDetails));
		    pApp_det->bFsCheck = bFsCheck;
		    pApp_det->is_oss = is_oss;
		    
		    char result[20] = {0};
		    CHAR current_bundle[MAX_BUFF_SIZE] = {0};
		    strncpy(current_bundle, bundle_token, sizeof(current_bundle) - 1);
		    current_bundle[sizeof(current_bundle) - 1] = '\0';
		    
		    RDMInfo("Processing bundle: %s\n", current_bundle);
		    
		    /* Check if bundle has cert: or app: prefix */
		    CHAR *bundle_name = current_bundle;
		    if (strncmp(current_bundle, "cert:", 5) == 0) {
			    bundle_name = current_bundle + 5;
			    strncpy(pApp_det->bundle_type, "cert", sizeof(pApp_det->bundle_type) - 1);
			    pApp_det->bundle_type[sizeof(pApp_det->bundle_type) - 1] = '\0';
			    RDMInfo("Detected cert bundle type for %s\n", bundle_name);
		    } else if (strncmp(current_bundle, "app:", 4) == 0) {
			    bundle_name = current_bundle + 4;
			    strncpy(pApp_det->bundle_type, "app", sizeof(pApp_det->bundle_type) - 1);
			    pApp_det->bundle_type[sizeof(pApp_det->bundle_type) - 1] = '\0';
			    RDMInfo("Detected app bundle type for %s\n", bundle_name);
		    }
		    /* No prefix - bundle_type will remain empty and can be set later if needed */
		    
		    /* Parse version from bundle name (format: name:version) */
		    char *ver = strchr(bundle_name, ':');
		    if (ver != NULL) {
			    strncpy(pApp_det->pkg_ver, ver + 1, sizeof(pApp_det->pkg_ver) - 1);
			    pApp_det->pkg_ver[sizeof(pApp_det->pkg_ver) - 1] = '\0';
		    } else {
			    /* No version specified, use NA */
			    strncpy(pApp_det->pkg_ver, "NA", sizeof(pApp_det->pkg_ver) - 1);
			    pApp_det->pkg_ver[sizeof(pApp_det->pkg_ver) - 1] = '\0';
			    RDMWarn("No version specified for %s, using NA\n", bundle_name);
		    }
		    sscanf(bundle_name, "%[^:]", result);
		    strncpy(pApp_det->app_name, result, sizeof(pApp_det->app_name) - 1);
		    pApp_det->app_name[sizeof(pApp_det->app_name) - 1] = '\0';
		    
		    RDMInfo("Parsed app_name: %s, pkg_ver: %s\n", pApp_det->app_name, pApp_det->pkg_ver);
		    snprintf(pApp_det->pkg_name, sizeof(pApp_det->pkg_name), "%s_%s-signed.tar", result, pApp_det->pkg_ver);
		    RDMInfo("pkg_name_signed = %s\n", pApp_det->pkg_name);
		    pApp_det->is_versioned_app = 1;
		    
		    /* Update App paths */
		    rdmUpdateAppDetails(prdmHandle, pApp_det, is_broadband);
		    rdmPrintAppDetails(pApp_det);
		    
		    /* Download the bundle */
		    ret = rdmDownloadApp(pApp_det, &download_status);
		    if(ret) {
			    RDMError("Failed to download the App: %s, status: %d\n", pApp_det->app_name, download_status);
		    } else {
			    RDMInfo("Download completed for App %s with status=%d\n", pApp_det->app_name, download_status);
			    t2ValNotify("RDM_INFO_AppDownloadComplete", pApp_det->app_name);
		    }
		    
		    /* Move to next bundle */
		    bundle_token = strtok_r(NULL, " ,", &saveptr);
	    }
    }

error1:

    if(download_status == 0) {
        RDMInfo("App download success, sending status as %d\n", download_status);
        t2CountNotify("RDM_INFO_AppDownloadSuccess", 1);
        if(pApp_det->is_versioned_app) {
            RDMInfo("Post Installation Successful for %s\n", pApp_det->app_name);
#ifndef IARMBUS_SUPPORT
            if (prdmHandle && prdmHandle->pRbusHandle) {
                ret = rdmRbusSetDownloadStatus(prdmHandle->pRbusHandle, true);
                RDMInfo("Updating Download status");
                if (ret != RDM_SUCCESS) {
                    RDMError("Failed to set download status via rbus\n");
                }
            }
#endif
            return download_status;

	} else {
            rdmUnInstallApps(prdmHandle, is_broadband);
            ret = rdmIARMEvntSendStatus(RDM_PKG_UNINSTALL);
            if(ret) {
               RDMError("Failed to send the IARM event\n");
            }
	}
    }
    else {
        RDMInfo("App download failed, sending status as %d\n", download_status);
	rdmUnInstallApps(prdmHandle, is_broadband);
	ret = rdmIARMEvntSendStatus(RDM_PKG_UNINSTALL);
        if(ret) {
            RDMError("Failed to send the IARM event\n");
        }
    }

    ret = rdmIARMEvntSendStatus(download_status);
    if(ret) {
        RDMError("Failed to send the IARM event\n");
    }

error2:
    if(prdmHandle != NULL) {
        rdmUnInit(prdmHandle);
    }

    if(prdmHandle != NULL) {
        free(prdmHandle);
    }

    return ret;
}
#endif
