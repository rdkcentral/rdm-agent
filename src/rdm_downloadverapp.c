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
#include "rdm_downloadutils.h"
#include "rdm_downloadverapp.h"
#ifndef GTEST_ENABLE
#include <system_utils.h>
#else
#include "unittest/mocks/system_utils.h"
#endif

static INT32 rdmDwnlVAVerifyApp(RDMAPPDetails *pRdmAppDet, CHAR *pVer)
{
    if(rdmDwnlValidation(pRdmAppDet, pVer)) {
        RDMError("Failed to validate the app\n");
        return RDM_FAILURE;
    }

    return RDM_SUCCESS;
}

static VOID  rdmDwnlVAGetInstallVer(RDMAPPDetails *pRdmAppDet,
                                    CHAR **ppVer,  INT32 *pInum,
                                    CHAR **ppUver, INT32 *pUnum)
{
    INT32 num_ver = 0, f_ver = 0;
    INT32 status = RDM_SUCCESS;
    INT32 i = 0;
    CHAR *pkg_json[RDM_MAX_VER_LIST];

    for(i = 0; i < RDM_MAX_VER_LIST; i++) {
        pkg_json[i] = calloc(RDM_APP_PATH_LEN, 1);
    }

    findPFileAll(pRdmAppDet->app_home, RDM_JSON_VER, pkg_json, &num_ver, RDM_MAX_VER_LIST);

    for(i = 0; i < num_ver; i++) {
        rdmJSONQuery(pkg_json[i], "version", ppVer[i]);
    }

    if(num_ver > 1) {
        qsString(ppVer, num_ver);
        num_ver = strRmDuplicate(ppVer, num_ver);
    }

    //Un-install the older versions
    if(num_ver > MAX_APP_VERSIONS_ALLOWED) {
        for(i = MAX_APP_VERSIONS_ALLOWED; i < num_ver; i++) {
            ppUver[*pUnum] = ppVer[i];
            (*pUnum)++;
            ppVer[i] = NULL;
        }
        num_ver = MAX_APP_VERSIONS_ALLOWED;
    }

    //Verify the installed apps
    f_ver = num_ver;
    for(i = 0; i < num_ver; i++) {
        status = rdmDwnlVAVerifyApp(pRdmAppDet, ppVer[i]);
        if(status) {
            ppUver[*pUnum] = ppVer[i];
            (*pUnum)++;
            ppVer[i] = "";
            f_ver--;
        }
    }
    if(num_ver > 1) {
        qsString(ppVer, num_ver);
    }

    *pInum = f_ver;

    for(i = 0; i < RDM_MAX_VER_LIST; i++) {
        free(pkg_json[i]);
    }
}

static INT32 rdmDwnlVAGetManifetVer(CHAR *pkgVersion, CHAR **ppVer)
{
    INT32 num_ver = 0;
    INT32 i =0;

    num_ver = strSplit(pkgVersion, " ", ppVer, RDM_MAX_VER_LIST);

    if(num_ver > 1) {
        qsString(ppVer, num_ver);
        num_ver = strRmDuplicate(ppVer, num_ver);

        if(num_ver > MAX_APP_VERSIONS_ALLOWED) {
            for(i = MAX_APP_VERSIONS_ALLOWED; i < num_ver; i++) {
                ppVer[i] = NULL;
            }
            num_ver = MAX_APP_VERSIONS_ALLOWED;
        }
    }

    for(i = 0; i < num_ver; i++) {
        CHAR *ptr = NULL;
        ptr = strstr(ppVer[i], "-v");
        if(ptr) {
            ppVer[i] = ptr+2;
        }
    }

    return num_ver;
}

static INT32 rdmDwnlVACheckList(CHAR **ppVer, CHAR *pVerify, INT32 num_ver)
{
    INT32 i = 0;

    for(i = 0; i < num_ver; i++) {
        if(!strcmp(ppVer[i], pVerify)) {
            return 1;
        }
    }
    return 0;
}

static VOID rdmDwnlVAGetFinalVer(CHAR **ppJver, INT32 jnum,
                                 CHAR **ppIver, INT32 inum,
                                 CHAR **ppFver, INT32 *pFnum,
                                 CHAR **ppUver, INT32 *pUnum)
{
    INT32 i = 0;
    INT32 fin_list = 0;
    INT32 t_num = 0;

    for(i = 0; i < jnum; i++) {
        ppFver[i] = ppJver[i];
    }
    for(i = 0; i < inum; i++) {
        ppFver[i + jnum] = ppIver[i];
    }
    fin_list = (jnum + inum);

    if(fin_list > 1) {
        qsString(ppFver, fin_list);
        fin_list = strRmDuplicate(ppFver, fin_list);
    }
    if(fin_list > MAX_APP_VERSIONS_ALLOWED) {
        for(i = MAX_APP_VERSIONS_ALLOWED; i < fin_list; i++) {
            ppFver[i] = "";
        }
        fin_list = MAX_APP_VERSIONS_ALLOWED;
    }

    /* Check for the older apps to be uninstalled on the system */
    t_num = *pUnum;
    for(i = 0; i < inum; i++) {
        if(!rdmDwnlVACheckList(ppFver, ppIver[i], fin_list)) {
            ppUver[t_num++] = ppIver[i];
        }
    }

    *pUnum = t_num;

    /* Check for apps versions to be installed on the system */
    t_num = fin_list;
    for(i = 0; i < t_num; i++) {
        if(rdmDwnlVACheckList(ppIver, ppFver[i], inum)) {
            ppFver[i] = "";
            fin_list--;
        }
    }
    if(t_num > 1) {
        qsString(ppFver, t_num);
    }

    *pFnum = fin_list;
}

