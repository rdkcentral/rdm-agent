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
#include <string.h>
#ifndef GTEST_ENABLE
#include <system_utils.h>
#ifdef IARMBUS_SUPPORT
#include "rdmMgr.h"
#else
#define RDM_PKG_EXTRACT_ERROR 5
#define RDM_PKG_VALIDATE_ERROR 7
#define RDM_PKG_INSTALL_COMPLETE 0
#endif
#else
#include "unittest/mocks/system_utils.h"
#include "unittest/mocks/rdmMgr.h"
#endif

static INT32 rdmDownlLXCCheck(CHAR *package, CHAR *appname)
{
    CHAR tmp[RDM_TMP_LEN_64] = {0};
    const CHAR suffix[] = "_container.ipk";
    size_t appname_len;

    if (package == NULL || appname == NULL) {
        return 0;
    }

    appname_len = strlen(appname);
    if (appname_len > sizeof(tmp) - sizeof(suffix)) {
        return 0;
    }

    memcpy(tmp, appname, appname_len);
    memcpy(tmp + appname_len, suffix, sizeof(suffix));

    if(!strcmp(package, tmp)) {
        return 1;
    }
    return 0;
}

static INT32 rdmDwnlLXCIPKExctact()
{
    return RDM_SUCCESS;
}

#ifdef L2_TEST_ENABLED
static INT32 rdmDwnlRetryIfRequiredFileMissing(RDMAPPDetails *pRdmAppDet, CHAR *signed_file, const CHAR *required_file)
{
    CHAR pkg_file[RDM_APP_PATH_LEN];
    INT32 status = RDM_SUCCESS;

    int ret = snprintf(pkg_file, sizeof(pkg_file), "%s/%s", pRdmAppDet->app_dwnl_path, required_file);
    if(ret < 0 || (size_t)ret >= sizeof(pkg_file)) {
        RDMWarn("Package file path truncated for %s\n", required_file);
        return RDM_FAILURE;
    }

    if(fileCheck(pkg_file)) {
        return RDM_SUCCESS;
    }

    CHAR pkg_cmd[RDM_APP_PATH_LEN * 2];
    snprintf(pkg_cmd, sizeof(pkg_cmd), "tar -xf %s -C %s",
            signed_file, pRdmAppDet->app_dwnl_path);
    RDMInfo("Trying system command extraction: %s\n", pkg_cmd);
    copyCommandOutput(pkg_cmd, NULL, 0);

    if(!fileCheck(pkg_file)) {
        RDMWarn("%s not found after retries for %s. Continuing extraction flow\n", required_file, signed_file);
        status = RDM_FAILURE;
    }
    return status;
}
#endif

