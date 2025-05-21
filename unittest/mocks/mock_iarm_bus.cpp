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

#include "unittest/mocks/mock_iarm_bus.h"
#include <gmock/gmock.h>// Include Google Mock


extern "C" { // Important: Keep the extern "C"
    IARM_Result_t IARM_Bus_Init(const char* name) {
        extern MockIARM* mockIARM;
        return mockIARM->IARM_Bus_Init(name);
    }

    IARM_Result_t IARM_Bus_Connect() {
        extern MockIARM* mockIARM;
        return mockIARM->IARM_Bus_Connect();
    }

    IARM_Result_t IARM_Bus_Disconnect() {
        extern MockIARM* mockIARM;
        return mockIARM->IARM_Bus_Disconnect();
    }

    IARM_Result_t IARM_Bus_Term() {
        extern MockIARM* mockIARM;
        return mockIARM->IARM_Bus_Term();
    }

    IARM_Result_t IARM_Bus_BroadcastEvent(const char* owner, IARM_EventId_t eventId, void* data, size_t len) {
        extern MockIARM* mockIARM;
        return mockIARM->IARM_Bus_BroadcastEvent(owner, eventId, data, len);
    }
} // end extern "C"
#ifdef GTEST_ENABLE  // Important: Keep this conditional compilation!

// The following is empty, but you can add some helper functions here if we want further
#endif
