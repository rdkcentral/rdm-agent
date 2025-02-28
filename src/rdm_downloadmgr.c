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

#include "rdmMgr.h"
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_download.h"
#include "rdm_openssl.h"
#include "rdm_downloadutils.h"
#include <system_utils.h>

static INT32 rdmDownlLXCCheck(CHAR *package, CHAR *appname)
{
    CHAR tmp[RDM_TMP_LEN_64] = {0};

    strncpy(tmp, appname, RDM_TMP_LEN_64);
    strcat(tmp, "_container.ipk");

    if(!strcmp(package, tmp)) {
        return 1;
    }
    return 0;
}

static INT32 rdmDwnlLXCIPKExctact()
{
    return RDM_SUCCESS;
}

INT32 rdmDwnlExtract(RDMAPPDetails *pRdmAppDet)
{
    CHAR tmp_file[RDM_APP_PATH_LEN];
    INT32 status = RDM_SUCCESS;

    /* Extract the package */
    status = tarExtract(pRdmAppDet->app_dwnl_filepath, pRdmAppDet->app_dwnl_path);
    if(status) {
        RDMError("Failed to extract the package\n");
        return status;
    }

    if(findPFile(pRdmAppDet->app_dwnl_path, "*-pkg.tar", tmp_file)) {
        /* Extract the package */
        RDMInfo("Intermediate PKG File : %s dwnd path %s\n", tmp_file,pRdmAppDet->app_dwnl_path);
        status = tarExtract(tmp_file, pRdmAppDet->app_dwnl_path);
        if(status) {
            RDMError("Failed to extract the package\n");
            return status;
        }
    }
    else if(findPFile(pRdmAppDet->app_dwnl_path, "*-signed.tar", tmp_file)) {
	    /* Extract the package */
	    RDMInfo("PKG File : %s dwnd path %s\n", tmp_file,pRdmAppDet->app_dwnl_path);
	    status = tarExtract(tmp_file, pRdmAppDet->app_dwnl_path);
	    if(status) {
		    RDMError("Failed to extract the package\n");
		    return status;
	    }
    }
    else {
        status = RDM_FAILURE;
    }
    strncpy(tmp_file, pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN);
    strcat(tmp_file, "/");
    strcat(tmp_file, pRdmAppDet->app_name);
    strcat(tmp_file, "_");
    strcat(tmp_file, pRdmAppDet->pkg_ver);
    strcat(tmp_file, ".tar");

    if(fileCheck(tmp_file)) {
	    RDMInfo("tmp_file = %s\n", tmp_file)
    }

    status = tarExtract(tmp_file, pRdmAppDet->app_dwnl_path);
    if(status) {
	    rdmIARMEvntSendPayload(pRdmAppDet->app_name,
			    pRdmAppDet->pkg_ver,
			    pRdmAppDet->app_home,
			    RDM_PKG_EXTRACT_ERROR);
	    RDMError("Failed to extract the package: %s\n", tmp_file);
    }

    CHAR app_file[RDM_APP_PATH_LEN];
    strncpy(app_file, pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN);
    strcat(app_file, "/");
    strcat(app_file, pRdmAppDet->app_name);
    strcat(app_file, ".tar");

    status = tarExtract(app_file, pRdmAppDet->app_home);
    if(status) {
	    rdmIARMEvntSendPayload(pRdmAppDet->app_name,
			    pRdmAppDet->pkg_ver,
			    pRdmAppDet->app_home,
			    RDM_PKG_EXTRACT_ERROR);
	    RDMError("Failed to extract the package: %s\n", tmp_file);
    }

    return status;
}

INT32 rdmDownloadMgr(RDMAPPDetails *pRdmAppDet)
{
    INT32 status = RDM_SUCCESS;
    INT32 rdm_status = RDM_DL_NOERROR;
    CHAR ipk_file[RDM_APP_PATH_LEN];
    /* Download the File Package  if not already downloaded */
    if(pRdmAppDet->dwld_status) {
        RDMInfo("File package %s/%s already available\n", pRdmAppDet->app_mnt, pRdmAppDet->pkg_name);

        status = rdmDwnlCreateFolder(pRdmAppDet->app_mnt, pRdmAppDet->app_name);
        if(status) {
            RDMError("Failed to create download paths\n");
            return status;
        }

        if(pRdmAppDet->is_usb) {

            snprintf(pRdmAppDet->app_dwnl_filepath, sizeof(pRdmAppDet->app_dwnl_filepath), "%s/%s", pRdmAppDet->app_dwnl_path, pRdmAppDet->pkg_name);

            status = copyFiles(pRdmAppDet->app_usb, pRdmAppDet->app_dwnl_filepath);
            if(status) {
                RDMError("Failed to copy from usb\n");
                return status;
            }
        }
    }
    else {
        status = rdmDwnlCreateFolder(pRdmAppDet->app_mnt, pRdmAppDet->app_name);
        if(status) {
            RDMError("Failed to create download paths\n");
            return status;
        }

        status = rdmDwnlApplication(pRdmAppDet->app_dwnl_url,
                                    pRdmAppDet->app_dwnl_path,
                                    pRdmAppDet->pkg_name,
                                    pRdmAppDet->app_dwnl_filepath,
                                    pRdmAppDet->is_mtls);
        if(status) {
            RDMError("Failed to download the package\n");
            rdm_status = RDM_DL_DWNLERR;
            return status;
        }
    }

    status = rdmDwnlExtract(pRdmAppDet);
    if(status) {
        RDMError("Failed to extract the package\n");
        rdm_status = RDM_DL_DWNLERR;
        goto error;
    }

    if(findPFile(pRdmAppDet->app_dwnl_path, "*.ipk", ipk_file)) {
        /* Remove the ipk file */
        RDMInfo("Removing %s after Extraction\n", ipk_file);
        removeFile(ipk_file);
    }

    RDMInfo("Download and Extraction Completed\n");

    if(pRdmAppDet->is_oss) {
        RDMInfo("IMAGE_TYPE IS OSS. Signature validation not required\n");
    }

    if(pRdmAppDet->is_usb) {
        if(!strcmp(pRdmAppDet->pkg_type, "plugin")){
            status = rdmPackageMgrStateChange(pRdmAppDet);
            if ( status == RDM_FAILURE){
                RDMError("State Transistion Failed for Plugin USB Download\n");
            }
        }
    }

    rdmIARMEvntSendPayload(pRdmAppDet->app_name,
                           pRdmAppDet->pkg_ver,
                           pRdmAppDet->app_home,
                           RDM_PKG_INSTALL_COMPLETE);

    rdmDwnlRunPostScripts(pRdmAppDet->app_home);

error:
    return rdm_status;
}
