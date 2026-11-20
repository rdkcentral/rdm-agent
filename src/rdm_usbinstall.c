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

#ifdef L2_TEST_ENABLED
static INT32 findPFileAllmock(const CHAR *pUsbPath, const CHAR *pattern,
                              CHAR *usb_apps[], INT32 *num_app, INT32 max_usb_app);
#endif

INT32 rdmUSBInstall(RDMHandle *prdmHandle, RDMAPPDetails *pRdmAppDet, CHAR *pUsbPath)
{
    CHAR *usb_apps[RDM_MAX_USB_APP] = {0};
    INT32 num_app = 0;
    INT32 i = 0;
    INT32 status = RDM_SUCCESS;
    INT32  ret = RDM_SUCCESS;
    INT32 download_status     = 0;

    if (prdmHandle == NULL || pRdmAppDet == NULL || pUsbPath == NULL) {
        RDMError("Invalid USB installation argument\n");
        return RDM_FAILURE;
    }

    for(i = 0; i < RDM_MAX_USB_APP; i++) {
        usb_apps[i] = calloc(RDM_APP_PATH_LEN, 1);
        if (usb_apps[i] == NULL) {
            RDMError("Unable to allocate USB package path\n");
            status = RDM_FAILURE;
            goto cleanup;
        }
    }

  #ifdef L2_TEST_ENABLED
    if (findPFileAllmock(pUsbPath, RDM_USB_TAR_SEARCH, usb_apps,
                         &num_app, RDM_MAX_USB_APP) != RDM_SUCCESS) {
        status = RDM_FAILURE;
        goto cleanup;
    }
  #else
    if (rdmFindPackageFiles(pUsbPath, RDM_USB_TAR_SEARCH, usb_apps,
                            RDM_APP_PATH_LEN, &num_app, RDM_MAX_USB_APP) != RDM_SUCCESS) {
        status = RDM_FAILURE;
        goto cleanup;
    }
  #endif
    if (num_app < 0 || num_app > RDM_MAX_USB_APP) {
        RDMError("Invalid USB package count\n");
        status = RDM_FAILURE;
        goto cleanup;
    }

    for(i = 0; i < num_app; i++) {
        CHAR *pkg_name = strrchr(usb_apps[i], '/');
        CHAR *app_start;
        CHAR *app_end;
        CHAR *separator;
        size_t app_len;

        pkg_name = pkg_name != NULL ? pkg_name + 1 : usb_apps[i];
        separator = strrchr(pkg_name, '_');
        if (separator == NULL || separator == pkg_name || separator[1] == '\0') {
            RDMError("Invalid USB package filename: %s\n", pkg_name);
            continue;
        }
     #ifdef L2_TEST_ENABLED
        app_start = pkg_name;
        app_end = separator;
     #else
        app_start = separator + 1;
        size_t pkg_len = strlen(pkg_name);
        const CHAR suffix[] = "-signed.tar";
        if (pkg_len < sizeof(suffix) - 1 ||
            strcmp(pkg_name + pkg_len - (sizeof(suffix) - 1), suffix) != 0) {
            RDMError("Invalid USB package suffix: %s\n", pkg_name);
            continue;
        }
        app_end = pkg_name + pkg_len - (sizeof(suffix) - 1);
     #endif
        if (app_end <= app_start) {
            RDMError("Empty or malformed USB application name: %s\n", pkg_name);
            continue;
        }
        app_len = (size_t)(app_end - app_start);
        if (app_len >= sizeof(pRdmAppDet->app_name)) {
            RDMError("USB application name exceeds destination size: %s\n", pkg_name);
            continue;
        }
        memset(pRdmAppDet, 0, sizeof(RDMAPPDetails));
        memcpy(pRdmAppDet->app_name, app_start, app_len);
        pRdmAppDet->app_name[app_len] = '\0';

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

        ret = snprintf(pRdmAppDet->app_usb, sizeof(pRdmAppDet->app_usb), "%s", usb_apps[i]);
        if (ret < 0 || (size_t)ret >= sizeof(pRdmAppDet->app_usb)) {
            RDMError("USB package path exceeds destination size\n");
            continue;
        }

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

cleanup:
    for (i = 0; i < RDM_MAX_USB_APP; i++) {
        free(usb_apps[i]);
    }
    return status;
}


#ifdef L2_TEST_ENABLED
static INT32 findPFileAllmock(const CHAR *pUsbPath, const CHAR *pattern,
                              CHAR *usb_apps[], INT32 *num_app, INT32 max_usb_app) {
    int mock_num_apps = 1;
    *num_app = mock_num_apps;
    if (mock_num_apps > 0) {
        int length = snprintf(usb_apps[0], RDM_APP_PATH_LEN, "%s",
                              "/mnt/usb/RDK-RRD-Test_1.0-pkg.tar");
        if (length < 0 || length >= RDM_APP_PATH_LEN) {
            RDMError("Mock USB package path exceeds destination size\n");
            *num_app = 0;
            return RDM_FAILURE;
        }
    }
    return 0;
}
#endif
