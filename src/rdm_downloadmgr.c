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

#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_download.h"
#include "rdm_openssl.h"
#include "rdm_downloadutils.h"
#ifndef GTEST_ENABLE
#include <system_utils.h>
#include "rdmMgr.h"
#else
#include "unittest/mocks/system_utils.h"
#include "unittest/mocks/rdmMgr.h"
#endif

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
    CHAR ip_file[RDM_APP_PATH_LEN];
    INT32 status = RDM_SUCCESS;

    /* Extract the package */
    status = tarExtract(pRdmAppDet->app_dwnl_filepath, pRdmAppDet->app_dwnl_path);
    if(status) {
        RDMError("Failed to extract the package\n");
        return status;
    }

    if(pRdmAppDet->is_versioned_app) {
	    if(findPFile(pRdmAppDet->app_dwnl_path, "*-signed.tar", tmp_file)) {
		    /* Extract the package */
		    RDMInfo("Extracting PKG File : %s to dwnd path %s\n", tmp_file,pRdmAppDet->app_dwnl_path);
		    status = tarExtract(tmp_file, pRdmAppDet->app_dwnl_path);
		    if(status) {
			    RDMError("Failed to extract the package\n");
			    return status;
                    }
		    RDMInfo("Extraction of %s is Successful\n", tmp_file);
            }
	    strncpy(tmp_file, pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN - 1);
	    tmp_file[sizeof(tmp_file) - 1] = '\0';
	    strcat(tmp_file, "/");
	    strcat(tmp_file, pRdmAppDet->app_name);
	    strcat(tmp_file, "_");
	    strcat(tmp_file, pRdmAppDet->pkg_ver);
	    strcat(tmp_file, ".tar");
	    tmp_file[sizeof(tmp_file) - 1] = '\0';  // Ensure null termination

	    if(fileCheck(tmp_file)) {
		    RDMInfo("tmp_file = %s\n", tmp_file);
            }

	    RDMInfo("Extracting %s to %s\n", tmp_file, pRdmAppDet->app_dwnl_path);
	    status = tarExtract(tmp_file, pRdmAppDet->app_dwnl_path);
	    if(status) {
		    rdmIARMEvntSendPayload(pRdmAppDet->app_name,
				    pRdmAppDet->pkg_ver,
				    pRdmAppDet->app_home,
				    RDM_PKG_EXTRACT_ERROR);
		    RDMError("Failed to extract the package: %s\n", tmp_file);
            }
	    RDMInfo("Extraction of %s is Successful \n", tmp_file);

	    CHAR app_file[RDM_APP_PATH_LEN];
	    strncpy(app_file, pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN -1);
	    app_file[sizeof(app_file) - 1] = '\0';
	    strcat(app_file, "/");
	    strcat(app_file, pRdmAppDet->app_name);
	    strcat(app_file, ".tar");
	    app_file[sizeof(app_file) - 1] = '\0';

	    status = tarExtract(app_file, pRdmAppDet->app_home);
	    if(status) {
		    rdmIARMEvntSendPayload(pRdmAppDet->app_name,
				    pRdmAppDet->pkg_ver,
				    pRdmAppDet->app_home,
				    RDM_PKG_EXTRACT_ERROR);
		    RDMError("Failed to extract the package: %s\n", tmp_file);
            }
    } else {
    if(findPFile(pRdmAppDet->app_dwnl_path, "*-pkg.tar", tmp_file)) {
        /* Extract the package */
        RDMInfo("Intermediate PKG File : %s dwnd path %s\n", tmp_file,pRdmAppDet->app_dwnl_path);
        status = tarExtract(tmp_file, pRdmAppDet->app_dwnl_path);
        if(status) {
            RDMError("Failed to extract the package\n");
            return status;
        }
    }
    else {
        status = RDM_FAILURE;
    }

    strncpy(tmp_file, pRdmAppDet->app_home, RDM_APP_PATH_LEN - 1);
    tmp_file[sizeof(tmp_file) - 1] = '\0';
    strcat(tmp_file, "/");
    strcat(tmp_file, pRdmAppDet->app_name);
    strcat(tmp_file, "_cpemanifest");
    tmp_file[sizeof(tmp_file) - 1] = '\0';

    if(fileCheck(tmp_file)) {
        RDMInfo("Package already extracted\n");
    }
    else {
        FILE *fp;
        CHAR *extn = NULL;
        INT32 is_lxc = 0;

        strncpy(tmp_file, pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN - 1);
	tmp_file[sizeof(tmp_file) - 1] = '\0';
        strcat(tmp_file, "/packages.list");
	tmp_file[sizeof(tmp_file) - 1] = '\0';
	RDMInfo("tmp_file = %s\n", tmp_file);
	
        fp = fopen(tmp_file, "r");
        if(fp == NULL) {
            RDMError("Not Found the Packages List file\n");
            return RDM_FAILURE;
        }

        while (fgets(tmp_file, RDM_APP_PATH_LEN, fp)) {
            size_t len = strlen (tmp_file);
            if ((len > 0) && (tmp_file[len - 1] == '\n'))
                tmp_file[len - 1] = 0;

            extn = getExtension(tmp_file);
	    RDMInfo("extn = %s\n", extn);

            if(!strcmp(extn, "tar")) {
                status = tarExtract(tmp_file, pRdmAppDet->app_home);
		RDMInfo("tmp_file = %s\nprdmAppDet->app_home = %s", tmp_file, pRdmAppDet->app_home);
                if(status) {
                    rdmIARMEvntSendPayload(pRdmAppDet->pkg_name,
                                           pRdmAppDet->pkg_ver,
                                           pRdmAppDet->app_home,
                                           RDM_PKG_EXTRACT_ERROR);
                    RDMError("Failed to extract the package: %s\n", tmp_file);
                    continue;
                }
            }//tar
            else if(!strcmp(extn, "ipk")) {
                if(rdmDownlLXCCheck(tmp_file, pRdmAppDet->app_name)) {
                    is_lxc = 1;
                }
                RDMInfo("tmp_file = %s\nprdmAppDet->app_home = %s", tmp_file, pRdmAppDet->app_home);
		strncpy(ip_file, pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN - 1);
                ip_file[sizeof(ip_file) - 1] = '\0';
		strcat(ip_file, "/");
		ip_file[sizeof(ip_file) - 1] = '\0';
        	strcat(ip_file, tmp_file);
		ip_file[sizeof(ip_file) - 1] = '\0';
                status = arExtract(ip_file, pRdmAppDet->app_dwnl_path);
                if(status) {
                    rdmIARMEvntSendPayload(pRdmAppDet->pkg_name,
                                           pRdmAppDet->pkg_ver,
                                           pRdmAppDet->app_home,
                                           RDM_PKG_EXTRACT_ERROR);
                    RDMError("Failed to extract the package: %s\n", tmp_file);
                    continue;
                }
                strncpy(tmp_file, pRdmAppDet->app_dwnl_path, RDM_APP_PATH_LEN - 1);
		tmp_file[sizeof(tmp_file) - 1] = '\0';
                strcat(tmp_file, "/data.tar.xz");
		tmp_file[sizeof(tmp_file) - 1] = '\0';

                status = tarExtract(tmp_file, pRdmAppDet->app_home);
                if(status) {
                    rdmIARMEvntSendPayload(pRdmAppDet->pkg_name,
                                           pRdmAppDet->pkg_ver,
                                           pRdmAppDet->app_home,
                                           RDM_PKG_EXTRACT_ERROR);
                    RDMError("Failed to extract the package: %s\n", tmp_file);
                    continue;
                }
                if(is_lxc) {
                    rdmDwnlLXCIPKExctact();
                }
            } //ipk
            else {
                RDMWarn("Unknown Package Extension\n");
            }
        } //while ()
        fclose(fp);
    }
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

	    size_t path_len = strlen(pRdmAppDet->app_dwnl_path);
            size_t name_len = strlen(pRdmAppDet->pkg_name);
            size_t total_len = path_len + name_len + 2; // +2 for '/' and null terminator

            if (total_len < sizeof(pRdmAppDet->app_dwnl_filepath)) {
                memmove(pRdmAppDet->app_dwnl_filepath, pRdmAppDet->app_dwnl_path, path_len);
                pRdmAppDet->app_dwnl_filepath[path_len] = '/';
                memmove(pRdmAppDet->app_dwnl_filepath + path_len + 1, pRdmAppDet->pkg_name, name_len + 1); // +1 to include null terminator

            } else {
                RDMError("Failed to copy download paths\n");
		return RDM_FAILURE;
            }

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
    } else if (pRdmAppDet->is_versioned_app) {
	 RDMInfo("Signature validation for Versioned APP\n");
	 status = rdmDwnlValidation(pRdmAppDet, NULL);
        if(status) {
            RDMError("signature validation failed\n");
            rdmIARMEvntSendPayload(pRdmAppDet->app_name,
                                   pRdmAppDet->pkg_ver,
                                   pRdmAppDet->app_home,
                                   RDM_PKG_VALIDATE_ERROR);

            pRdmAppDet->dwld_status = 0;
            rdm_status = RDM_DL_DWNLERR;
            goto error;
        }
        RDMInfo("RDM package download success: %s \n", pRdmAppDet->pkg_name);
    } else {
        status = rdmDwnlValidation(pRdmAppDet, NULL);
        if(status) {
            RDMError("signature validation failed\n");
            rdmIARMEvntSendPayload(pRdmAppDet->pkg_name,
                                   pRdmAppDet->pkg_ver,
                                   pRdmAppDet->app_home,
                                   RDM_PKG_VALIDATE_ERROR);

            pRdmAppDet->dwld_status = 0;
            rdm_status = RDM_DL_DWNLERR;
            goto error;
        }
        RDMInfo("RDM package download success: %s \n", pRdmAppDet->pkg_name);
    }

    if(pRdmAppDet->is_usb) {
        if(!strcmp(pRdmAppDet->pkg_type, "plugin")){
            status = rdmPackageMgrStateChange(pRdmAppDet);
            if ( status == RDM_FAILURE){
                RDMError("State Transistion Failed for Plugin USB Download\n");
            }
        }
    }

    if(pRdmAppDet->is_versioned_app) {
	    rdmIARMEvntSendPayload(pRdmAppDet->app_name,
			    pRdmAppDet->pkg_ver,
			    pRdmAppDet->app_home,
			    RDM_PKG_INSTALL_COMPLETE);
    } else {
           rdmIARMEvntSendPayload(pRdmAppDet->pkg_name,
                           pRdmAppDet->pkg_ver,
                           pRdmAppDet->app_home,
                           RDM_PKG_INSTALL_COMPLETE);
    }
    rdmDwnlRunPostScripts(pRdmAppDet->app_home, pRdmAppDet->is_versioned_app);

error:
    return rdm_status;
}
