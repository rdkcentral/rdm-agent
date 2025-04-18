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
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#ifndef GTEST_ENABLE
#include "rbus.h"
#include "rdm_types.h"
#include "rdm_rbus.h"
#include "rdm_utils.h"
#else
#include "unittest/mocks/mock_rdm_rbus.h"
#endif
#include "rdm.h"
/** @brief This Function initializes rbus.
 *
 *  @param[in/out]  ppRDMRbusHandle  rbus handle
 *  @param[in/out]  rbusName	     rbus Name
 *
 *  @return  Returns the status of the operation.
 *  @retval  Returns RDM_SUCCESS on success, RDM_FAILURE otherwise.
 */
INT32 rdmRbusInit(VOID **pRDMRbusHandle, INT8* rbusName)
{
    INT32 rc  = RBUS_ERROR_SUCCESS;
    INT32 ret = RDM_SUCCESS;
#ifndef GTEST_ENABLE
    rbusHandle_t handle;

    if (pRDMRbusHandle == NULL ) {
        RDMError("pRDMRbusHandle is NULL \n");
        return RDM_FAILURE;
    }

    if (rbusName == NULL) {
       RDMError("rbusName is NULL\n" );
	   return RDM_FAILURE;
    }

    rc = rbus_checkStatus();
    if(RBUS_ENABLED != rc) {
        RDMError("rbus is not active: %d\n", rc);
        return RDM_FAILURE;
    }

    rc = rbus_open(&handle, rbusName);
    if(rc != RBUS_ERROR_SUCCESS) {
        RDMError("rbus_open failed: %d\n", rc);
        ret = RDM_FAILURE;
    }

    *pRDMRbusHandle = (VOID*)handle;
#endif
    return ret;
}

/** @brief This Function get RFC details.
 *
 *  @param[in/out]  None
 *
 *  @return  Returns the status of the operation.
 *  @retval  Returns RDM_SUCCESS on success, RDM_FAILURE otherwise.
 */
INT32 rdmRbusGetRfc(VOID *plRDMRbusHandle, INT8* rdmRFCName, VOID *pValue )
{
    INT32           ret = RDM_SUCCESS;
    INT32           rc  = RBUS_ERROR_SUCCESS;
    INT32          *rdm_enb = pValue;
    INT8           *rdm_ver = pValue;
    rbusValue_t     paramValue = NULL;
    rbusValueType_t rbusValueType;
    INT8           *stringValue = NULL;
    rbusHandle_t    handle = plRDMRbusHandle;

    if(handle == NULL) {
        RDMError("Invalid RFC handle\n");
        return RDM_FAILURE;
    }

    if(rdm_enb == NULL) {
        RDMError("Input param is null\n");
        return RDM_FAILURE;
    }

    if (rdmRFCName == NULL){
        RDMError("RFC name is null\n");
        return RDM_FAILURE;
    }

    *rdm_enb = 0;

    rc = rbus_get(handle, rdmRFCName, &paramValue);
    if(rc != RBUS_ERROR_SUCCESS) {
        RDMError("Unable to get: %s rc =%d \n", rdmRFCName, rc);
        ret = RDM_FAILURE;
        goto exit;
    }
    rbusValueType = rbusValue_GetType(paramValue);
    if(rbusValueType == RBUS_BOOLEAN) {
        if (rbusValue_GetBoolean(paramValue)){
            *rdm_enb = 1;
        }
    }
    else if(rbusValueType == RBUS_STRING){
        stringValue   = rbusValue_ToString(paramValue, NULL, 0);
        if(stringValue == NULL) {
            RDMError("Unable to get url\n");
            ret = RDM_FAILURE;
            goto exit;
        }
        else {
            strcpy( rdm_ver, stringValue);
            RDMInfo("RDM URL: %s\n", stringValue);
        }
    }

exit:
    if(paramValue) {
        rbusValue_Release(paramValue);
        paramValue = NULL;
    }
    if(stringValue) {
	free(stringValue);
	stringValue = NULL;
    }
    return ret;
}

/** @brief This Function is for uninit.
 *
 *  @param[in/out]  None
 *
 *  @return  Returns the status of the operation.
 *  @retval  Returns RDM_SUCCESS on success, RDM_FAILURE otherwise.
 */
VOID rdmRbusUnInit(VOID *pRDMbusHandle)
{
    INT32 rc  = RBUS_ERROR_SUCCESS;
#ifndef GTEST_ENABLE
    rbusHandle_t    handle = pRDMbusHandle;

    if(handle == NULL) {
        RDMError("Invalid rbus handle\n");
        return;
    }

    rc = rbus_close(handle);
    if(rc != RBUS_ERROR_SUCCESS) {
       RDMError("Unable to Close receiver bus: %d\n", rc);
    }
#endif
}
