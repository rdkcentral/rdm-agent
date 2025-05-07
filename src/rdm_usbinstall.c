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
#include <stdbool.h>
#ifndef GTEST_ENABLE
#include <json_parse.h>
#endif

#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_usbinstall.h"
#include "rdm_download.h"
#include "rdm_downloadutils.h"
#ifndef GTEST_ENABLE
#include <system_utils.h>
#endif

INT32 rdmUSBInstall(RDMHandle *prdmHandle, RDMAPPDetails *pRdmAppDet, CHAR *pUsbPath)
{
    CHAR *usb_apps[RDM_MAX_USB_APP];
    INT32 num_app = 0;
    INT32 i = 0;
    INT32 status = RDM_SUCCESS;
    INT32  ret = RDM_SUCCESS;
    INT32 download_status     = 0;

    for(i = 0; i < RDM_MAX_USB_APP; i++) {
        usb_apps[i] = calloc(RDM_APP_PATH_LEN, 1);
    }

    findPFileAll(pUsbPath, RDM_USB_TAR_SEARCH, usb_apps, &num_app, RDM_MAX_USB_APP);

    for(i = 0; i < num_app; i++) {
        CHAR *pkg_name = NULL;
        CHAR *pkg = NULL;
        CHAR *app_name;

        pkg_name = getPartChar(usb_apps[i], '/');
        pkg      = getPartChar(pkg_name, '_');
        app_name = getPartStr(pkg_name, "signed");
        memset(pRdmAppDet, 0 ,sizeof(RDMAPPDetails));
        memcpy(pRdmAppDet->app_name, pkg, (app_name - pkg - 2));

        /* Parse the JSON file and get the app details */
        ret = rdmJSONGetAppDetName(pRdmAppDet->app_name, pRdmAppDet);
        if(ret) {
            RDMError("Failed to get App details from Manifest: %s\n", pRdmAppDet->app_name);
            continue;
        }

        if(strcmp(pRdmAppDet->pkg_name,pkg_name) != 0){
            RDMError("Package %s does not belong to the currently running firmware. Hence skipping it\n",pkg_name);
            continue;
        }

        /* Update the app paths */
        rdmUpdateAppDetails(prdmHandle, pRdmAppDet, 0);

        rdmPrintAppDetails(pRdmAppDet);

        strncpy(pRdmAppDet->app_usb, usb_apps[i], MAX_BUFF_SIZE);

        /* download is not required, package is already available in USB */
        pRdmAppDet->dwld_status = 1;
        pRdmAppDet->is_usb = 1;

        ret = rdmDownloadApp(pRdmAppDet, &download_status);
        if(ret) {
            RDMError("Failed to download the App: %s, status: %d\n", pRdmAppDet->app_name, download_status);
            continue;
        }
    }

    if(i == num_app) {
        RDMWarn("Reached end of manifest file\n");
    }

    return status;
}
