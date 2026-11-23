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

// Link-time forwarders to MockRdmUtils for symbols referenced by rdm_utils.c but defined elsewhere.
#include "mock_rdm_utils.h"

MockRdmUtils* mockRdmUtils = nullptr;

extern "C" {
INT32 rdmDwnlIsOCSPEnable(void) { return mockRdmUtils ? mockRdmUtils->rdmDwnlIsOCSPEnable() : 0; }
void* doCurlInit() { return mockRdmUtils ? mockRdmUtils->doCurlInit() : NULL; }
INT32 doHttpFileDownload(void *in_curl, FileDwnl_t *pfile_dwnl, MtlsAuth_t *auth, unsigned int max_dwnl_speed, char *dnl_start_pos, int *out_httpCode) {
    return mockRdmUtils ? mockRdmUtils->doHttpFileDownload(in_curl, pfile_dwnl, auth, max_dwnl_speed, dnl_start_pos, out_httpCode) : 0;
}
void doStopDownload(void *curl) { if (mockRdmUtils) mockRdmUtils->doStopDownload(curl); }
void t2CountNotify(char *marker, int val) { if (mockRdmUtils) mockRdmUtils->t2CountNotify(marker, val); }
void t2ValNotify(char *marker, char *val) { if (mockRdmUtils) mockRdmUtils->t2ValNotify(marker, val); }
}
