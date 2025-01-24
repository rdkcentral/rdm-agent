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

#ifndef _RDM_JSONQUERY_H_
#define _RDM_JSONQUERY_H_

#define RDM_MANIFEST      "/etc/rdm/rdm-manifest.json"
//#define RDM_MANIFEST      "/tmp/usb/rdm/rdm-manifest.json"

#define RDM_MANIFEST_PATH  "//packages/"
#define RDM_JSONQ_APPNAME  "app_name"
#define RDM_JSONQ_DOND     "dwld_on_demand"
#define RDM_JSONQ_DMC      "dwld_method_controller"
#define RDM_JSONQ_PKGNAME  "pkg_name"
#define RDM_JSONQ_PKGTYPE  "pkg_type"
#define RDM_JSONQ_VER      "pkg_version"
#define RDM_JSONQ_APPSIZE  "app_size"
#define RDM_JSONQ_ISVER    "is_versioned"

INT32 rdmJSONGetLen(CHAR const* pfName,
                   INT32 *pLen);

INT32 rdmJSONQuery(CHAR const* pfName,
                   CHAR const* pPath,
                   CHAR *pOut);

INT32 rdmJSONGetAppDetID(INT32          idx,
                         RDMAPPDetails *pRdmAppDet);

INT32 rdmJSONGetAppDetName(CHAR          *pName,
                           RDMAPPDetails *pRdmAppDet);

INT32 rdmJSONGetAppNames(INT32          idx,
                         CHAR *pAppName);
#endif //_RDM_JSONQUERY_H_
