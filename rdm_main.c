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
    rdmRbusUnInit(prdmHandle->pRbusHandle);
    if(prdmHandle->pApp_det) {
        free(prdmHandle->pApp_det);
    }
}

static VOID rdmHelp()
{
    RDMInfo("Usage:\n");
    RDMInfo("To Install apps from manifset : rdm\n");
    RDMInfo("To Install single app         : rdm -a <app_name>\n");
    RDMInfo("To Install from USB           : rdm -u <usb_path>\n");
    RDMInfo("To Install Versioned app      : rdm -v <app_name>\n");
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
        while ((opt_c = getopt (argc, argv, "a:u:v:hbo")) != -1) {
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
                case 'h':
                default :
                    rdmHelp();
                    return RDM_FAILURE;
            }
        }
    }

    if(is_broadband) {
        bFsCheck = 0;
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
            /* reset app details */
            memset(pApp_det, 0, sizeof(RDMAPPDetails));

            /* Parse the JSON file and get the app details */
            ret = rdmJSONGetAppDetID(idx, pApp_det);

            pApp_det->bFsCheck = bFsCheck;
            pApp_det->is_oss   = is_oss;

            /* Update the app paths */
            ret = rdmUpdateAppDetails(prdmHandle, pApp_det, is_broadband);
            if(ret) {
                RDMError("Failed to get downloadpath\n");
                break;
            }
            rdmPrintAppDetails(pApp_det);

            idx++;

            if(pApp_det->dwld_on_demand) {
                RDMInfo("dwld_on_demand set to yes!!! Check RFC value of the APP to be downloaded\n");

                if(!strcmp(pApp_det->dwld_method_controller, "RFC")) {
                    snprintf(rfc_app, sizeof(rfc_app), "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.%s.Enable",pApp_det->app_name);

                    ret =  rdmRbusGetRfc(prdmHandle->pRbusHandle,
                                         rfc_app,
                                         &app_rfc_status);

                    if(ret == RDM_SUCCESS && app_rfc_status == 0) {
                        RDMWarn("APP RFC is not enabled, skipping the download for:App:%d=>yes=>%s\n",idx-1, pApp_det->app_name);
                        continue;
                    }
                }
            }
            RDMInfo("Start the download of App: %s\n", pApp_det->app_name);

            ret = rdmDownloadApp(pApp_det, &download_status);
            if(ret) {
                RDMError("Failed to download the App: %s, status: %d\n", pApp_det->app_name, download_status);
                break;
            }

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
	    char result[20];

	    RDMInfo("Install App from custom path: %s\n", app_name);
	    char *ver = strchr(app_name, ':');
	    if (ver != NULL) {
		    strncpy(pApp_det->pkg_ver, ver + 1, sizeof(pApp_det->pkg_ver) - 1);
		    pApp_det->pkg_ver[sizeof(pApp_det->pkg_ver) - 1] = '\0';
	    }
	    sscanf(app_name, "%[^:]", result);
	    strncpy(pApp_det->app_name, result, sizeof(pApp_det->app_name) - 1);
            pApp_det->app_name[sizeof(pApp_det->app_name) - 1] = '\0';
	    snprintf(pApp_det->pkg_name, sizeof(pApp_det->pkg_name), "%s_%s-signed.tar", result, pApp_det->pkg_ver);
	    RDMInfo("pkg_name_signed = %s\n", pApp_det->pkg_name);
	    pApp_det->is_versioned_app = 1;

	    /* Update App paths */
	    rdmUpdateAppDetails(prdmHandle, pApp_det, is_broadband);
	    rdmPrintAppDetails(pApp_det);
	    ret = rdmDownloadApp(pApp_det, &download_status);
	    if(ret) {
		    RDMError("Failed to download the App: %s, status: %d\n", pApp_det->app_name,download_status);
            } else {
	        RDMInfo("Download of %s App completed with status=%d\n", pApp_det->app_name, download_status);
            }
    }

error1:

    if(download_status == 0) {
        RDMInfo("App download success, sending status as %d\n", download_status);
        t2CountNotify("RDM_INFO_AppDownloadSuccess");
	if(pApp_det->is_versioned_app) {
            RDMInfo("Post Installation Successful for %s\n", pApp_det->app_name);
	    return download_status;
	} else {
            rdmUnInstallApps(is_broadband);
            ret = rdmIARMEvntSendStatus(RDM_PKG_UNINSTALL);
            if(ret) {
               RDMError("Failed to send the IARM event\n");
            }
	}
    }
    else {
        RDMInfo("App download failed, sending status as %d\n", download_status);
	rdmUnInstallApps(is_broadband);
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