static VOID rdmDwnlVAUnInstall(RDMAPPDetails *pRdmAppDet, CHAR **ppVer, INT32 num_ver)
{
    INT32 i = 0;
    CHAR vdapp[MAX_BUFF_SIZE];
    CHAR vhapp[MAX_BUFF_SIZE];

    for(i = 0; i < num_ver; i++) {
        RDMInfo("Uninstall the %s version of %s\n", pRdmAppDet->app_name, ppVer[i]);
        snprintf(vdapp, MAX_BUFF_SIZE, "%s/v%s", pRdmAppDet->app_dwnl_path, ppVer[i]);
        snprintf(vhapp, MAX_BUFF_SIZE, "%s/v%s", pRdmAppDet->app_home, ppVer[i]);
        rdmDwnlUnInstallApp(vdapp, vhapp);
    }
    removeFile(pRdmAppDet->app_dwnl_path);
    removeFile(pRdmAppDet->app_home);
}

static INT32 rdmDwnlVAInstall(RDMAPPDetails *pRdmAppDet, CHAR **ppVer, INT32 num_ver)
{
    INT32 i = 0;
    INT32 dlen = 0;
    INT32 hlen = 0;

    dlen = strlen(pRdmAppDet->app_dwnl_path);
    hlen = strlen(pRdmAppDet->app_home);

    for(i = 0; i < num_ver; i++) {
        RDMInfo("Install the %s version of %s\n", pRdmAppDet->app_name, ppVer[i]);
        pRdmAppDet->app_dwnl_path[dlen] = 0;
        pRdmAppDet->app_home[hlen] = 0;
        strcat(pRdmAppDet->app_dwnl_path, "/v");
        strcat(pRdmAppDet->app_dwnl_path, ppVer[i]);
        strcat(pRdmAppDet->app_home, "/v");
        strcat(pRdmAppDet->app_home, ppVer[i]);
        strcat(pRdmAppDet->app_home, "/package");

        if(rdmDownloadMgr(pRdmAppDet)) {
            RDMError("Failed to Install the App %s version of %s\n", pRdmAppDet->app_name, ppVer[i]);
        }
    }

    pRdmAppDet->app_dwnl_path[dlen] = 0;
    pRdmAppDet->app_home[hlen] = 0;

    return RDM_SUCCESS;
}

static VOID rdmDwnlVAGetDetails(RDMAPPDetails *pRdmAppDet,
                                CHAR **ppJver, INT32 *pJnum,
                                CHAR **ppIver, INT32 *pInum,
                                CHAR **ppFver, INT32 *pFnum,
                                CHAR **ppUver, INT32 *pUnum)
{
    INT32 jnum_ver = 0;
    INT32 inum_ver = 0;
    INT32 fnum_ver = 0;
    INT32 unum_ver = 0;

    /* Get the version details from the mainfest */
    jnum_ver = rdmDwnlVAGetManifetVer(pRdmAppDet->pkg_ver, ppJver);

    /* Get Already installed version details */
    rdmDwnlVAGetInstallVer(pRdmAppDet,
                           ppIver, &inum_ver,
                           ppUver, &unum_ver);

    /* Get version details to be installed and un-installed*/
    rdmDwnlVAGetFinalVer(ppJver, jnum_ver,
                         ppIver, inum_ver,
                         ppFver, &fnum_ver,
                         ppUver, &unum_ver);
    *pJnum = jnum_ver;
    *pInum = inum_ver;
    *pFnum = fnum_ver;
    *pUnum = unum_ver;
}

INT32 rdmDownloadVerApp(RDMAPPDetails *pRdmAppDet)
{
    INT32 status = RDM_SUCCESS;
    CHAR *jver_list[RDM_MAX_VER_LIST] = {0};
    CHAR *iver_list[RDM_MAX_VER_LIST] = {0};
    CHAR *fver_list[RDM_MAX_VER_LIST] = {0};
    CHAR *uver_list[RDM_MAX_VER_LIST] = {0};
    INT32 jnum_ver = 0;
    INT32 inum_ver = 0;
    INT32 fnum_ver = 0;
    INT32 unum_ver = 0;

    RDMInfo("Version App download\n");

    rdmDwnlVAGetDetails(pRdmAppDet,
                        jver_list, &jnum_ver,
                        iver_list, &inum_ver,
                        fver_list, &fnum_ver,
                        uver_list, &unum_ver);

    /* Uninstall the apps */
    if(unum_ver > 0) {
        rdmDwnlVAUnInstall(pRdmAppDet, uver_list, unum_ver);
    }

    /* Install the apps */
    if(fnum_ver > 0) {
        status = rdmDwnlVAInstall(pRdmAppDet, fver_list, fnum_ver);
    }

    return status;
}
