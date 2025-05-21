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

#ifndef MOCK_RDM_RBUS_H
#define MOCK_RDM_RBUS_H

#include <gmock/gmock.h>
#include "mock_rdm_rbustypes.h"

class MockRdmRbus {
public:
    MOCK_METHOD(rbusValueType_t, rbusValue_GetType, (void*), ());
    MOCK_METHOD(INT32, rbus_get, (void*, INT8*, void*), ());
    MOCK_METHOD(const INT8*, rbusValue_ToString, (void*, void*, int), ());
    MOCK_METHOD(bool, rbusValue_GetBoolean, (void*), ());
    MOCK_METHOD(void, rbusValue_Release, (void*), ());
    MOCK_METHOD(INT32, rbus_checkStatus, (), ());
    MOCK_METHOD(INT32, rbus_open, (void*, INT8*), ());
    MOCK_METHOD(INT32, rbus_close, (void*), ());
};

#endif // MOCK_RDM_RBUS_H