INT32 rdmDwnlExtract(RDMAPPDetails *pRdmAppDet)
{
    CHAR tmp_file[RDM_APP_PATH_LEN];
    CHAR ip_file[RDM_APP_PATH_LEN];
    INT32 status = RDM_SUCCESS;
    INT32 path_len = 0;

    /* Extract the package */
    status = tarExtract(pRdmAppDet->app_dwnl_filepath, pRdmAppDet->app_dwnl_path);
    if(status) {
        RDMError("Failed to extract the package %s\n", pRdmAppDet->app_dwnl_filepath);
        return status;
    }

    if(pRdmAppDet->is_versioned_app) {
	    if(findPFile(pRdmAppDet->app_dwnl_path, "*-signed.tar", tmp_file)) {
		    /* Extract the package */
		    RDMInfo("Extracting PKG File : %s to dwnd path %s\n", tmp_file,pRdmAppDet->app_dwnl_path);
		    status = tarExtract(tmp_file, pRdmAppDet->app_dwnl_path);
		    if(status) {
			    RDMError("Failed to extract the package %s\n", tmp_file);
			    return status;
                    }
		    RDMInfo("Extraction of %s is Successful\n", tmp_file);
            }

        #ifdef L2_TEST_ENABLED
            if(rdmDwnlRetryIfRequiredFileMissing(pRdmAppDet, tmp_file, "pkg_cpemanifest") == RDM_SUCCESS)
            {
                RDMInfo("Extraction of pkg_cpemanifest is successful\n");
            }
        #endif

        path_len = snprintf(tmp_file, sizeof(tmp_file), "%s/%s_%s.tar",
                            pRdmAppDet->app_dwnl_path, pRdmAppDet->app_name, pRdmAppDet->pkg_ver);
        if(path_len < 0 || (size_t)path_len >= sizeof(tmp_file)) {
            RDMError("Package path too long\n");
            return RDM_FAILURE;
        }

	    if(fileCheck(tmp_file)) {
		    RDMInfo("FileCheck = %s\n", tmp_file);
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

        #ifdef L2_TEST_ENABLED
            if(rdmDwnlRetryIfRequiredFileMissing(pRdmAppDet, tmp_file, "package.json") == RDM_SUCCESS)
            {
                RDMInfo("Extraction of package.json is successful\n");
            }
        #endif

	    CHAR app_file[RDM_APP_PATH_LEN];
        path_len = snprintf(app_file, sizeof(app_file), "%s/%s.tar",
                            pRdmAppDet->app_dwnl_path, pRdmAppDet->app_name);
        if(path_len < 0 || (size_t)path_len >= sizeof(app_file)) {
            RDMError("Package path too long\n");
            return RDM_FAILURE;
        }

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

#ifdef L2_TEST_ENABLED
    if(rdmDwnlRetryIfRequiredFileMissing(pRdmAppDet, tmp_file, "pkg_cpemanifest") == RDM_SUCCESS)
    {
       RDMInfo("Extraction of pkg_cpemanifest is successful\n");
    }
#endif

#ifdef L2_TEST_ENABLED
    path_len = snprintf(tmp_file, sizeof(tmp_file), "%s/pkg_cpemanifest", pRdmAppDet->app_dwnl_path);
#else
    path_len = snprintf(tmp_file, sizeof(tmp_file), "%s/%s_cpemanifest",
                        pRdmAppDet->app_home, pRdmAppDet->app_name);
#endif
    if(path_len < 0 || (size_t)path_len >= sizeof(tmp_file)) {
        RDMError("Package path too long\n");
        return RDM_FAILURE;
    }

    if(fileCheck(tmp_file)) {
        RDMInfo("%s Package already extracted\n", tmp_file);
    }
    else {
        FILE *fp;
        CHAR *extn = NULL;
        INT32 is_lxc = 0;

        path_len = snprintf(tmp_file, sizeof(tmp_file), "%s/packages.list", pRdmAppDet->app_dwnl_path);
        if(path_len < 0 || (size_t)path_len >= sizeof(tmp_file)) {
            RDMError("Package path too long\n");
            return RDM_FAILURE;
        }

        fp = fopen(tmp_file, "r");
        if(fp == NULL) {
            RDMError("Not Found the Packages List file\n");
	    t2CountNotify("RDM_ERR_package_notfound", 1);
            return RDM_FAILURE;
        }

        while (fgets(tmp_file, RDM_APP_PATH_LEN, fp)) {
            size_t len = strlen (tmp_file);
            if ((len > 0) && (tmp_file[len - 1] == '\n'))
                tmp_file[len - 1] = 0;

            extn = getExtension(tmp_file);

            if(!strcmp(extn, "tar")) {
                status = tarExtract(tmp_file, pRdmAppDet->app_home);
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
                path_len = snprintf(ip_file, sizeof(ip_file), "%s/%s", pRdmAppDet->app_dwnl_path, tmp_file);
                if(path_len < 0 || (size_t)path_len >= sizeof(ip_file)) {
                    RDMError("Package path too long\n");
                    status = RDM_FAILURE;
                    continue;
                }
                status = arExtract(ip_file, pRdmAppDet->app_dwnl_path);
                if(status) {
                    rdmIARMEvntSendPayload(pRdmAppDet->pkg_name,
                                           pRdmAppDet->pkg_ver,
                                           pRdmAppDet->app_home,
                                           RDM_PKG_EXTRACT_ERROR);
                    RDMError("Failed to extract the package: %s\n", tmp_file);
                    continue;
                }
		        if(findPFile(pRdmAppDet->app_dwnl_path, "data.tar.*", tmp_file)) {
                /* Extract the package */
                    RDMInfo("Data file: %s dwnd path %s\n", tmp_file,pRdmAppDet->app_dwnl_path);
                    status = tarExtract(tmp_file, pRdmAppDet->app_home);
                    if(status) {
                        RDMError("Failed to extract the package\n");
			            rdmIARMEvntSendPayload(pRdmAppDet->pkg_name,
                                           pRdmAppDet->pkg_ver,
                                           pRdmAppDet->app_home,
                                           RDM_PKG_EXTRACT_ERROR);
                        RDMError("Failed to extract the package: %s\n", tmp_file);
                        continue;
                    }
                } else {
                    RDMError("Failed to find data.tar.* in extracted IPK at path: %s\n", pRdmAppDet->app_dwnl_path);
                    rdmIARMEvntSendPayload(pRdmAppDet->pkg_name,
                                           pRdmAppDet->pkg_ver,
                                           pRdmAppDet->app_home,
                                           RDM_PKG_EXTRACT_ERROR);
                    status = RDM_FAILURE;
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
                                    pRdmAppDet->app_dwnl_filepath);
        if(status) {
            RDMError("Failed to download the package %s\n", pRdmAppDet->pkg_name);
            t2CountNotify("NF_INFO_rdm_package_failure", 1);
            rdm_status = RDM_DL_DWNLERR;
            return status;
        }
    }

    status = rdmDwnlExtract(pRdmAppDet);
    if(status) {
        RDMError("Failed to extract the package %s\n", pRdmAppDet->pkg_name);
        rdm_status = RDM_DL_DWNLERR;
        goto error;
    }

    if(findPFile(pRdmAppDet->app_dwnl_path, "*.ipk", ipk_file)) {
        /* Remove the ipk file */
        RDMInfo("Removing %s after Extraction\n", ipk_file);
        removeFile(ipk_file);
    }

    RDMInfo("Download and Extraction Completed\n");
    t2CountNotify("RDM_INFO_extraction_complete", 1);

    if(pRdmAppDet->is_oss) {
        RDMInfo("IMAGE_TYPE IS OSS. Signature validation not required\n");
    } else if (pRdmAppDet->is_versioned_app) {
	 RDMInfo("Signature validation for Versioned APP\n");
	 status = rdmDwnlValidation(pRdmAppDet, NULL);
        if(status) {
            RDMError("signature validation failed\n");
	    t2CountNotify("RDM_ERR_rsa_signature_failed", 1);
            rdmIARMEvntSendPayload(pRdmAppDet->app_name,
                                   pRdmAppDet->pkg_ver,
                                   pRdmAppDet->app_home,
                                   RDM_PKG_VALIDATE_ERROR);

            pRdmAppDet->dwld_status = 0;
            rdm_status = RDM_DL_DWNLERR;
            goto error;
        }
        RDMInfo("RDM package download success: %s \n", pRdmAppDet->pkg_name);
	t2ValNotify("NF_INFO_rdm_success", pRdmAppDet->pkg_name);
    } else {
        status = rdmDwnlValidation(pRdmAppDet, NULL);
        if(status) {
            RDMError("signature validation failed\n");
	    t2CountNotify("RDM_ERR_rsa_signature_failed", 1);
            rdmIARMEvntSendPayload(pRdmAppDet->pkg_name,
                                   pRdmAppDet->pkg_ver,
                                   pRdmAppDet->app_home,
                                   RDM_PKG_VALIDATE_ERROR);

            pRdmAppDet->dwld_status = 0;
            rdm_status = RDM_DL_DWNLERR;
            goto error;
        }
        RDMInfo("RDM package download success: %s \n", pRdmAppDet->pkg_name);
	t2ValNotify("NF_INFO_rdm_success", pRdmAppDet->pkg_name);
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
    rdmDwnlRunPostScripts(pRdmAppDet, pRdmAppDet->is_versioned_app);

error:
    return rdm_status;
}
