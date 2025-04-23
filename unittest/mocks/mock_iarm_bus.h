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

#ifndef MOCK_IARM_BUS_H
#define MOCK_IARM_BUS_H

#include <gmock/gmock.h>
#include "rdm_types.h" // Ensure this header includes the definition for IARM_Result_t and other types

extern "C" {
    #include "mocks/libIBus.h"
}

class MockIARM {
public:
    MOCK_METHOD(IARM_Result_t, IARM_Bus_Init, (const char*), ());
    MOCK_METHOD(IARM_Result_t, IARM_Bus_Connect, (), ());
    MOCK_METHOD0(IARM_Bus_Disconnect, IARM_Result_t());
    MOCK_METHOD0(IARM_Bus_Term, IARM_Result_t());
    MOCK_METHOD(IARM_Result_t, IARM_Bus_BroadcastEvent, (const char*, IARM_EventId_t, void*, size_t), ());
};

#endif // MOCK_IARM_BUS_H
