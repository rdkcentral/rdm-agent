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

#ifndef _RDM_RBUS_H_
#define _RDM_RBUS_H_

#define RDM_RFC_URL        "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CDLDM.CDLModuleUrl"
#define RDM_RFC_MTLS       "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MTLS.mTlsXConfDownload.Enable"
#define RDM_CODEBIG_STATUS "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CodeBigFirst.Enable"
#define RDM_INIT_NAME "RFCCHECK"

enum rfcType
{
   RFC_TYPE_BOOLEAN,
   RFC_TYPE_STRING
};

INT32 rdmRbusInit(VOID **pRDMRbusHandle, INT8* rbusName);
INT32 rdmRbusGetRfc(VOID *plRDMRbusHandle, INT8* rdmRFCName, VOID *pValue);
VOID  rdmRbusUnInit(VOID *pRDMbusHandle);
#endif //_DCM_RBUS_H_
