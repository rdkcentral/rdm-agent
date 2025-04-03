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
